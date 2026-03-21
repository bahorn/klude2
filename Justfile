setup distro kversion:
    docker build \
        --build-arg="KVERSION={{ kversion }}" \
        -t klude2-build:{{ distro }}-{{ kversion }} \
        ./build-system/{{ distro }}


build distro kversion:
    mkdir -p artifacts && \
        docker run \
        --rm \
        -v `pwd`/src:/build/src:ro \
        -v `pwd`/artifacts:/workdir/artifacts:rw \
        -e KVERSION={{ kversion }} \
        klude2-build:{{ distro }}-{{ kversion }}

together distro kversion:
    just setup {{ distro }} {{ kversion }}
    just build {{ distro }} {{ kversion }}

copy remote_host:
    scp ./artifacts/loader.bin {{ remote_host }}:/tmp
