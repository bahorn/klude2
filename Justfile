sample_dir := `pwd` + "/samples/basic-ftrace"

# setup a given distro build environment
setup distro kversion:
    docker build \
        --build-arg="KVERSION={{ kversion }}" \
        -t klude2-build:{{ distro }}-{{ kversion }} \
        ./build-system/{{ distro }}

# build against a distro container
build-distro distro kversion payload=sample_dir:
    mkdir -p artifacts && \
        docker run \
        --rm \
        -v `pwd`/src:/build/src:ro \
        -v `pwd`/artifacts:/workdir/artifacts:rw \
        -v {{ payload }}:/build/src/payload \
        -e KVERSION={{ kversion }} \
        klude2-build:{{ distro }}-{{ kversion }}

# this is for building against a local built kernel
build-path kpath payload=sample_dir:
    mkdir -p artifacts && \
        cp {{ kpath }}/System.map ./artifacts/kallsyms && \
        docker run \
        --rm \
        -v `pwd`/src:/build/src:ro \
        -v `pwd`/artifacts:/workdir/artifacts:rw \
        -v {{ payload }}:/build/src/payload:rw \
        -v {{ kpath }}:/kernel:ro \
        -e KVERSION="" \
        klude2-build:custom-custom

# setup and build against the given distro container
all-distro distro kversion payload=sample_dir:
    just setup {{ distro }} {{ kversion }}
    just build-distro {{ distro }} {{ kversion }} {{ payload }}

# setup and build against a kernel you have at a given path
all-path path payload=sample_dir:
    just setup custom custom
    just build-path {{ path }} {{ payload }}

copy remote_host:
    scp ./artifacts/loader.bin {{ remote_host }}:/tmp
