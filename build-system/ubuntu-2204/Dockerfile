FROM ubuntu:22.04

ARG KVERSION=5.15.0-119-generic
ENV KPATH=/usr/src/

RUN apt update && \
    apt -y upgrade && \
    apt install -y build-essential linux-headers-$KVERSION xxd python3

RUN mkdir -p /build /workdir

CMD cp -r /build/src /workdir/src && cd /workdir && make -C src
