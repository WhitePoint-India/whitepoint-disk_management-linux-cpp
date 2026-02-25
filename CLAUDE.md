# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Initial setup (required once)
git submodule update --init --recursive

# Build from example directory
cd example
make                      # Debug build
make BUILD_TYPE=Release   # Release build
make clean               # Clean build artifacts
make rebuild             # Clean and rebuild
make verbose             # Build with verbose output

# Run (requires sudo for hardware access)
sudo ./build/example

# Debug
make debug               # Launch lldb debugger
```

## Architecture Overview

C++23 disk management library for Linux providing NIST 800-88 compliant disk sanitization. Uses `std::variant`-based polymorphism for disk types and a delegate pattern for progress reporting.

### Disk Type Hierarchy

```
Disk (base class, private members with getters)
├── Disks::ATADisk    (SATA/IDE, frozen state detection via ioctl)
├── Disks::NVMeDisk   (NVMe SSDs)
└── Disks::USBDisk    (USB storage)
```

Disk instances are stored as `DiskVariant = std::variant<ATADisk, NVMeDisk, USBDisk>` (defined in `disks.hpp`). Access common `Disk&` via `std::visit`.

### Sanitization Method Pattern

All 16 delete methods use a **CRTP singleton** pattern via `SingletonMethod<Derived>` (defined in `operations.hpp`):

```
DiskDeleteMethod (abstract base, virtual deleteDisk overloads per disk type)
└── SingletonMethod<T> (CRTP: deleted copy/move, static shared(), Meyers singleton)
    ├── ZeroWrite, RandomWrite, RandomZeroWrite
    ├── NIST80088Clear, NIST80088Purge
    ├── DoD522022M, DoD522028M, AFSSI5020
    ├── NAVSOP523926MFM, NAVSOP523926RLL
    ├── NSALegacy, NSAModern, BSIVSITR
    ├── Gutmann, SecureErase, EnhancedSecureErase
```

To add a new method: create header inheriting `SingletonMethod<NewMethod>` with `friend class SingletonMethod<NewMethod>`, implement private constructor calling `SingletonMethod("ID")`, override `getTitle()`, `getDescription()`, `getStages()`, and the relevant `deleteDisk()` overloads. Register in `disk_management.cpp` methods vector.

### Delegate Pattern for Progress

`DiskDeleteMethod::Delegate` is a pure virtual interface for progress reporting:
- `onStageStarted(Stage)` / `onStageCompleted(Stage)` — stage lifecycle
- `onProgress(Stage, Progress)` — byte-level progress within a stage
- `onCompleted()` / `onError(string)` — operation result
- `shouldCancel()` — cooperative cancellation (default returns false)

`Stage` uses `std::string_view` for id/name. `Progress` provides `fractionCompleted()` and `percentageCompleted()`.

### Hardware Detection

`DiskManagement::fetchDisks()` in `disk_management.cpp`:
1. Creates lshw `hwNode`, calls `scan_system()`
2. Recursively walks the hardware tree looking for `hw::disk` nodes
3. Classifies by parent capabilities: "nvme" -> NVMeDisk, "sata" -> ATADisk, "usb" -> USBDisk
4. Falls back to node description matching if capabilities are empty
5. Filters out NVMe hwmon and `/dev/ng*` character devices
6. Returns `std::vector<DiskVariant>`

### Key Files

| File | Purpose |
|------|---------|
| `include/disk.hpp` | Base `Disk` class with private members and `[[nodiscard]]` getters |
| `include/disks.hpp` | `ATADisk`, `NVMeDisk`, `USBDisk` + `DiskVariant` typedef |
| `include/operations.hpp` | `Stage`, `Progress`, `DiskDeleteMethod`, `Delegate`, `SingletonMethod<T>` |
| `include/disk_management` | Public API header: `fetchDisks()` + `methods` vector |
| `src/disk_management.cpp` | lshw integration, hardware detection, method registry |
| `src/operations.cpp` | `Progress` and `DiskDeleteMethod` base implementations |

### External Dependencies

- **external/lshw/**: Git submodule, built as static lib. lshw headers are PRIVATE to `disk_management` target (not leaked to consumers). Requires a global `void status(const char*)` stub.
- **CMake 3.11+**: Build system with FetchContent support.

## Development Notes

- **C++ Standard**: C++23 (`cxx_std_23` in both library and example CMakeLists)
- **Privileges**: Root access (sudo) required for all hardware operations
- **Sector sizes**: `unsigned int` (not signed), with zero-division guard in `getSectorCount()`
- **Const correctness**: All getters are `const`, `isFrozen()` is `const`, `methods` vector is `const`
- **`[[nodiscard]]`**: Applied to all query methods on `Disk`, `Progress`, `DiskDeleteMethod`, and `Delegate::shouldCancel()`

## Critical Warnings

- This library performs **permanent, irreversible data destruction**
- NVMe sanitize operations cannot be stopped even by power cycle
- Root privileges required for all sanitization operations
- Most delete methods (14/16) currently have stub I/O implementations (TODO comments only)
- `ATADisk::isFrozen()` uses raw file descriptors without RAII
