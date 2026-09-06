# Bonjour Madame reverse-engineering notes

This directory is the project notebook for the preservation and reverse engineering of **Bonjour Madame**, an HCL demoscene production released in 1998.

## Documents

- [evidence.md](evidence.md) records the supplied files, provenance clues, hashes, and directly observed facts.
- [hcl-archive-format.md](hcl-archive-format.md) describes the recovered `BONJOUR.HCL` container format.
- [unpacking-tool.md](unpacking-tool.md) documents the C command-line extractor and its batch launcher.
- [strategy.md](strategy.md) records the current reverse-engineering strategy and engineering decisions.
- [research-log.md](research-log.md) is a chronological work log, including hypotheses and their validation state.

## Documentation rules

The notes distinguish between:

- **Verified:** supported directly by bytes in the supplied files or by repeatable analysis.
- **Likely:** strongly supported, but not yet proved by executable analysis or runtime observation.
- **Unknown:** an open question that must not silently become an implementation assumption.
- **Decision:** an engineering choice for the preservation project, not necessarily a property of the original program.

All offsets are byte offsets from the beginning of the relevant file. Hexadecimal values use a `0x` prefix. Multi-byte integer fields discovered so far are little-endian unless stated otherwise.
