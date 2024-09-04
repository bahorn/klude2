build:
	./build-system/build.sh $(DISTRO) $(KVERSION)
	@if [ $(TESTHOST) ]; then \
		scp ./artifacts/loader.bin $(TESTHOST):~/; \
	fi

setup:
	docker build \
		--build-arg="KVERSION=$(KVERSION)" \
		-t bahorn/klude2-build:$(DISTRO)-$(KVERSION) \
		./build-system/$(DISTRO)
