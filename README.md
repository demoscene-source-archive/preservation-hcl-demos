# Preservation of HCL Demos

Preservation effort focused on HCL's french demos (late 1990s).

![alt text](img/demo_bonjour.png)
![alt text](img/demo_matrix.png)
![alt text](img/demo_poulets.png)
![alt text](img/demo_train.png)

## Build the extractor on Windows

Install CMake 3.15 or newer and Visual Studio with the Desktop development
with C++ workload, then run from the repository root:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

The executable is written to `bin/hcl_unpack.exe`, including when using a
multi-configuration generator. MSVC builds use the static C runtime.

```powershell
.\bin\hcl_unpack.exe demo-unpack\the-train\THETRAIN.HCL output-train
```

Entries with a `.HCL` extension (case-insensitive) are checked for PCX images
and 3D Studio models. Recognized assets receive an additional `.PCX` or `.3DS`
suffix: `00000.hcl` becomes `00000.hcl.PCX`. PCX detection validates the header,
encoded pixel length, and palette; 3DS detection validates the root length
and immediate child chunk boundaries and requires an editor chunk.
Original filename casing and payload bytes are preserved. Unknown or malformed
assets retain their names. Existing output files are never
overwritten; use a fresh output directory to extract again.

Loose assets, such as Matrix's numbered HCL files, use `--asset`:

```powershell
.\bin\hcl_unpack.exe --asset demo-unpack\matrix\04.HCL output-matrix
```

Run `demo-unpack/matrix/UNPACK_MATRIX.BAT` to process the whole Matrix release
into its `MATRIX` subdirectory. Its `01.HCL` and `02.HCL` have nonstandard PCX
signatures and retain their original names; the tool does not repair bytes.

Run the regression checks after building:

```powershell
python tests\unpack.py
```

These require Python 3 and check all three demo archives against their extracted
payload hashes, Matrix's 33 loose assets, format detection, malformed data,
and output collision handling.
Temporary test outputs are kept under the ignored `build/` directory.
