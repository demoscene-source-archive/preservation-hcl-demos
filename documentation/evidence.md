# Evidence inventory

## Supplied release files

| File | Size (bytes) | Original timestamp shown by the filesystem | Initial identification |
|---|---:|---|---|
| `BONJOUR.EXE` | 207,340 | 1998-09-07 16:40:46 | DOS `MZ` executable using the PMODE/W DOS extender |
| `BONJOUR.HCL` | 1,883,397 | 1998-09-07 16:11:52 | Custom, uncompressed resource container |
| `BONJOUR.NFO.TXT` | 2,173 | 1998-09-07 14:26:08 | Original production notes and credits |
| `BONJOUR.XM` | 112,971 | 1998-09-06 22:37:52 | FastTracker II XM music module; not stored in the HCL archive |
| `FILE_ID.DIZ` | 56 | 1998-09-07 13:55:18 | Short BBS-style release description |
| `scene.org.txt` | 2,714 | 2004-09-30 13:49:10 | Later scene.org archive notice |

The filesystem timestamps are useful provenance evidence, but they have not yet been independently authenticated.

## Cryptographic fingerprints

| File | SHA-256 |
|---|---|
| `BONJOUR.EXE` | `01418601B4C9815CAEDA74109325C449055CAAD90825DFE4D124BE8C6046B586` |
| `BONJOUR.HCL` | `FCEBAB7E4F5C0E43DA345EF975352BFF46CA20FBCF89F4FF7151D75DC3A621D2` |

Hashes for the other supplied files remain to be recorded.

## Release and authorship clues

The original NFO identifies the production as **Bonjour Madame** by **Hcl**, dated 1998. It credits:

- Diabolo: code
- Danube: graphics
- Lluvia / Slk: guest music
- Fra / Pipo: guest graphics

The NFO also mentions Hcl members and earlier productions. `scene.org.txt` describes this release as placing second at RTS Party 1998. This placement is archival metadata from the supplied scene.org notice and has not yet been checked against an independent party-results source.

The NFO text is in a legacy DOS character encoding. Rendering it directly as a modern Windows text file produces mojibake (for example around accented French characters). The original bytes should be preserved; any future transcription should explicitly identify and convert the source encoding.

## Executable observations

**Verified:** `BONJOUR.EXE` begins with the DOS `MZ` signature.

The executable header contains the ASCII string:

```text
PMODE/W v1.33 DOS extender - Copyright 1994-1997, Daredevil and Tran.
```

This identifies the program as a DOS protected-mode executable rather than a Windows PE executable. It also suggests a 32-bit DOS program, but the exact compiler, executable payload format, entry point, and instruction set mode still need to be confirmed by executable parsing and disassembly.

## Resource observations

The HCL archive contains exactly 34 live directory entries:

- 25 files named with a `.pcx` extension
- 4 files named with a `.3ds` extension
- 5 files named with an `.inf` extension

The `.pcx` payloads begin with `0A 05 01 08`, consistent with version-5, RLE-encoded, 8-bit PCX images. Their headers are structurally readable.

The `.3ds` payloads begin with the 3DS main-chunk identifier `4D 4D`. In each inspected file, the little-endian chunk length in the following four bytes matches the complete archived payload size. This is strong evidence that they are intact Autodesk 3D Studio files, not merely custom data with a misleading extension.

The `.inf` payloads are binary rather than text. Their purpose and record structure are currently unknown. Plausible roles include animation, object placement, effect timing, or precomputed geometry, but none of these interpretations is yet verified.

## Preservation observations

- The standalone XM file and the HCL archive together contain the obvious music, image, model, and custom effect data needed by the executable.
- The HCL data region is contiguous: every payload starts exactly where the previous payload ends.
- The final payload ends exactly at the end of `BONJOUR.HCL`; there is no observed trailer.
- No HCL payload-level compression or encryption is present at the container layer. PCX files retain their own native RLE compression.
