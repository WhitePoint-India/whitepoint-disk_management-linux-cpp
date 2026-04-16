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

C++23 disk management library for Linux providing NIST 800-88 compliant disk sanitization. Uses virtual-dispatch polymorphism with capability interfaces for disk operations and a callback-based pattern for progress reporting.

### Public vs Internal Headers

Headers in `include/` are the public API. Headers in `include/internal/` are implementation details not exposed to consumers. The CMakeLists.txt enforces this: `include/` is a PUBLIC include directory, `include/internal/` is PRIVATE.

### Disk Type Hierarchy

Consumers see only the `Disk` base class (via `std::unique_ptr<Disk>`). Concrete disk types are internal:

```
Disk (base class, private members with getters)  [public: include/disk.hpp]
├── ATADisk  (+ BlockWritable + ATASecureErasable) [internal: include/internal/ata_disk.hpp]
└── NVMeDisk (+ BlockWritable + NVMeSanitizable)   [internal: include/internal/nvme_disk.hpp]
```

### Capability Interfaces

Sanitization methods use `dynamic_cast` to check disk capabilities rather than coupling to concrete types:

- **`BlockWritable`** — sector-level write access (`writeBlock()`, `getSectorCount()`, `getSectorSize()`)
- **`ATASecureErasable`** — ATA secure erase operations (`isFrozen()`, `unfreeze()`, `secureEraseUnit()`)
- **`NVMeSanitizable`** — NVMe sanitize/format commands (`nvmeSanitize()`, `nvmeFormatNVM()`)

### Sanitization Methods and Auto-Registration

Methods inherit from `DiskSanitizationInterface` (public API: `sanitize(Disk&, Callback)`) and use `AutoRegisterMethod<T>` to self-register into `SanitizationMethodRegistry` at construction time.

Each method is a **Meyers singleton** with deleted copy/move:

```
DiskSanitizationInterface (abstract base, virtual sanitize())  [public]
├── NISTClear           — NIST 800-88 Clear (3-stage)         [internal]
├── NISTPurge           — NIST 800-88 Purge                   [internal]
├── SecureErase         — ATA Secure Erase                    [internal]
└── EnhancedSecureErase — ATA Enhanced Secure Erase           [internal]
```

**Static library linker issue**: Because methods auto-register via static initialization, and static libraries drop translation units with no unresolved symbols, `disk_management.cpp` acts as a composition root — it explicitly calls each method's `shared()` to force construction.

To add a new method:
1. Create header in `include/internal/` inheriting `DiskSanitizationInterface` and `private AutoRegisterMethod<NewMethod>`
2. Implement private constructor calling `DiskSanitizationInterface("KEY")` and `AutoRegisterMethod(*this)`
3. Add static `shared()` returning a Meyers singleton
4. Override `sanitize(Disk&, Callback)`
5. Add source file to `CMakeLists.txt`
6. Add `#include` and `shared()` call in `disk_management.cpp`'s `ensureMethodsRegistered()`

### Progress Reporting

Progress uses a callback (`std::function<void(const SanitizationProgress)>`) instead of a delegate interface:

- **`SanitizationCallback`** — typedef for `std::function<void(const SanitizationProgress)>`
- **`SanitizationProgress`** — encapsulates stage reference, index/total counts, and fraction/percentage completed
- **`SanitizationStage`** — abstract base with `title()` and `description()`
- **`LocalizableSanitizationStage`** — extends `SanitizationStage` with `localizedTitle()` and `localizedDescription()`

Methods define their own `Stage` inner classes (e.g., `NISTClear::Stage`) inheriting `LocalizableSanitizationStage` with an enum of stage values.

### Hardware Detection

`HardwareDetector` is an abstract interface (`include/hardware_detector.hpp`). The default implementation `LshwHardwareDetector` (`include/internal/lshw_hardware_detector.hpp`) uses the lshw library:

1. Creates lshw `hwNode`, calls `scan_system()`
2. Recursively walks the hardware tree looking for `hw::disk` nodes
3. Classifies by parent capabilities: "nvme" -> NVMeDisk, "sata" -> ATADisk
4. Falls back to node description matching if capabilities are empty
5. Filters out NVMe hwmon and `/dev/ng*` character devices
6. Returns `std::vector<std::unique_ptr<Disk>>`

`DiskManagement::fetchDisks()` has two overloads: a default one using `LshwHardwareDetector`, and one accepting any `HardwareDetector&` for testing/injection.

### External Dependencies

- **external/lshw/**: Git submodule, built as static lib. lshw headers are PRIVATE to `disk_management` target (not leaked to consumers). Requires a global `void status(const char*)` stub (defined in `lshw_hardware_detector.cpp`).
- **CMake 3.11+**: Build system with FetchContent support.

## Development Notes

- **C++ Standard**: C++23 (`cxx_std_23` in both library and example CMakeLists)
- **Privileges**: Root access (sudo) required for all hardware operations
- **Sector sizes**: `unsigned int` (not signed), with zero-division guard in `getSectorCount()`
- **Const correctness**: All getters are `const`, `methods` vector returns `const&`
- **`[[nodiscard]]`**: Applied to all query methods on `Disk`, `SanitizationProgress`, `DiskSanitizationInterface`, and `HardwareDetector`
- **Compiler warnings**: `-Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor -Woverloaded-virtual -Wcast-align -Wconversion -Wsign-conversion`

## Critical Warnings

- This library performs **permanent, irreversible data destruction**
- NVMe sanitize operations cannot be stopped even by power cycle
- Root privileges required for all sanitization operations
- Most sanitization methods currently have stub I/O implementations
