# KDemo

My os development demo.

For now, there is only a bootsector in this repository.

## TODOS

1. [x] `bootsec.S` refactoring
2. [x] Interupts (no devices support yet)
3. [x] Kernel library (partial)
4. [x] Memory management (page only)

   - [x] page management
   - [ ] a simple memory allocator

5. [ ] File system
6. [x] Task management (kernel thread only)

   - [x] lock
   - [x] thread safe kprint
   - [x] thread safe list_head
   - [ ] task functions refactoring

7. [ ] Shell
8. [ ] Support more gcc versions and std versions

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

- [WIP] A os development tutorial from Charliechen114514: [CCOperateSystem](https://github.com/Charliechen114514/CCOperateSystem)
