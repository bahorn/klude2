#include <linux/module.h>
#include <linux/kernel.h>

long i = 0x1337;
long j = 0;
char uwu[] = "magic test please appear\0";
char uwu2[] = "magictest\0";

int main(void)
{
    printk("%lx\n", &uwu[0]);
    printk("%i\n", uwu2[0]);
    i += 1;
    j = i;
    printk("%li\n", i);
    printk("Greetings from SHELF\n");
    printk("%s\n", uwu);
    return 0;
}
