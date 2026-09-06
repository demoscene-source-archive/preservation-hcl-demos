# Research log

## 2026-08-22 — Initial inspection

### Actions

- Inventoried the six supplied release/archive files.
- Recorded SHA-256 fingerprints for `BONJOUR.EXE` and `BONJOUR.HCL`.
- Read the release notes and scene.org notice.
- Inspected the leading bytes of the executable and archive.
- Parsed all live HCL directory entries directly from the archive.
- Checked all entry ranges for continuity and bounds.
- Inspected the first 16 bytes of every payload.

### Findings

- `BONJOUR.EXE` is an `MZ` DOS executable containing PMODE/W v1.33.
- `BONJOUR.HCL` begins with the live-entry count 34.
- Its header is a fixed-capacity directory with 256 entries of 20 bytes each.
- Each entry contains a 12-byte NUL-terminated DOS filename, a little-endian absolute offset, and a little-endian size.
- The first payload begins exactly after the fixed directory at `0x1404`.
- All 34 payload ranges are contiguous, in bounds, non-overlapping, and collectively fill the rest of the archive through EOF.
- Filename bytes after the first NUL are not clean padding and appear to contain stale memory data.
- The archive contains 25 PCX images, four structurally valid 3DS files, and five unidentified binary INF files.
- The XM soundtrack is a separate release file rather than an HCL entry.

### Hypotheses changed by evidence

- **Rejected:** the HCL file might be a compressed or encrypted monolithic archive. The directory and native PCX/3DS signatures show that it is a simple uncompressed container.
- **Supported:** the original executable is the best source for the custom INF semantics and full scene timeline.
- **Supported:** extraction can be implemented independently before executable disassembly.

### Decisions

- Preserve all original inputs unchanged.
- Document facts, hypotheses, and implementation choices separately.
- Build a strict Python extractor first, then use its output for deeper asset analysis.
- Defer final runtime/library choices until runtime behavior is captured.
- Keep C as the likely language for the final standalone reimplementation.

### Next evidence to collect

- Hashes and detailed format metadata for every supplied file.
- PCX dimensions, palette organization, and decoded-image contact sheets.
- 3DS chunk/object inventories.
- Statistical and structural analysis of the five INF payloads.
- PMODE/W payload structure, compiler fingerprints, strings, and disassembly entry points.
- Reference execution captures in a DOS emulator.

## 2026-08-22 — Minimal C archive extractor

### Implementation

- Added `hcl_unpack.c`, a small command-line extractor based on C stdio file access.
- Added `UNPACK_BONJOUR.BAT`, configured to extract `BONJOUR.HCL` into `BONJOUR` beside the launcher.
- Added a prebuilt 64-bit Windows `hcl_unpack.exe`.
- Used no third-party library or archive framework.
- Linked the Microsoft C runtime statically with `/MT`; the resulting executable imports only `KERNEL32.dll`.

### Validation

- Compiled the C source as C11 with MSVC `/W4 /WX` and no warnings.
- Successfully extracted all 34 payloads from the supplied archive.
- Compared SHA-256 for every extracted file with the corresponding original archive byte range: 34 of 34 matched.
- Confirmed that a repeated extraction refuses to overwrite the first existing payload and returns exit code 1.

### Decisions

- Keep the command-line interface explicit: input archive and output directory are separate arguments.
- Let the project-specific BAT choose the same basename (`BONJOUR`) for the output directory.
- Use `%~dp0` in the BAT so invocation is independent of the caller's working directory.
- Preserve existing extracted files by default rather than silently truncating them.
- Retain the requested final `PAUSE` and propagate the extractor exit code after it.
