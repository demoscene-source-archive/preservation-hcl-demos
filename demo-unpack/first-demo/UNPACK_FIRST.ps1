param([string]$SevenZip)

$ErrorActionPreference = 'Stop'
if (-not $SevenZip) {
    $command = Get-Command 7z.exe -ErrorAction SilentlyContinue
    if ($command) { $SevenZip = $command.Source }
    else { $SevenZip = Join-Path $env:ProgramFiles '7-Zip\7z.exe' }
}
if (-not (Test-Path -LiteralPath $SevenZip -PathType Leaf)) {
    throw '7-Zip is required. Install it or pass -SevenZip with the path to 7z.exe.'
}

$archive = Join-Path $PSScriptRoot 'FIRST.HCL'
$output = Join-Path $PSScriptRoot 'FIRST'
if (Test-Path -LiteralPath $output) {
    throw "Output already exists: $output. Use a fresh output directory."
}

# FIRST.HCL is an ARJ self-extracting executable, not an HCL resource container.
# Read its archive with 7-Zip; do not execute the DOS self-extractor.
& $SevenZip x $archive "-o$output" -aos
if ($LASTEXITCODE -ne 0) {
    throw "7-Zip extraction failed (exit code $LASTEXITCODE)."
}
