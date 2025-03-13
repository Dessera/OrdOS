# KDemo

My os development demo.

For now, there is only a bootsector in this repository.

## TODOS

1. [ ] User space
2. [ ] Memory allocation
3. [ ] File system
4. [ ] Shell
5. [ ] Support more gcc versions and std versions

## Build

> The project uses gcc14 (with c23 support) to build.

Build commands:

```bash
make all          # Build all
make clean        # Clean all
make rebuild      # Clean and build all
make qemu         # Build and run qemu
make qemu DEBUG=  # Build and run qemu with no debug flags (-s -S)
```

Testers:

```bash
make test                     # Build all testers
make TESTER=<name> qemu_test  # Build and run a tester
```

## Thanks

- A os development tutorial from Charliechen114514: [CCOperateSystem](https://github.com/Charliechen114514/CCOperateSystem)
