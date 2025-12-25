build:
	make -C $(KPATH)$(KVERSION) M=`pwd`/src modules
	objcopy --remove-section=__mcount_loc -X --keep-global-symbol='' --strip-unneeded ./src/kshelf-loader.ko
	objcopy --strip-unneeded ./src/kshelf-loader.ko
	python3 tools/remove_syms.py ./src/kshelf-loader.ko


clean:
	make -C $(KPATH)$(KVERSION) M=`pwd`/src clean
