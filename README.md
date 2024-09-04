# klude2 - kSHELF loader

This is an updated and simplified version of my project `klude`.

This repo only supports Ubuntu 22.04 and 24.04, but I have tested the resulting
kSHELFs on various kernel versions built with easylkb[1] via another loader
project.

## Usage

First, we need to get a copy of the kallsyms from the target kernel:
```
mkdir ./artifacts/
cp /proc/kallsyms ./artifacts/
```

It doesn't need to be up to date, just needs to have the symbols you want to
link against in it.

Set the following environment variables:
```
DISTRO=disto_to_build_against
KVERSION=the_target_kernel
TESTHOST=your_test_host
```

The target kernel is just the output of `uname -r` and the test host is just the
box to scp the output loader on it for testing.
Distro is used to specify which Dockerfile to use in `./build-system/`.

Build the build container:
```
make setup
```

And finally, build the project and copy the loader over:
```
make build
```

## References

* [1] https://github.com/deepseagirl/easylkb
