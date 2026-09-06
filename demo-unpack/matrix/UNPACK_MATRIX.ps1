$ErrorActionPreference = 'Stop'

# Matrix ships loose assets rather than an HCL container.
$extractor = Join-Path $PSScriptRoot '..\..\bin\hcl_unpack.exe'
$output = Join-Path $PSScriptRoot 'MATRIX'
if (Test-Path -LiteralPath $output) {
    throw "Output already exists: $output. Use a fresh output directory."
}

$files = foreach ($number in 1..33) {
    $name = '{0:D2}.HCL' -f $number
    $source = Join-Path $PSScriptRoot $name
    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Missing release asset: $source"
    }
    $source
}

New-Item -ItemType Directory -Path $output | Out-Null
foreach ($file in $files) {
    & $extractor --asset $file $output
    if ($LASTEXITCODE -ne 0) {
        throw "Asset extraction failed: $file (exit code $LASTEXITCODE)"
    }
}
Write-Host 'Prepared 33 assets in MATRIX using automatic format detection.'
