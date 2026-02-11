param(
    [string]$Configuration = "Release",
    [string]$OutName = "MemTrans.exe"
)

$ErrorActionPreference = "Stop"

$gpp = "C:\msys64\ucrt64\bin\g++.exe"
$windres = "C:\msys64\ucrt64\bin\windres.exe"

if (!(Test-Path $gpp)) { throw "g++ not found: $gpp" }
if (!(Test-Path $windres)) { throw "windres not found: $windres" }

$flags = @("-std=c++17", "-O2", "-Wall", "-Wextra", "-pedantic", "-DNDEBUG")
if ($Configuration -ieq "Debug") {
    $flags = @("-std=c++17", "-O0", "-g", "-Wall", "-Wextra", "-pedantic")
}

& $windres .\resources\version.rc -O coff -o .\resources\version.res
& $gpp @flags .\\MemTrans.cpp .\resources\version.res -o $OutName -mwindows

Write-Host "Build success: $OutName"

