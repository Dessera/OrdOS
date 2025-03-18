# Ordos

My os development demo.

## TODOS

1. [ ] User space
2. [ ] More log info
3. [ ] Memory allocation
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

## Thanks

- A os development tutorial from Charliechen114514: [CCOperateSystem](https://github.com/Charliechen114514/CCOperateSystem)
