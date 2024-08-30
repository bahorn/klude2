DISTRO=ubuntu-2204
KVERSION=5.15.0-119-generic
TESTHOST=a@192.168.122.207

#DISTRO=ubuntu-2404
#KVERSION=6.8.0-41-generic
#TESTHOST=a@192.168.122.201


build:
	./build-system/build.sh $(DISTRO) $(KVERSION)
	scp ./artifacts/loader.bin $(TESTHOST):~/

setup:
	docker build \
		--build-arg="KVERSION=$(KVERSION)" \
		-t bahorn/klude2-build:$(DISTRO)-$(KVERSION) \
		./build-system/$(DISTRO)
