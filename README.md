# klude2 - kSHELF loader

This is an updated and simplified version of my project `klude`.

This only targets Ubuntu 24.04, but is trival to port to older versions of
Ubuntu and other distros with some minor work.

## Usage

First, we need to get a copy of the kallsyms from the target kernel:
```
mkdir ./artifacts/
cp /proc/kallsyms ./artifacts/
```

It doesn't need to be up to date, just needs to have the symbols you want to
link against in it.

Then you need to edit 2 lines in the `Makefile`:
```
KVERSION=the_target_kernel
TESTHOST=your_test_host
```

The target kernel is just the output of `uname -r` and the test host is just the
box to scp the output loader on it for testing.

Build the build container:
```
make setup
```

And finally, build the project and copy the loader over:
```
make build
```
