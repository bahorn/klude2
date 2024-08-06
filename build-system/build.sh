mkdir -p artifacts && \
    docker run \
        --rm \
        -v `pwd`/src:/build/src:ro \
        -v `pwd`/artifacts:/workdir/artifacts:rw \
        -e KVERSION=$1 \
        bahorn/klude2-build:$1
