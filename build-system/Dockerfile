FROM ubuntu:24.04

ARG KVERSION=6.8.0-39-generic

RUN apt update && \
    apt -y upgrade && \
    apt install -y build-essential linux-headers-$KVERSION xxd python3

RUN mkdir -p /build /workdir

CMD cp -r /build/src /workdir/src && cd /workdir && make -C src
