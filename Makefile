KVERSION=6.8.0-39-generic


build:
	./build-system/build.sh $(KVERSION)
	scp ./artifacts/loader.bin a@192.168.122.201:~/

setup:
	docker build \
		--build-arg="KVERSION=$(KVERSION)" \
		-t bahorn/klude2-build:$(KVERSION) \
		./build-system/
