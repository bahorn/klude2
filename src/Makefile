loader: lkm-loader-build
	make -C ./loader-bin/

lkm-loader-build: sample
	make -C ./loader/ build
	cp ./loader/src/kshelf-loader.ko /workdir/artifacts
	xxd -n kshelf_loader -i ./loader/src/kshelf-loader.ko \
		> /workdir/artifacts/kshelf_loader.h

sample: stubgen
	make -C ./sample/

stubgen:
	python3 ./kshelf-utils/stubgen.py ./kshelf-utils/kallsyms | sort | uniq \
		> /workdir/artifacts/stubs.c
	$(CC) -Wno-builtin-declaration-mismatch -fno-plt -nostdlib -fPIE -shared -o /workdir/artifacts/stub_lib /workdir/artifacts/stubs.c
