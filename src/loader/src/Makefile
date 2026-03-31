TARGET = kshelf-loader
ccflags-y := -std=gnu99 -Wno-declaration-after-statement

obj-m += $(TARGET).o
$(TARGET)-objs := util.o shelf.o main.o ../../../artifacts/payload.o
