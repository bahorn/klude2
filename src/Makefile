loader: lkm-loader-build
	make -C ./loader-bin/

lkm-loader-build: sample
	make -C ./loader/ build
	cp ./loader/src/kshelf-loader.ko /workdir/artifacts
	./kshelf-utils/xxd.py ./loader/src/kshelf-loader.ko kshelf_loader \
		> /workdir/artifacts/kshelf_loader.h

sample: stubgen
	make -C ./sample/

stubgen:
	python3 ./kshelf-utils/stubgen.py /workdir/artifacts/kallsyms | sort | uniq \
		> /workdir/artifacts/stubs.c
	$(CC) -Wno-builtin-declaration-mismatch -fno-plt -nostdlib -fPIE -shared -o /workdir/artifacts/stub_lib /workdir/artifacts/stubs.c
