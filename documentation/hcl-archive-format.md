# `BONJOUR.HCL` archive format

## Status

The container-level layout is **verified** against all 34 entries in the supplied archive. It is simple enough to extract without reverse engineering `BONJOUR.EXE`. Executable analysis is still useful for confirming the original lookup semantics and the meaning of the custom `.inf` files.

## Byte layout

```text
offset  size             meaning
------  ---------------  --------------------------------------------
0x0000  4                live entry count (uint32 little-endian)
0x0004  256 * 20         fixed-capacity directory
0x1404  remaining bytes  concatenated file payloads
```

The first four bytes are:

```text
22 00 00 00
```

They decode to 34, which matches the number of valid directory entries.

The first payload begins at `0x1404` (decimal 5,124), and:

```text
4 + (256 * 20) = 5124 = 0x1404
```

This proves that the archive reserves space for 256 directory entries even though only the first 34 are live in this file.

## Directory entry

Each directory entry is 20 bytes:

```c
struct hcl_entry_disk {
    char     name[12];   /* NUL-terminated DOS 8.3 name */
    uint32_t offset;     /* absolute archive offset, little-endian */
    uint32_t size;       /* payload length in bytes, little-endian */
};
```

This C declaration describes the logical disk fields, not a structure that should be read directly with `fread`: host padding, host endianness, malformed input, and integer overflow must be handled explicitly.

### Filename padding caveat

Bytes after the first NUL in the 12-byte filename field are not reliably zero-filled. The first entries contain apparent stale memory fragments, including repeated byte values and pieces of unrelated text. A reader must:

1. Read exactly 12 raw name bytes.
2. Stop at the first NUL for the logical filename.
3. Reject a name without a NUL unless compatibility testing later proves that the original loader accepted it.
4. Never use the suffix bytes as part of the filename.

The stale suffix bytes may reveal how the original packing tool populated its filename buffer, but they are not needed for extraction.

## Verified directory

| # | Name | Offset | Size | End offset | Identified type |
|---:|---|---:|---:|---:|---|
| 0 | `d1.pcx` | `0x00001404` | 77,697 | `0x00014385` | PCX |
| 1 | `d2.pcx` | `0x00014385` | 3,297 | `0x00015066` | PCX |
| 2 | `a.pcx` | `0x00015066` | 67,521 | `0x00025827` | PCX |
| 3 | `b.pcx` | `0x00025827` | 45,460 | `0x000309BB` | PCX |
| 4 | `bonjour.pcx` | `0x000309BB` | 22,410 | `0x00036145` | PCX |
| 5 | `boutonz.pcx` | `0x00036145` | 18,189 | `0x0003A852` | PCX |
| 6 | `flow.pcx` | `0x0003A852` | 57,083 | `0x0004874D` | PCX |
| 7 | `fond1.pcx` | `0x0004874D` | 48,717 | `0x0005459A` | PCX |
| 8 | `fondue.pcx` | `0x0005459A` | 73,356 | `0x00066426` | PCX |
| 9 | `gr1.pcx` | `0x00066426` | 11,110 | `0x00068F8C` | PCX |
| 10 | `gr2.pcx` | `0x00068F8C` | 13,053 | `0x0006C289` | PCX |
| 11 | `gr3.pcx` | `0x0006C289` | 8,471 | `0x0006E3A0` | PCX |
| 12 | `gr4.pcx` | `0x0006E3A0` | 13,261 | `0x0007176D` | PCX |
| 13 | `gree.3ds` | `0x0007176D` | 35,964 | `0x0007A3E9` | 3DS |
| 14 | `greemap.pcx` | `0x0007A3E9` | 279,246 | `0x000BE6B7` | PCX |
| 15 | `greet.inf` | `0x000BE6B7` | 92,500 | `0x000D500B` | Unknown binary INF |
| 16 | `heartz.pcx` | `0x000D500B` | 7,093 | `0x000D6BC0` | PCX |
| 17 | `logo1.pcx` | `0x000D6BC0` | 19,813 | `0x000DB925` | PCX |
| 18 | `logo2.pcx` | `0x000DB925` | 52,894 | `0x000E87C3` | PCX |
| 19 | `mama.pcx` | `0x000E87C3` | 14,482 | `0x000EC055` | PCX |
| 20 | `mamaz.pcx` | `0x000EC055` | 13,220 | `0x000EF3F9` | PCX |
| 21 | `mapcred.pcx` | `0x000EF3F9` | 85,959 | `0x001043C0` | PCX |
| 22 | `nappe.pcx` | `0x001043C0` | 72,512 | `0x00115F00` | PCX |
| 23 | `rev3.3ds` | `0x00115F00` | 57,025 | `0x00123DC1` | 3DS |
| 24 | `revfond.pcx` | `0x00123DC1` | 34,670 | `0x0012C52F` | PCX |
| 25 | `revmapz.pcx` | `0x0012C52F` | 101,022 | `0x00144FCD` | PCX |
| 26 | `rev.inf` | `0x00144FCD` | 52,896 | `0x00151E6D` | Unknown binary INF |
| 27 | `rose.pcx` | `0x00151E6D` | 55,374 | `0x0015F6BB` | PCX |
| 28 | `sprfume.pcx` | `0x0015F6BB` | 3,824 | `0x001605AB` | PCX |
| 29 | `the.inf` | `0x001605AB` | 96,208 | `0x00177D7B` | Unknown binary INF |
| 30 | `the5.3ds` | `0x00177D7B` | 32,752 | `0x0017FD6B` | 3DS |
| 31 | `themap.pcx` | `0x0017FD6B` | 75,072 | `0x001922AB` | PCX |
| 32 | `wooom.inf` | `0x001922AB` | 106,260 | `0x001AC1BF` | Unknown binary INF |
| 33 | `wooom2.3ds` | `0x001AC1BF` | 129,862 | `0x001CBD05` | 3DS |

The final end offset is decimal 1,883,397, exactly equal to the archive size.

## Integrity rules for a new reader

A preservation-quality reader should validate all of the following before extraction:

- The archive is at least `0x1404` bytes long.
- The live count is no greater than 256.
- Every live entry has a usable NUL-terminated filename.
- `offset + size` does not overflow and does not exceed the archive length.
- Output names are treated as basenames; path separators, drive syntax, `.` and `..` are rejected.
- Entries do not overlap the directory or one another, unless later evidence shows that aliasing was intentionally supported.
- Duplicate names are reported rather than silently overwritten.

For this specific archive, entry offsets are monotonically increasing, payloads do not overlap, payloads have no gaps, and the final entry ends at EOF.

## Minimal parsing pseudocode

```text
read little-endian uint32 count at offset 0
require count <= 256

for index in 0 .. count-1:
    entry_offset = 4 + index * 20
    read 12 raw filename bytes
    decode bytes before first NUL as the filename
    read little-endian uint32 payload_offset
    read little-endian uint32 payload_size
    validate [payload_offset, payload_offset + payload_size)
```

## Unknown container semantics

The following details have not yet been verified from `BONJOUR.EXE`:

- Whether filename lookup is case-sensitive.
- Whether the loader linearly scans the live entries or builds another index.
- Whether short reads or malformed entries were handled by the original code.
- Whether unused directory slots contain any meaningful packer residue.
- Whether other HCL archives used the same 256-entry capacity and exact layout.
