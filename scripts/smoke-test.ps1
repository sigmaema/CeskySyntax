$ErrorActionPreference = 'Stop'

$envFile = Join-Path $PSScriptRoot '..\.env'
if (Test-Path $envFile) {
    Get-Content $envFile | ForEach-Object {
        $line = $_.Trim()
        if (-not $line -or $line.StartsWith('#')) { return }
        $parts = $line.Split('=', 2)
        if ($parts.Count -eq 2) {
            [System.Environment]::SetEnvironmentVariable($parts[0].Trim(), $parts[1].Trim(), 'Process')
        }
    }
}

$toolBin = $env:CESKYSYNTAX_TOOLBIN
if (-not $toolBin) {
    throw 'Set CESKYSYNTAX_TOOLBIN in .env or your shell environment.'
}

$compiler = $env:CESKYSYNTAX_COMPILER
if (-not $compiler) {
    $compiler = 'g++.exe'
}

$transpiler = Join-Path $PSScriptRoot '..\build\cesky_transpiler.exe'
$source = Join-Path $PSScriptRoot '..\examples\demo.csx'
$outputCpp = Join-Path $PSScriptRoot '..\generated.cpp'
$exePath = Join-Path $PSScriptRoot '..\generated.exe'

if (-not (Test-Path $transpiler)) {
    throw 'Build the transpiler first. Expected build\\cesky_transpiler.exe.'
}

$env:PATH = "$toolBin;$env:PATH"

& $transpiler $source $outputCpp --run --compiler=$compiler
if ($LASTEXITCODE -ne 0) {
    throw 'Transpiler pipeline failed.'
}

if (-not (Test-Path $outputCpp)) {
    throw 'Generated C++ file was not created.'
}

$generated = Get-Content $outputCpp -Raw
if ($generated -notmatch 'int main\(') {
    throw 'Generated C++ does not contain main().' 
}

Write-Host 'Smoke test passed.'
