FROM fedora:40

ARG KVERSION=6.10.6-200.fc40
ENV KPATH=/usr/src/kernels/

RUN dnf -y update && \
    dnf -y install make automake gcc gcc-c++ kernel-devel-$KVERSION xxd python3

RUN mkdir -p /build /workdir

CMD cp -r /build/src /workdir/src && cd /workdir && make -C src
