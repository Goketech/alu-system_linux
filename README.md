# ALU System Linux

A collection of low-level binary analysis tools for Linux, implementing core functionality of GNU binutils in C. This project provides educational implementations of ELF binary inspection utilities.

## Project Structure

```
alu-system_linux/
├── README.md                 # This file
└── nm_objdump/               # ELF symbol table and object dump utilities
    ├── README.md             # Module-specific documentation
    ├── Makefile              # Build configuration
    ├── hnm.c                 # nm implementation entry point
    ├── hobjdump.c            # objdump implementation entry point
    ├── hnm.h                 # nm module headers
    ├── hobjdump.h            # objdump module headers
    └── *.c                   # Implementation modules
```

## Tools Included

### hnm - Symbol Table Dumper
A clone of the GNU `nm` command that lists symbols from ELF object files.
- Displays symbol names, values, and types
- Supports both 32-bit and 64-bit ELF files
- Handles little-endian and big-endian architectures
- Processes multiple files in a single invocation

### hobjdump - Object File Dumper
A clone of the GNU `objdump -sf` command that displays ELF file headers and section contents.
- Shows ELF file header information (architecture, entry point, flags)
- Displays section headers with names, sizes, addresses, and flags
- Outputs section contents in hexadecimal format
- Supports both 32-bit and 64-bit ELF files

## Building

```bash
cd nm_objdump
make all          # Build both hnm and hobjdump
make hnm          # Build only hnm
make hobjdump     # Build only hobjdump
make clean_all    # Remove binaries
make oclean_all   # Remove object files
make fclean_all   # Remove both binaries and object files
make re_all       # Clean rebuild
```

## Requirements

- **Linux** (primary target) or Unix-like system with ELF support
- GCC or compatible C compiler
- Standard C library with `<elf.h>` header (provided by `libc6-dev` on Debian/Ubuntu, `glibc-devel` on RHEL/Fedora)
- POSIX-compliant system calls (`stat`, `fread`, `fseek`, etc.)

> **Note**: This project will not compile on macOS natively due to the lack of `<elf.h>`. Use a Linux VM, container (Docker), or WSL for development on non-Linux systems.

## Usage

### hnm
```bash
./hnm [file...]          # List symbols from object file(s)
./hnm                    # Defaults to 'a.out'
```

### hobjdump
```bash
./hobjdump [file...]     # Dump file headers and sections
./hobjdump               # Defaults to 'a.out'
```

## Features

- **Cross-architecture**: Handles both 32-bit (ELF32) and 64-bit (ELF64) formats
- **Endianness support**: Automatically detects and converts big-endian ELF files
- **Robust parsing**: Validates ELF magic numbers and file integrity
- **Memory safe**: Proper resource cleanup and error handling
- **Standard compliant**: Follows ELF specification and GNU tool output formats
- **Pedantic compilation**: Built with `-Wall -Werror -Wextra -pedantic`

## Educational Value

This project demonstrates:
- ELF binary format parsing
- Low-level file I/O and memory management
- Endianness conversion techniques
- Symbol table and section header navigation
- Systems programming in C
- Makefile-based build systems

## License

This project is open source. See individual source files for licensing details.

## Contributing

Contributions are welcome! Please ensure code follows the existing style and passes all compilation checks (`make all` with zero warnings).