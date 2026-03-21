# klude2 - kSHELF loader

This is an updated and simplified version of my project `klude`[1]

This repo supports / tested on the following distros for the loader:
* Ubuntu 22.04
* Ubuntu 24.04
* Fedora 40

I have tested the resulting kSHELFs on various kernel versions built with 
easylkb[2].
Only thing to note in that case is be aware of kernel changes, i.e vmalloc
becoming a macro in 6.10, etc.

This is used in my project [skp](https://github.com/bahorn/skp) and as the
payload for my [UEFI bootkit](https://blog.b.horn.uk/posts/grabit/).

Check out my article in [tmp.0ut #4](https://tmpout.sh/4/5.html) for an explainer on how this works.

## Usage

First, we need to get a copy of the kallsyms from the target kernel:
```
mkdir ./artifacts/
cp /proc/kallsyms ./artifacts/
```

It doesn't need to be up to date, just needs to have the symbols you want to
link against in it.
You can use a tool like `vmlinux-to-elf`[3] to extract kallsyms from the kernel
bzImage you are targeting.


Then just do the following to copy it to /tmp on a target host:
```
just together ubuntu-2404 6.8.0-106-generic
just copy ubuntu-24.04-testing.vm # your host
```

The ubuntu-2404 argument is the container to use in `/build-system`

## License

GPL2.

Some code in `src/sample/` is derived from xcellerator's tutorial series [4],
which the code is also under GPL2.

## References

* [1] https://github.com/bahorn/klude
* [2] https://github.com/deepseagirl/easylkb
* [3] https://github.com/marin-m/vmlinux-to-elf
* [4] https://github.com/xcellerator/linux_kernel_hacking/tree/master/3_RootkitTechniques/3.3_set_root
