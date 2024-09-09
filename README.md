# klude2 - kSHELF loader

This is an updated and simplified version of my project `klude`.

This repo supports / tested on the following distros for the loader:
* Ubuntu 22.04
* Ubuntu 24.04
* Fedora 40

I have tested the resulting kSHELFs on various kernel versions built with 
easylkb[1] via another loader project.
Only thing to note in that case is be aware of kernel changes, i.e vmalloc
becoming a macro in 6.10, etc.

## Usage

First, we need to get a copy of the kallsyms from the target kernel:
```
mkdir ./artifacts/
cp /proc/kallsyms ./artifacts/
```

It doesn't need to be up to date, just needs to have the symbols you want to
link against in it.
You can use a tool like `vmlinux-to-elf` to extract kallsyms from the kernel
bzImage you are targeting.

Set the following environment variables:
```
DISTRO=disto_to_build_against
KVERSION=the_target_kernel
TESTHOST=your_test_host
```

The target kernel is just the output of `uname -r` and the test host is just the
box to scp the output loader to for testing.
If unset, a binary will just be written to `artifacts`.
Distro is used to specify which Dockerfile to use in `./build-system/`.

Build the build container:
```
make setup
```

And finally, build the project and copy the loader over:
```
make build
```

## License

GPL2

## References

* [1] https://github.com/deepseagirl/easylkb
* [2] https://github.com/marin-m/vmlinux-to-elf
