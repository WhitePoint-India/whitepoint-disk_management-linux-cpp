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

This is a C++20 disk management library for Linux that provides hardware-level disk operations including secure erasure.

### Core Class Hierarchy
- **Base**: `DiskManagement::Disk` - Abstract base class for all disk types
- **Implementations**: `ATADisk` (SATA/IDE), `NVMeDisk` (NVMe SSDs), `USBDisk` (USB storage)
- **Erasure Strategies**: `SecureErase` (hardware-level), `GutmannMethod` (35-pass overwrite)

### Key Components
- **include/disk-management**: Single public header with complete API
- **src/disk-management.cpp**: Hardware detection using lshw library (parses XML output)
- **src/secure-erase.cpp**: ATA/NVMe secure erase implementation
- **src/gutmann-method.cpp**: Software-based multi-pass overwrite
- **external/lshw/**: Git submodule for hardware detection (filtered to remove GUI/DB dependencies)

### Hardware Detection Flow
1. `fetchDisks()` calls lshw with XML output format
2. XML is parsed to extract disk information (model, serial, logical name, bus type)
3. Disk objects are created based on bus type (NVMe, ATA, USB)
4. Unique_ptr is used for memory management

## Development Notes

- **C++ Standard**: C++20 required
- **Privileges**: Root access needed for hardware operations
- **Submodule**: lshw must be initialized before building
- **Build System**: CMake-based with example Makefile wrapper
- **Current Branch**: Working on linking/lshw branch for lshw integration

## Critical Warnings

- This library performs **permanent data destruction** operations
- Always test in safe environments first
- Secure erase implementations currently stubbed (TODO markers in code)
- Requires root privileges for hardware access