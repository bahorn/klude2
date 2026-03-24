FROM ubuntu:24.04

ARG KVERSION=custom
ENV KPATH=/kernel

RUN apt update && \
    apt -y upgrade && \
    apt install -y build-essential xxd python3 libelf-dev

RUN mkdir -p /build /workdir

CMD cp -r /build/src /workdir/src && cd /workdir && make -C src
