SRC_DIR=./src
OBJ_DIR=./obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

build: all.o
	../kshelf-utils/kshelf-ld -o /workdir/artifacts/main.bin ./all.o /workdir/artifacts/stub_lib
	# strip /workdir/artifacts/main.bin
	readelf -a ./all.o
	xxd -n payload -i /workdir/artifacts/main.bin > /workdir/artifacts/payload.h

all.o: $(OBJ_FILES)
	$(LD) -r -o all.o $(OBJ_FILES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	../kshelf-utils/kshelf-cc -I ./include -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
