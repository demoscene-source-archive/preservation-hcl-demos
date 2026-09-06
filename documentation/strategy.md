# Reverse-engineering and preservation strategy

## Goal

Recover the behavior and audiovisual presentation of the 1998 DOS demo in a form that is understandable, testable, and maintainable. The initial target languages under consideration are Python and C.

## Decisions

### 1. Preserve originals and work from derived files

**Decision:** treat the supplied release files as immutable evidence. Generated extracts, decoded images, traces, and rebuilt executables will live in separate directories when implementation begins.

Reason: byte-for-byte reproducibility is essential when an interpretation later changes.

### 2. Use Python for investigation and tooling

**Decision:** implement the first HCL reader/extractor and binary-analysis probes in Python.

Reason: Python is well suited to explicit binary parsing, fast iteration, corpus inspection, image conversion, structure inference, and automated reports. It can also become a readable executable specification for the archive and `.inf` formats.

This does not commit the final demo runtime to Python.

### 3. Use C for a faithful standalone runtime if practical

**Decision:** use C as the likely final implementation language after the data formats and timeline are understood.

Reason: C maps naturally to the original DOS-era implementation, supports deterministic real-time rendering and audio, and can target both a portable modern runtime and, if desired, a historically constrained DOS-compatible build.

The rendering/audio library and exact target platform remain open decisions. They should be chosen only after the original video mode, palette behavior, timing, XM playback semantics, and 3D pipeline are observed.

### 4. Treat the original executable as an oracle

**Decision:** combine static analysis with controlled runtime observation rather than attempting a blind visual rewrite from assets alone.

The executable can answer questions about:

- resource lookup and load order;
- `.inf` record interpretation;
- scene order and effect timing;
- video mode, palette changes, and frame pacing;
- 3DS transformation and rasterization behavior;
- XM synchronization and channel/row triggers;
- keyboard and exit behavior.

### 5. Separate three deliverables

**Decision:** keep the work conceptually split into:

1. **Forensic tools:** parsers, extractors, validators, and inspection reports.
2. **Behavioral specification:** documented formats, timing, scene state, and verified reference captures.
3. **Reimplementation:** a clean runtime that consumes the original or losslessly derived assets.

This separation makes it possible to improve the analysis without entangling it with renderer code.

## Proposed phases

### Phase A: inventory and lossless extraction

- Finish hashing all original files.
- Implement a bounds-checked HCL lister/extractor.
- Verify extracted payload hashes and lossless archive round trips.
- Decode PCX metadata and palettes without altering the archived originals.
- Parse the 3DS chunk trees and inventory objects, meshes, cameras, lights, and materials.

### Phase B: understand the executable

- Identify the protected-mode executable payload and compiler/runtime signatures.
- Locate resource filenames and the HCL open/read/lookup routines.
- Identify graphics, keyboard, timer, and XM-player code or linked libraries.
- Run the demo in an instrumented DOS emulator and capture video, audio, logs, screenshots, and input behavior.
- Correlate runtime file reads with scene transitions.

### Phase C: infer the `.inf` formats and timeline

- Analyze value widths, alignment, repetition periods, and cross-file similarities.
- Compare `.inf` lengths and records against corresponding 3DS mesh counts.
- Trace executable accesses to determine record size and field meaning.
- Build parsers only after hypotheses are supported by both static and dynamic evidence.

### Phase D: behavioral prototype

- Implement resource loading and scene sequencing in Python or a small inspection harness.
- Reproduce palettes, camera paths, object transforms, and timing scene by scene.
- Establish objective reference comparisons using frame captures and audio positions.

### Phase E: C reimplementation

- Choose a portable graphics/audio layer based on the observed requirements.
- Port the verified behavioral model, retaining explicit fixed-width types and deterministic timing.
- Compare output against original reference captures at known timeline points.
- Package source, build instructions, original-file fingerprints, and format documentation.

## Current open questions

- What compiler and libraries produced `BONJOUR.EXE`?
- What video mode and refresh/timing source does the original use?
- Is the renderer software-only, and what are its palette and shading rules?
- Which XM player is embedded, and how does scene synchronization relate to XM order/row positions?
- What are the `.inf` record layouts?
- Do the 3DS files contain the final runtime meshes, or are they transformed/preprocessed using `.inf` data?
- Is the preservation target a modern faithful port, a DOS-source reconstruction, or both?
