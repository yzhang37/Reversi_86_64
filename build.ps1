$ErrorActionPreference = 'Stop'

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$Source = Join-Path $Root 'src\reversi_win32.c'
$Resource = Join-Path $Root 'src\reversi.rc'

function Find-VisualStudio {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (Test-Path $vswhere) {
        $path = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        if ($LASTEXITCODE -eq 0 -and $path -and (Test-Path $path)) {
            return $path
        }
    }

    $fallbacks = @(
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Professional",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Enterprise",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Community",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Professional",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Enterprise"
    )

    foreach ($candidate in $fallbacks) {
        if (Test-Path $candidate) {
            return $candidate
        }
    }

    throw 'Visual Studio with MSVC was not found.'
}

function Invoke-Build {
    param(
        [Parameter(Mandatory = $true)][string]$Arch,
        [Parameter(Mandatory = $true)][string]$OutputDir,
        [Parameter(Mandatory = $true)][string]$Defines,
        [string]$CompilerOptions = '',
        [Parameter(Mandatory = $true)][string]$Subsystem,
        [Parameter(Mandatory = $true)][string]$OsVersion
    )

    New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null

    $vsRoot = Find-VisualStudio
    $vcvars = Join-Path $vsRoot 'VC\Auxiliary\Build\vcvarsall.bat'
    if (!(Test-Path $vcvars)) {
        throw "vcvarsall.bat was not found at $vcvars"
    }

    $objDir = Join-Path $OutputDir 'obj'
    New-Item -ItemType Directory -Force -Path $objDir | Out-Null

    $exe = Join-Path $OutputDir 'REVERSI.exe'
    $res = Join-Path $objDir 'reversi.res'
    $cmd = '"' + $vcvars + '" ' + $Arch +
        ' && cd /d "' + $Root + '"' +
        ' && rc /nologo /I "' + (Join-Path $Root 'src') + '" /fo "' + $res + '" "' + $Resource + '"' +
        ' && cl /nologo /O2 /W4 /utf-8 /GS- /Zl ' + $CompilerOptions + ' ' + $Defines + ' ' +
        '/Fo"' + $objDir + '\\" /Fe"' + $exe + '" "' + $Source + '" "' + $res + '" ' +
        '/link /NODEFAULTLIB /ENTRY:ReversiEntry /SUBSYSTEM:WINDOWS,' + $Subsystem +
        ' /OSVERSION:' + $OsVersion + ' kernel32.lib user32.lib gdi32.lib shell32.lib'

    Write-Host "Building $Arch -> $exe"
    & cmd.exe /d /c $cmd
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed for $Arch"
    }
}

function Set-PeVersion {
    param(
        [Parameter(Mandatory = $true)][string]$Path,
        [Parameter(Mandatory = $true)][uint16]$Major,
        [Parameter(Mandatory = $true)][uint16]$Minor
    )

    $bytes = [IO.File]::ReadAllBytes($Path)
    $pe = [BitConverter]::ToInt32($bytes, 0x3c)
    $optional = $pe + 24
    [BitConverter]::GetBytes($Major).CopyTo($bytes, $optional + 40)
    [BitConverter]::GetBytes($Minor).CopyTo($bytes, $optional + 42)
    [BitConverter]::GetBytes($Major).CopyTo($bytes, $optional + 48)
    [BitConverter]::GetBytes($Minor).CopyTo($bytes, $optional + 50)
    [IO.File]::WriteAllBytes($Path, $bytes)
}

Invoke-Build `
    -Arch 'x86' `
    -OutputDir (Join-Path $Root 'build\x86') `
    -Defines '/DWINVER=0x0400 /D_WIN32_WINNT=0x0400 /D_WIN32_WINDOWS=0x0400' `
    -CompilerOptions '/arch:IA32' `
    -Subsystem '5.01' `
    -OsVersion '5.01'
Set-PeVersion -Path (Join-Path $Root 'build\x86\REVERSI.exe') -Major 4 -Minor 0
Invoke-Build `
    -Arch 'x64' `
    -OutputDir (Join-Path $Root 'build\x64') `
    -Defines '/DUNICODE /D_UNICODE /DWINVER=0x0600 /D_WIN32_WINNT=0x0600' `
    -CompilerOptions '' `
    -Subsystem '6.0' `
    -OsVersion '6.0'

Write-Host ''
Write-Host 'Done:'
Write-Host ('  ' + (Join-Path $Root 'build\x86\REVERSI.exe'))
Write-Host ('  ' + (Join-Path $Root 'build\x64\REVERSI.exe'))
