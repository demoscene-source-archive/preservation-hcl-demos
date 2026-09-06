# C command-line HCL extractor

## Delivered files

| File | Purpose |
|---|---|
| `hcl_unpack.c` | Dependency-free C source for the extractor |
| `hcl_unpack.exe` | Prebuilt 64-bit Windows command-line extractor |
| `UNPACK_BONJOUR.BAT` | Double-clickable launcher configured for the supplied archive |

### Delivered-file fingerprints

| File | SHA-256 |
|---|---|
| `hcl_unpack.c` | `85407BD4986981A59137B1C7DADDEC95D2726A0E1124C270CFFF5DF066BF0EDD` |
| `hcl_unpack.exe` | `051B1D3E8F9B3200E0CD3F26802F4751F82211CBC71AAF9C49E11E89499AF25E` |
| `UNPACK_BONJOUR.BAT` | `FAE62B6275D554AE8EAE5FAA7E8EAF7E509BEDAD9EB3B9623999ECB5A8445335` |

## Normal use

Run `UNPACK_BONJOUR.BAT` from the release directory. It invokes:

```bat
hcl_unpack.exe BONJOUR.HCL BONJOUR
```

Paths in the actual batch file are based on `%~dp0`, so the launcher still works when it is started from another current working directory or by double-clicking it in Explorer.

The command creates a `BONJOUR` directory beside the archive and writes the 34 archived files into it. The batch file stores the extractor's exit code, reports failures, executes `PAUSE`, and returns the extractor's original exit code after the pause.

The extractor can also be used directly:

```text
hcl_unpack.exe <archive.hcl> <output-directory>
```

## Dependencies

The source has no third-party dependencies. It uses:

- `fopen`, `fread`, `fwrite`, `fseek`, and `ftell` for all archive and payload I/O;
- `malloc` for one 64 KiB copy buffer and output path strings;
- `_mkdir` and `_stat` from the Windows C runtime for output-directory handling;
- portable `mkdir` and `stat` branches for non-Windows builds.

The delivered Windows executable was compiled with Microsoft Visual C++ using `/MT`, so its C runtime is linked statically. A dependency inspection reports only `KERNEL32.dll`, a standard Windows system DLL; no Visual C++ redistributable or external archive/image library is required.

Reference build commands are included at the top of `hcl_unpack.c`:

```bat
cl /nologo /W4 /O2 /MT hcl_unpack.c /Fe:hcl_unpack.exe
```

```text
gcc -std=c99 -Wall -Wextra -O2 -o hcl_unpack.exe hcl_unpack.c
```

## Safety and validation behavior

Before creating any payload file, the extractor validates the complete live directory:

- the archive is at least the fixed `0x1404`-byte header size;
- the live entry count is at most 256;
- each 12-byte filename field contains a NUL terminator;
- logical names are non-empty basenames and contain no slash, backslash, colon, control character, `.` or `..` path component;
- every payload begins after the fixed header and ends within the archive;
- payload ranges do not overlap;
- filenames are unique under ASCII case-insensitive comparison, matching Windows filesystem expectations.

The output directory is created if it does not exist. An existing directory is accepted, but the extractor refuses to overwrite any existing output path. On a payload read/write/finalization error, it removes only the incomplete file it just created and returns failure.

This strict behavior is an intentional preservation decision: a repeated batch run cannot silently destroy extracted files that may have been inspected or modified.

## Verification performed

The delivered source was compiled as C11 with MSVC warnings elevated to errors:

```bat
cl /std:c11 /W4 /WX /O2 /MT /TC hcl_unpack.c
```

The resulting executable was run against the supplied `BONJOUR.HCL`:

- process exit code: 0;
- files reported and produced: 34;
- each output size matched its directory entry;
- SHA-256 of each output matched SHA-256 of the corresponding byte range in the archive: 34/34;
- the final rebuilt executable was subjected to the same 34/34 payload-hash test after static runtime linking;
- a second run against the populated output directory refused to overwrite `d1.pcx` and returned exit code 1;
- PE dependency inspection found only `KERNEL32.dll`.

The batch file itself was not executed during automation because its required final `PAUSE` is deliberately interactive. Its command line and error-code handling were inspected directly.
