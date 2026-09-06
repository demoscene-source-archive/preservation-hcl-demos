# First Demo (HCL)

- Pouet: https://www.pouet.net/prod.php?which=61406
- Release: https://files.scene.org/view/parties/1995/wired95/demo/firsthcl.zip
- Download: https://files.scene.org/get/parties/1995/wired95/demo/firsthcl.zip
- Downloaded: 2026-09-06
- Original ZIP: demo-releases/firsthcl.zip
- ZIP SHA-256: 6211B7A77F93BC876A01AADC6A10024CABC84B1ACF0512956B0D24E8AA2D1A6B
- FIRST.HCL SHA-256: 8D9827387B71C20EE2FC538FC010A8985CA27E930C71829C973A4D5A2EB067BA

FILE_ID.DIZ identifies the release as First / HcL, presented at Wired'95.
All 25 original ZIP files and the WHAT directory layout are preserved here.
Every original file was checked byte-for-byte against its ZIP member.

## Container and extracted files

FIRST.HCL begins with a DOS MZ executable header. 7-Zip 24.07 identifies an
embedded ARJ archive at offset 15104 (0x3B00), occupying the remaining 260195
bytes. Strings in the executable identify an ARJ self-extractor. This is not
the custom HCL resource format handled by bin/hcl_unpack.exe.

WHAT/HCL.EXE is byte-identical to FIRST.HCL. The FIRST directory contains
the 21 files extracted from that archive using 7-Zip, without executing the
self-extractor. All 21 extracted SHA-256 hashes match the corresponding
loose files already supplied in WHAT.

| Format | Count | Verification |
| --- | ---: | --- |
| GIF images | 7 | Fully decoded with Pillow 11.3.0; all are 320x200 |
| MOD music | 1 | CESIUM.MOD has the M.K. signature at offset 1080; playback not tested |
| DOS EXE programs | 11 | MZ headers; declared executable sizes equal file sizes, with no appended overlay |
| DOS COM launchers | 2 | !.COM and DEMOHCL.COM contain BAT2EXEC 1.5 identification strings |

The native files retain their original names and bytes. No additional HCL
containers were found among the extracted files. Executable code and any
resources compiled into it have not been decompiled or carved out.

FIRST.COM, outside the archive, also contains BAT2EXEC identification strings.
Its embedded command text includes creation of WHAT, copying FIRST.HCL to
HCL.EXE, and a DELTREE command for WHAT. No release EXE or COM was executed.

## Repeat extraction

Run UNPACK_FIRST.BAT, or invoke UNPACK_FIRST.ps1 with PowerShell. The script
uses 7z.exe from PATH or the standard Program Files/7-Zip installation;
an alternative path can be supplied with -SevenZip. It extracts FIRST.HCL
into FIRST and refuses to proceed if that output directory already exists.
