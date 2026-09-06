# Matrix (HCL)

- Pouet: https://www.pouet.net/prod.php?which=58720
- Release: https://files.scene.org/view/mirrors/hornet/demos/1997/m/matrix.zip
- Download: https://files.scene.org/get/mirrors/hornet/demos/1997/m/matrix.zip
- Downloaded: 2026-09-06
- Original ZIP: demo-releases/matrix.zip
- ZIP SHA-256: B9EBB8A2F502B27EAA5FBBBD24435E8724144548B590FD269679675C69A63E7C

The original NFO identifies Matrix by HCL, first presented at WIRED'97.
All 38 ZIP members are preserved here, with their bytes checked against the
ZIP: MATRIX.EXE, MATRIX.NFO, SEARCH.XM, 33 numbered HCL assets, JIFJDTLY.COM,
and -E-ACE-W-.DJ. No release executable was run.

Unlike Bonjour Madame, Mutant Poulets, and The Train, this release already
stores its assets as individual files. There is no HCL resource container
to pass to bin/hcl_unpack.exe.

The MATRIX subdirectory contains byte-identical copies of all 33 HCL assets:

- 21 PCX images receive an additional .PCX suffix. All were fully decoded
  with Pillow 11.3.0 to validate the format.
- 10 3D Studio files receive an additional .3DS suffix: 03, 07, 13, 16, 19,
  22, 24, 25, 30, and 32. Their 0x4D4D root signatures, declared lengths,
  and immediate child chunk boundaries were checked. Geometry was not rendered.
- 01.HCL and 02.HCL retain their names. Both have a zero first byte instead
  of the PCX manufacturer byte 0x0A. Changing only that byte in memory allows
  Pillow to decode each as a 320x200 PCX. The files remain unmodified on disk.

Run UNPACK_MATRIX.BAT to recreate the MATRIX subdirectory, or invoke
UNPACK_MATRIX.ps1 with PowerShell. The script invokes bin/hcl_unpack.exe with
--asset for each HCL file, automatically detecting PCX and 3DS data, and
refuses to overwrite an existing output directory.
Every prepared asset was checked by SHA-256 against its original HCL file.
