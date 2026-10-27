# strace - ptrace-based Syscall Tracer

A Linux `strace` clone written in C. It uses `ptrace(2)` to stop a child process at every
system-call entry and exit, reads the child's registers, and prints each syscall in the
same shape as the real `strace`.

The project is built in nine incremental stages (`strace_0` through `strace_8`). Each
stage adds one formatting feature, from printing bare syscall numbers to decoding
strings, flags, buffers, and `struct stat`.

## Overview

| Binary | Stage | What it prints |
|--------|-------|----------------|
| `strace_0` | 0 | Syscall number only |
| `strace_1` | 1 | Syscall name |
| `strace_2` | 2 | Name plus raw return value |
| `strace_3` | 3 | Name, raw parameter registers, return value |
| `strace_4` | 4 | `char *` parameters as quoted strings |
| `strace_5` | 5 | Integer parameters in their real types (`int`, `size_t`, ...) |
| `strace_6` | 6 | Flag parameters as macros (`O_RDONLY`, `PROT_READ`, ...) |
| `strace_7` | 7 | `read`/`write` buffers as escaped strings |
| `strace_8` | 8 | `fstat` structure fields (`st_mode`, `st_size`) |

## Repository Layout

```
strace/
├── Makefile                  # One target per stage: strace_0 ... strace_8
├── hstrace.h                 # Shared declarations for the helper modules
├── syscalls.h                # Syscall table supplied with the assignment
│
├── 0_strace.c                # Stage 0 entry point: syscall number
├── 1_strace.c                # Stage 1 entry point: syscall name
├── 2_strace.c                # Stage 2 entry point: return value
├── 3_strace.c                # Stage 3 entry point: raw parameters
├── 4_strace.c                # Stage 4 entry point: strings
├── 6_strace.c                # Stages 5 and 6 entry point
├── 7_strace.c                # Stage 7 entry point: buffers
├── 8_strace.c                # Stage 8 entry point: structures
│
├── 4_printParams.c           # Stage 4 parameter dispatch
├── 5_printParams.c           # Stage 5 integer formatting
├── 6_printParams.c           # Stage 6 macros and NULL pointers
├── 7_printParams.c           # Stage 7 buffer dispatch
├── 8_printParams.c           # Stage 8 structure dispatch
│
├── 6_printStrParam.c         # NUL-terminated string printing
├── 8_printStrParam.c         # read/write buffer printing with escapes
│
├── 8_printFlagsInt.c         # mmap/open/access flag decoding
├── 8_printStatParam.c        # struct stat decoding for fstat
└── syscall.h                 # Local fragment; the full syscalls.h is still required
```

The numeric prefix marks the stage a file was introduced for. Later stages sometimes
reuse or extend an earlier helper, so the `Makefile` is the authoritative list of which
objects go into each binary.

## Requirements

- Linux on x86-64 (`struct user_regs_struct`, `orig_rax`, and the x86-64 argument
  registers are used directly)
- GCC and `make`
- The complete `syscalls.h` distributed with the assignment, containing `type_t`,
  `syscall_t`, and the `syscalls_64[]` table
- Permission to call `ptrace(2)` on your own child process (normally allowed; some
  hardened containers and seccomp profiles block it)

## Building

The default `make` target only builds `0_strace.o`, so name a stage explicitly:

```bash
make strace_0      # syscall number
make strace_1      # syscall name
make strace_2      # return value
make strace_3      # raw parameters
make strace_4      # strings
make strace_5      # integers
make strace_6      # macros
make strace_7      # buffers
make strace_8      # structures
```

Build every stage:

```bash
for n in 0 1 2 3 4 5 6 7 8; do make strace_$n || break; done
```

Compilation flags:

```make
CFLAGS = -Wall -Werror -Wextra -pedantic
```

Cleanup targets:

```bash
make clean_all     # remove strace_0 ... strace_8
make oclean_all    # remove object files
make fclean_all    # remove both
make re_all        # rebuild all stages
```

## Usage

```bash
./strace_8 <program> [arguments...]
```

The first argument is passed directly to `execve(2)`, so it must be a path. `execve`
does not search `PATH`:

```bash
./strace_8 /bin/ls -l          # correct
./strace_8 ls -l               # fails: execve("ls") -> ENOENT
```

The child inherits the tracer's environment unchanged. Program output and trace output
share the same stdout, so a verbose target can interleave with the trace. Redirect the
program's output if that matters:

```bash
./strace_8 /bin/ls -l > /dev/null
```

### Example Output

Stage 8, tracing a small program (addresses and counts vary):

```text
execve("./hello", ["./hello"], [/* 32 vars */]) = 0
brk(NULL) = 0x55d3c1a2b000
access("/etc/ld.so.preload", R_OK) = -1
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=57341, ...}) = 0
mmap(NULL, 57341, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7f6c2a4e7000
...
write(1, "hello\n", 6) = 6
exit_group(0) = ?
```

The final `) = ?` is expected: when the tracee exits, its registers are no longer
readable, so the return value of the last syscall cannot be shown.

### Stage-by-Stage Output

Given a target that calls `open("/tmp/x", O_RDONLY)`:

```text
strace_0:  2
strace_1:  open
strace_2:  open = 0x3
strace_3:  open("/tmp/x", 0x0) = 0x3
strace_4:  open("/tmp/x", 0x0) = 0x3
strace_5:  open("/tmp/x", 0) = 3
strace_6:  open("/tmp/x", O_RDONLY) = 3
strace_7:  open("/tmp/x", O_RDONLY) = 3
strace_8:  open("/tmp/x", O_RDONLY) = 3
```

## How It Works

1. `main` calls `fork()`.
2. The child calls `PTRACE_TRACEME` and then `execve(argv[1], argv + 1, envp)`.
3. The parent waits for the child to stop after `execve`.
4. `PTRACE_SYSCALL` resumes the child and stops it again at the next syscall entry and
   at that syscall's exit. The tracer alternates between the two stops.
5. At each stop, `PTRACE_GETREGS` fills a `struct user_regs_struct`:
   - `orig_rax` selects the syscall in `syscalls_64[]`
   - `rdi`, `rsi`, `rdx`, `r10`, `r8`, `r9` hold arguments 1-6
   - `rax` holds the return value
6. On entry, the syscall name and parameters are printed. On exit, the return value is
   printed on the same line.
7. The loop ends when `wait` reports `WIFEXITED`.

### Parameter Registers

| Argument | Register |
|----------|----------|
| 1 | `rdi` |
| 2 | `rsi` |
| 3 | `rdx` |
| 4 | `r10` |
| 5 | `r8` |
| 6 | `r9` |
| Syscall number | `orig_rax` |
| Return value | `rax` |

### Late Parameter Reads

Some syscalls write into buffers owned by the caller. Those buffers only contain useful
data after the syscall returns, so their parameters are printed at the exit stop rather
than the entry stop:

- `read` -- the kernel-filled buffer is fetched with `PTRACE_PEEKTEXT` at exit
- `fstat` -- `struct stat` is read after a successful return

`write` buffers are already valid on entry, but reuse the same buffer formatter.

The `lateParamRead` helper centralizes that decision.

### The execve Special Case

After `execve` succeeds, the child's original memory image no longer exists, so its
`argv` and `envp` cannot be read from the tracee. `printExecveParams` prints them from
the tracer's own `argv`/`envp` instead. Strings are truncated at 32 characters, and at
most 16 arguments are shown. The environment is summarized as `[/* N vars */]`.

## Formatting Rules

### Return Values

| Type | Format |
|------|--------|
| `INT`, `PID_T` | `%i` |
| `LONG`, `SSIZE_T` | `%li` |
| `UNSIGNED_INT`, `UINT32_T` | `%u` |
| `UNSIGNED_LONG`, `U64`, `SIZE_T` | `%lu` |
| anything else | `%#lx` |

### Parameters

- Integer types are printed with their signed or unsigned format.
- `char *` values are printed as quoted, NUL-terminated strings.
- `read` and `write` buffers are printed as quoted strings with up to 32 bytes;
  non-printable bytes use `\n`, `\t`, `\r`, or octal escapes, and a truncated buffer
  ends with `...`.
- Pointer parameters that are null and are recognized as pointer types print `NULL`
  (stages 6 and later).
- Variadic tails print `...`.
- Unrecognized types and pointers print as `0x...`.

### Flags

| Syscall | Parameter | Decoding |
|---------|-----------|----------|
| `mmap` | `prot` | `PROT_NONE`, `PROT_READ`, `PROT_WRITE`, `PROT_EXEC` |
| `mmap` | `flags` | `MAP_SHARED`, `MAP_PRIVATE`, `MAP_FIXED`, `MAP_ANONYMOUS`, ... |
| `open` | `flags` | `O_RDONLY`, `O_WRONLY`, `O_RDWR`, `O_CREAT`, `O_CLOEXEC`, ... |
| `access` | `mode` | `F_OK`, `X_OK`, `W_OK`, `R_OK` |

Flags are combined with `|`, for example `PROT_READ|PROT_WRITE` or
`O_RDONLY|O_CLOEXEC`.

### Structures

For `fstat`, stage 8 prints a partial `struct stat`:

```text
fstat(3, {st_mode=S_IFREG|0644, st_size=57341, ...}) = 0
```

`st_mode` includes the file-type macro and the permission bits. Other members are
intentionally elided as `...`, and other structure pointers are printed as addresses.

## Helper API

Declared in `hstrace.h`:

| Function | Purpose |
|----------|---------|
| `tracerLoop` | Fork/wait/ptrace loop |
| `printReturn` | Format the syscall return value |
| `lateParamRead` | Decide entry-stop vs. exit-stop parameter printing |
| `printExecveParams` | Print `argv`/`envp` for `execve` |
| `printParams` | Dispatch all parameters of a syscall |
| `printParam` | Dispatch a single parameter by type |
| `printInt` / `_printInt` | Print integer-typed parameters |
| `printFlagsInt` | Decode `mmap`/`open`/`access` flags |
| `printStrParam` / `_printStrParam` | Print NUL-terminated strings |
| `printStatParam` / `printStatMode` | Print `fstat` fields |
| `isPtrParam` | Identify pointer parameter types |

## Limitations

- **x86-64 only.** The register names and `struct user_regs_struct` layout are
  architecture-specific. 32-bit compatibility mode is not handled.
- **No process following.** `fork`, `vfork`, and `clone` children are not traced, and
  threads are not followed. Only the original tracee is observed.
- **No `PATH` lookup.** The target must be an explicit path, as described above.
- **No errno decoding.** Failed calls show the negative return value (for example
  `-1`) rather than a symbolic name such as `ENOENT`.
- **No signal decoding.** Signal stops are not formatted, and signal delivery is not
  reported.
- **No attach or filtering.** There is no `-p` equivalent, no syscall selection, and no
  timestamping.
- **Partial strings and structs.** `execve` arguments are capped at 32 characters and
  16 entries; `read`/`write` previews are capped at 32 bytes; only `st_mode` and
  `st_size` are decoded from `struct stat`.
- **Table coverage.** Syscall names and types come from the supplied `syscalls_64[]`
  table, which covers the Linux x86-64 syscall range it was generated for. Newer or
  unknown syscalls are not described.
- **Last return value is unknown.** The trace ends with `) = ?` because the tracee's
  registers are gone after exit.

## Testing

Build stage 8 and trace a trivial program:

```bash
cat > hello.c <<'EOF'
#include <unistd.h>

int main(void)
{
	write(1, "hello\n", 6);
	return (0);
}
EOF

gcc -o hello hello.c
./strace_8 ./hello
```

Expected shape:

```text
execve("./hello", ["./hello"], [/* N vars */]) = 0
...
write(1, "hello\n", 6) = 6
exit_group(0) = ?
```

If the system has GNU `strace`, compare the two on a small command:

```bash
strace -o real.txt /bin/true
./strace_8 /bin/true > mine.txt
```

The implementations will not match byte-for-byte (this tracer does not decode errno or
signals), but syscall names, arguments, and successful return values should correspond.

## Troubleshooting

| Symptom | Likely cause |
|---------|--------------|
| `usage: ... <prog> <prog args>...` | No target program was given |
| Trace is empty or the target dies immediately | `execve` failed; pass an absolute or relative path |
| `ptrace: Operation not permitted` | The environment blocks `ptrace` (container seccomp policy) |
| Output interleaves with the target's output | Both write to stdout; redirect one of them |
| Unknown syscall shown as garbage | Syscall is missing from `syscalls_64[]` |
| Trace stops after a `fork` | Child processes are not followed |

## References

- `ptrace(2)` -- process tracing interface
- `wait(2)` -- stop and exit status reporting
- `syscall(2)` and `syscalls(2)` -- Linux syscall ABI
- [System V AMD64 ABI](https://refspecs.linuxfoundation.org/abi/) -- argument registers
- [GNU strace source](https://github.com/strace/strace) -- output format reference

## License

Educational / open source. See the root README for details.
