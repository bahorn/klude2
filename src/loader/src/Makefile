TARGET = kshelf-loader
ccflags-y := -std=gnu99 \
	-Wno-declaration-after-statement

obj-m += $(TARGET).o

kshelf-loader-m += ./util.o ./shelf.o ./main.o
