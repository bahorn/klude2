#KVERSION=5.15.0-91-generic
#TESTHOST=a@192.168.122.207

KVERSION=6.8.0-41-generic
TESTHOST=a@192.168.122.201


build:
	./build-system/build.sh $(KVERSION)
	scp ./artifacts/loader.bin $(TESTHOST):~/

setup:
	docker build \
		--build-arg="KVERSION=$(KVERSION)" \
		-t bahorn/klude2-build:$(KVERSION) \
		./build-system/
