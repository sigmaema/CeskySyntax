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

$env:PATH = "$toolBin;$env:PATH"

Set-Location (Join-Path $PSScriptRoot '..')

& "$toolBin\g++.exe" -std=c++17 -O2 -Iinclude src/main.cpp src/Lexer.cpp src/Translator.cpp src/CodeGenerator.cpp src/Executor.cpp -o build/cesky_transpiler.exe

if ($LASTEXITCODE -ne 0) {
    throw "Build failed with exit code $LASTEXITCODE"
}

Write-Host "Build successful: cesky_transpiler.exe"
