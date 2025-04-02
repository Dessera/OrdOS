# Ordos

My os development demo.

## TODOS

1. [x] User space
2. [x] More log info
3. [x] Memory allocation
4. [ ] File system
5. [ ] Shell
6. [ ] Support more gcc versions and std versions

## Build

> The project uses gcc14 (with c23 support) to build.

Build commands:

```bash
make all          # Build all
make clean        # Clean all
make rebuild      # Clean and build all
make qemu         # Run qemu
make qemu_debug   # Run qemu with debug flags
```

Testers:

```bash
make test                     # Build all testers
make TESTER=<name> qemu_test  # Build and run a tester
```

## Debug

When running `make all` or `make qemu_debug`, there will be a `ordos.debug` file in the `build` directory (which is a `elf32-i386` file). You can use `gdb` to debug it.

> Note that functions before `init_vpage` cannot be debugged because the kernel is not running in flat memory model before `init_vpage`.

## Thanks

- A os development tutorial from Charliechen114514: [CCOperateSystem](https://github.com/Charliechen114514/CCOperateSystem)
