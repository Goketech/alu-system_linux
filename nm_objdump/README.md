# nm_objdump - ELF Binary Analysis Tools

Implementation of `nm` and `objdump -sf` commands for ELF binary analysis on Linux. Written in C as part of the ALU System Linux project.

## Overview

This module provides two complementary tools for inspecting ELF (Executable and Linkable Format) binaries:

| Tool | GNU Equivalent | Purpose |
|------|----------------|---------|
| `hnm` | `nm` | List symbols from object files |
| `hobjdump` | `objdump -sf` | Display file headers and section contents |

## Architecture

```
nm_objdump/
├── Makefile                      # Build system
├── hnm.c                         # hnm entry point
├── hobjdump.c                    # hobjdump entry point
├── hnm.h                         # hnm public API
├── hobjdump.h                    # hobjdump public API
├── hnm_main_help.c               # ELF opening, error handling, state management
├── hobjdump_main_help.c          # ELF opening, error handling, state management
├── hnm_getFileHeader.c           # ELF file header parsing (32/64-bit)
├── hobjdump_getFileHeader.c      # ELF file header parsing (32/64-bit)
├── hnm_getSecHeaders.c           # Section header table parsing (32/64-bit)
├── hobjdump_getSecHeaders.c      # Section header table parsing (32/64-bit)
├── hnm_getSymTables.c            # Symbol table extraction (32/64-bit)
├── hnm_E32SymToE64Sym.c          # 32-bit to 64-bit symbol conversion
├── hnm_printSymTables.c          # Symbol table formatted output
├── hobjdump_getSecHeadStrTab.c   # Section header string table extraction
├── hobjdump_printFileInfo.c      # File header formatted output
└── hobjdump_printSections.c      # Section contents formatted output
```

## Building

### Prerequisites
- GCC (tested with GCC 9+)
- Linux system with `<elf.h>` header
- POSIX environment

### Commands
```bash
# Build everything
make all

# Build individual tools
make hnm
make hobjdump

# Cleanup
make clean_all      # Remove binaries
make oclean_all     # Remove object files
make fclean_all     # Remove both
make re_all         # Clean rebuild
```

### Compilation Flags
```makefile
CFLAGS = -Wall -Werror -Wextra -pedantic
```
All code compiles with zero warnings under strict pedantic mode.

## Usage

### hnm - Symbol Table Dumper
```bash
./hnm [file1] [file2] ...
```

**Output format** (matching GNU nm):
```
<address> <type> <symbol_name>
```

**Symbol types**:
| Type | Description |
|------|-------------|
| `T` | Text section symbol (code) |
| `D` | Data section symbol (initialized data) |
| `B` | BSS section symbol (uninitialized data) |
| `U` | Undefined symbol (external reference) |
| `t`/`d`/`b` | Local (static) variants of above |
| `?` | Unknown/other |

**Examples**:
```bash
./hnm a.out                    # Single file
./hnm libfoo.so libbar.a       # Multiple files
./hnm                          # Defaults to 'a.out'
```

### hobjdump - Object File Dumper
```bash
./hobjdump [file1] [file2] ...
```

**Output includes**:
1. **File header**: Architecture, entry point, flags, section count
2. **Section headers**: Name, size, VMA, LMA, offset, alignment, flags
3. **Section contents**: Hexadecimal dump of each section's data

**Examples**:
```bash
./hobjdump a.out               # Full dump of a.out
./hobjdump kernel.elf boot.elf # Multiple files
```

## Technical Details

### ELF Support Matrix

| Feature | Support |
|---------|---------|
| ELF32 (32-bit) | ✅ Full |
| ELF64 (64-bit) | ✅ Full |
| Little-endian | ✅ Native |
| Big-endian | ✅ Auto-detect + convert |
| ET_REL (relocatable) | ✅ |
| ET_EXEC (executable) | ✅ |
| ET_DYN (shared object) | ✅ |
| ET_CORE (core dump) | ✅ |

### State Management

Both tools use a state struct pattern for consistent error handling and resource management:

```c
// hnm state
typedef struct nm_state_s {
    char *exec_name;        // Program name for errors
    char *f_name;           // Current file name
    FILE *f_stream;         // File handle
    int f_size;             // File size
    bool big_endian;        // Endianness flag
    bool ELF_32bit;         // Architecture flag
    Elf64_Ehdr f_header;    // File header (always 64-bit internal)
    Elf64_Shdr *s_headers;  // Section header array
    Elf64_Shdr *symtab_sh;  // .symtab section header
    Elf64_Sym *symtab_st;   // Symbol table array
} nm_state;

// hobjdump state (similar, adds section name string table)
typedef struct objdump_state_s {
    // ... common fields ...
    char *sh_strtab;        // Section header string table
} objdump_state;
```

### Key Algorithms

1. **ELF Validation**: Checks magic bytes (`0x7f 'E' 'L' 'F'`) before parsing
2. **Endianness Detection**: Reads `EI_DATA` from ELF ident; byte-swaps on big-endian
3. **Architecture Detection**: Reads `EI_CLASS` (ELF32/ELF64); converts 32→64 internally
4. **Section Navigation**: Uses `e_shoff`, `e_shnum`, `e_shentsize` to locate section headers
5. **Symbol Resolution**: Finds `.symtab` section, reads symbols, maps to section types
6. **String Tables**: Uses `e_shstrndx` to locate section name strings

### Memory Management

- All allocated memory freed in `closeState()`
- File handles closed on success and error paths
- No memory leaks under normal or error conditions

## API Reference

### hnm Public Functions

| Function | Description |
|----------|-------------|
| `openELF(nm_state*)` | Open and validate ELF file |
| `getFileHeader(nm_state*)` | Parse ELF file header |
| `getSecHeaders(nm_state*)` | Parse section header table |
| `getSymTables(nm_state*)` | Extract symbol table |
| `printSymTables(nm_state*)` | Print formatted symbols |
| `initState(nm_state*)` | Initialize state struct |
| `closeState(nm_state*)` | Cleanup resources |
| `errorMsg(format, err_str, state)` | Standardized error output |

### hobjdump Public Functions

| Function | Description |
|----------|-------------|
| `openELF(objdump_state*)` | Open and validate ELF file |
| `getFileHeader(objdump_state*)` | Parse ELF file header |
| `getSecHeaders(objdump_state*)` | Parse section header table |
| `getSecHeadStrTab(objdump_state*)` | Extract section name string table |
| `printFileInfo(objdump_state*)` | Print file header info |
| `printSections(objdump_state*)` | Print section headers + contents |
| `initState(objdump_state*)` | Initialize state struct |
| `closeState(objdump_state*)` | Cleanup resources |
| `errorMsg(format, err_str, state)` | Standardized error output |

## Testing

### Quick Validation
```bash
# Build
make all

# Test on the tools themselves
./hnm hnm
./hobjdump hnm

# Test on system binaries
./hnm /bin/ls
./hobjdump /bin/ls
```

### Expected Behavior
- Exit code 0: Success
- Exit code 1: Any file failed to process
- Errors printed to stderr with program name prefix
- Multiple files: each file's output separated by blank line with filename header

## Implementation Notes

### Design Decisions

1. **Unified 64-bit internal representation**: 32-bit ELF structs converted to 64-bit on read, simplifying downstream logic
2. **State-passing architecture**: All functions receive state pointer; no globals
3. **Lazy allocation**: Buffers allocated only when needed (section headers, symbol tables, string tables)
4. **GNU-compatible output**: Format matches `nm` and `objdump -sf` for familiar UX

### Known Limitations

- Does not support archive (.a) files directly (nm does; this operates on ELF members only)
- No support for compressed sections (`.zdebug_*`)
- Limited symbol versioning support
- No demangling of C++ symbol names

### Extending the Tools

To add new features:
1. Add function declarations to appropriate header
2. Implement in new `.c` file
3. Add to Makefile object list and target dependencies
4. Follow existing patterns for error handling and state usage

## File Format References

- [ELF Specification](https://refspecs.linuxfoundation.org/elf/)
- [System V ABI](https://wiki.osdev.org/ELF)
- [GNU binutils source](https://sourceware.org/git/?p=binutils-gdb.git) - for output format reference

## Troubleshooting

| Issue | Solution |
|-------|----------|
| "File format not recognized" | Not an ELF file, or corrupted |
| "No such file" | Check path; use absolute path |
| Segfault on large files | Check `f_size` handling; ensure 64-bit `off_t` |
| Wrong symbol types | Verify section flag mapping in `getSymNMTypeBySec` |
| Endianness issues | Confirm `bswap` functions handle all fields |

## License

Educational / Open Source. See root README for details.