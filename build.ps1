param(
    [switch]$Production,
    [switch]$Mui,
    [switch]$KeepIntermediate,
    [switch]$Clean,
    [switch]$Rebuild,
    [string[]]$Locale,
    [switch]$ListLocales
)

$ErrorActionPreference = 'Stop'

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$Source = Join-Path $Root 'src\reversi_win32.c'
$CpuSource = Join-Path $Root 'src\reversi_cpu_x86.c'
$ModernSource = Join-Path $Root 'src\reversi_modern_x86.c'
$Resource = Join-Path $Root 'src\reversi.rc'
$HelpGenerator = Join-Path $Root 'help\make_hlp_sources.py'
$HelpCompiler = Join-Path $Root 'tools\vendor\hcw\hcw.exe'

$Languages = @(
    @{ Locale = 'en-US'; Define = 'REVERSI_LANG_EN_US'; Hex = '0409' },
    @{ Locale = 'zh-CN'; Define = 'REVERSI_LANG_ZH_CN'; Hex = '0804' },
    @{ Locale = 'zh-TW'; Define = 'REVERSI_LANG_ZH_TW'; Hex = '0404' },
    @{ Locale = 'ja-JP'; Define = 'REVERSI_LANG_JA_JP'; Hex = '0411' },
    @{ Locale = 'ko-KR'; Define = 'REVERSI_LANG_KO_KR'; Hex = '0412' },
    @{ Locale = 'fr-FR'; Define = 'REVERSI_LANG_FR_FR'; Hex = '040c' },
    @{ Locale = 'de-DE'; Define = 'REVERSI_LANG_DE_DE'; Hex = '0407' },
    @{ Locale = 'es-ES'; Define = 'REVERSI_LANG_ES_ES'; Hex = '040a' },
    @{ Locale = 'sv-SE'; Define = 'REVERSI_LANG_SV_SE'; Hex = '041d' },
    @{ Locale = 'fi-FI'; Define = 'REVERSI_LANG_FI_FI'; Hex = '040b' },
    @{ Locale = 'pt-PT'; Define = 'REVERSI_LANG_PT_PT'; Hex = '0816' },
    @{ Locale = 'it-IT'; Define = 'REVERSI_LANG_IT_IT'; Hex = '0410' },
    @{ Locale = 'ru-RU'; Define = 'REVERSI_LANG_RU_RU'; Hex = '0419' },
    @{ Locale = 'uk-UA'; Define = 'REVERSI_LANG_UK_UA'; Hex = '0422' },
    @{ Locale = 'ar-SA'; Define = 'REVERSI_LANG_AR_SA'; Hex = '0401' },
    @{ Locale = 'he-IL'; Define = 'REVERSI_LANG_HE_IL'; Hex = '040d' }
)

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

function Select-Languages {
    if (!$Locale -or $Locale.Count -eq 0) {
        return $Languages
    }

    $requested = @()
    foreach ($entry in $Locale) {
        $requested += $entry -split ',' | Where-Object { $_.Trim().Length -gt 0 } | ForEach-Object { $_.Trim() }
    }

    $selected = @()
    foreach ($name in $requested) {
        if ($name -ieq 'all') {
            return $Languages
        }

        $match = $Languages | Where-Object { $_.Locale -ieq $name } | Select-Object -First 1
        if (!$match) {
            $known = ($Languages | ForEach-Object { $_.Locale }) -join ', '
            throw "Unknown locale '$name'. Known locales: $known"
        }
        $selected += $match
    }
    return $selected
}

function Show-Languages {
    Write-Host 'Supported locales:'
    foreach ($language in $Languages) {
        Write-Host ('  ' + $language.Locale)
    }
}

function Invoke-Build {
    param(
        [Parameter(Mandatory = $true)][string]$Arch,
        [Parameter(Mandatory = $true)][string]$OutputDir,
        [Parameter(Mandatory = $true)][string]$Defines,
        [string]$Optimization = '/O2',
        [string]$CompilerOptions = '',
        [string]$LinkerOptions = '',
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
    $mainObj = Join-Path $objDir 'reversi_win32.obj'
    $cpuObj = Join-Path $objDir 'reversi_cpu_x86.obj'
    $modernObj = Join-Path $objDir 'reversi_modern_x86.obj'
    $linkObjects = '"' + $mainObj + '" "' + $res + '"'

    if ($Arch -eq 'x86') {
        $baseDefines = ($Defines -replace ' *\/DREVERSI_LANG_[A-Za-z0-9_]+', '').Trim()
        $cacheDir = Join-Path $Root 'build\.cache\obj\x86'
        $sharedCpuObj = Join-Path $cacheDir 'reversi_cpu_x86.obj'
        $sharedModernObj = Join-Path $cacheDir 'reversi_modern_x86.obj'
        Ensure-SharedX86Object -SourcePath $CpuSource -OutputPath $sharedCpuObj -Defines $baseDefines -Optimization $Optimization -CompileArch 'IA32'
        Ensure-SharedX86Object -SourcePath $ModernSource -OutputPath $sharedModernObj -Defines $baseDefines -Optimization $Optimization -CompileArch 'SSE2'
        Copy-Item -LiteralPath $sharedCpuObj -Destination $cpuObj -Force
        Copy-Item -LiteralPath $sharedModernObj -Destination $modernObj -Force
        $linkObjects += ' "' + $cpuObj + '" "' + $modernObj + '"'
    }

    $cmd = '"' + $vcvars + '" ' + $Arch +
        ' && cd /d "' + $Root + '"' +
        ' && rc /nologo ' + $Defines + ' /I "' + (Join-Path $Root 'src') + '" /fo "' + $res + '" "' + $Resource + '"' +
        ' && cl /nologo ' + $Optimization + ' /W4 /utf-8 /GS- /Zl ' + $CompilerOptions + ' ' + $Defines + ' ' +
        '/c /Fo"' + $mainObj + '" "' + $Source + '"' +
        ' && link /nologo /NODEFAULTLIB /ENTRY:ReversiEntry /SUBSYSTEM:WINDOWS,' + $Subsystem +
        ' ' + $LinkerOptions +
        ' /OSVERSION:' + $OsVersion + ' /OUT:"' + $exe + '" ' + $linkObjects +
        ' kernel32.lib user32.lib gdi32.lib shell32.lib advapi32.lib'

    Write-Host "Building $Arch -> $exe"
    & cmd.exe /d /c $cmd
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed for $Arch"
    }
}

function Invoke-MuiResourceBuild {
    param(
        [Parameter(Mandatory = $true)][string]$Arch,
        [Parameter(Mandatory = $true)][string]$OutputPath,
        [Parameter(Mandatory = $true)][string]$Defines
    )

    $vsRoot = Find-VisualStudio
    $vcvars = Join-Path $vsRoot 'VC\Auxiliary\Build\vcvarsall.bat'
    if (!(Test-Path $vcvars)) {
        throw "vcvarsall.bat was not found at $vcvars"
    }

    $outputDir = Split-Path -Parent $OutputPath
    $objDir = Join-Path $outputDir 'obj'
    New-Item -ItemType Directory -Force -Path $objDir | Out-Null

    $res = Join-Path $objDir 'reversi.res'
    $machine = if ($Arch -eq 'x64') { 'X64' } else { 'X86' }
    $subsystem = if ($Arch -eq 'x64') { '6.0' } else { '5.01' }
    $osVersion = if ($Arch -eq 'x64') { '6.0' } else { '5.01' }
    $cmd = '"' + $vcvars + '" ' + $Arch +
        ' && cd /d "' + $Root + '"' +
        ' && rc /nologo ' + $Defines + ' /I "' + (Join-Path $Root 'src') + '" /fo "' + $res + '" "' + $Resource + '"' +
        ' && link /nologo /NOENTRY /DLL /SUBSYSTEM:WINDOWS,' + $subsystem +
        ' /OSVERSION:' + $osVersion + ' /MACHINE:' + $machine +
        ' /OUT:"' + $OutputPath + '" "' + $res + '"'

    Write-Host "Building MUI $Arch -> $OutputPath"
    & cmd.exe /d /c $cmd
    if ($LASTEXITCODE -ne 0) {
        throw "MUI resource build failed for $Arch -> $OutputPath"
    }
}

function Invoke-HelpBuild {
    param(
        [Parameter(Mandatory = $true)][string]$Locale
    )

    if (!(Test-Path $HelpGenerator)) {
        return
    }

    Write-Host "Generating WinHelp sources -> help\$Locale"
    & python $HelpGenerator --locale $Locale
    if ($LASTEXITCODE -ne 0) {
        throw "WinHelp source generation failed for $Locale"
    }

    $helpDir = Join-Path $Root ("help\$Locale")
    $hpj = Join-Path $helpDir 'REVERSI.hpj'
    if ((Test-Path $HelpCompiler) -and (Test-Path $hpj)) {
        Write-Host "Compiling WinHelp -> help\$Locale\REVERSI.HLP"
        & $HelpCompiler /C /E $hpj
        $hlp = Join-Path $helpDir 'REVERSI.HLP'
        if ($LASTEXITCODE -ne 0 -and !(Test-Path $hlp)) {
            throw "WinHelp compile failed for $Locale"
        }
    } elseif (Test-Path $hpj) {
        Write-Warning "Help Workshop not found; generated sources but skipped HLP compile for $Locale."
    }
}

function Copy-LocaleHelpFiles {
    param(
        [Parameter(Mandatory = $true)][string]$OutputDir,
        [Parameter(Mandatory = $true)][string]$Locale
    )

    $helpDir = Join-Path $Root ("help\$Locale")
    New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null
    $copied = @()
    foreach ($file in @('REVERSI.HLP', 'REVERSI.CNT', 'REVERSI.CHM')) {
        $sourcePath = Join-Path $helpDir $file
        if (Test-Path $sourcePath) {
            $destination = Join-Path $OutputDir $file
            Copy-Item -LiteralPath $sourcePath -Destination $destination -Force
            $copied += $destination
        }
    }
    return $copied
}

function Copy-LocaleResourceFiles {
    param(
        [Parameter(Mandatory = $true)][string]$OutputDir,
        [Parameter(Mandatory = $true)][string]$Locale,
        [Parameter(Mandatory = $true)][string]$ResourcePath
    )

    New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null
    if (Test-Path $ResourcePath) {
        Copy-Item -LiteralPath $ResourcePath -Destination (Join-Path $OutputDir 'reversi.res') -Force
    }

    $localeRc = Join-Path $Root ("src\lang\reversi_$Locale.rcinc")
    if (Test-Path $localeRc) {
        Copy-Item -LiteralPath $localeRc -Destination (Join-Path $OutputDir "reversi_$Locale.rcinc") -Force
    }
}

function Remove-ArchHelpFiles {
    param([Parameter(Mandatory = $true)][string]$OutputDir)

    foreach ($file in @('reversi.hlp', 'reversi.cnt', 'reversi.chm', 'REVERSI.HLP', 'REVERSI.CNT', 'REVERSI.CHM')) {
        Remove-Item -LiteralPath (Join-Path $OutputDir $file) -Force -ErrorAction SilentlyContinue
    }
}

function Ensure-SharedX86Object {
    param(
        [Parameter(Mandatory = $true)][string]$SourcePath,
        [Parameter(Mandatory = $true)][string]$OutputPath,
        [Parameter(Mandatory = $true)][string]$Defines,
        [Parameter(Mandatory = $true)][string]$CompileArch,
        [string]$Optimization = '/O2'
    )

    New-Item -ItemType Directory -Force -Path (Split-Path -Parent $OutputPath) | Out-Null

    $needsCompile = -not (Test-Path $OutputPath)
    if (!$needsCompile) {
        $sourceTime = (Get-Item $SourcePath).LastWriteTimeUtc
        $objectTime = (Get-Item $OutputPath).LastWriteTimeUtc
        $needsCompile = $sourceTime -gt $objectTime
    }

    if (!$needsCompile) {
        return
    }

    $vsRoot = Find-VisualStudio
    $vcvars = Join-Path $vsRoot 'VC\Auxiliary\Build\vcvarsall.bat'
    if (!(Test-Path $vcvars)) {
        throw "vcvarsall.bat was not found at $vcvars"
    }

    $compilerCmd = '"' + $vcvars + '" x86 && cd /d "' + $Root + '"' +
        ' && cl /nologo ' + $Optimization + ' /W4 /utf-8 /GS- /Zl ' + $Defines + ' /arch:' + $CompileArch +
        ' /c /Fo"' + $OutputPath + '" "' + $SourcePath + '"'

    Write-Host ("Building shared x86 object -> $OutputPath")
    & cmd.exe /d /c $compilerCmd
    if ($LASTEXITCODE -ne 0) {
        throw "Failed to build shared x86 object: $OutputPath"
    }
}

function Remove-IntermediateFiles {
    param([Parameter(Mandatory = $true)][string]$OutputDir)

    if ($KeepIntermediate) {
        return
    }

    # Release folders should contain runnable files, not compiler/resource scratch state.
    Remove-Item -LiteralPath (Join-Path $OutputDir 'obj') -Recurse -Force -ErrorAction SilentlyContinue
    foreach ($file in @('reversi.res', 'reversi_*.rcinc')) {
        Remove-Item -Path (Join-Path $OutputDir $file) -Force -ErrorAction SilentlyContinue
    }
}

function Clear-OutputPath {
    param([Parameter(Mandatory = $true)][string]$Path)

    $buildRoot = [IO.Path]::GetFullPath((Join-Path $Root 'build'))
    $target = [IO.Path]::GetFullPath($Path)
    $withinBuildTree = $target -eq $buildRoot -or $target.StartsWith($buildRoot + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)
    if (!$withinBuildTree) {
        throw "Refusing to clean unexpected path: $target"
    }

    if (Test-Path $target) {
        Remove-Item -LiteralPath $target -Recurse -Force -ErrorAction SilentlyContinue
    }
}

function Invoke-Clean {
    param(
        [Parameter(Mandatory = $true)]$SelectedLanguages,
        [switch]$MuiBuild
    )

    if ($MuiBuild) {
        Clear-MuiOutput -Path (Join-Path $Root 'build\MUI')
        return
    }

    foreach ($language in $SelectedLanguages) {
        $localeOutput = Join-Path $Root ("build\$($language.Locale)")
        Clear-OutputPath -Path $localeOutput
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

function Get-LanguageByLocale {
    param([Parameter(Mandatory = $true)][string]$Name)

    $match = $Languages | Where-Object { $_.Locale -ieq $Name } | Select-Object -First 1
    if (!$match) {
        throw "Internal build error: locale '$Name' was not found."
    }
    return $match
}

function Clear-MuiOutput {
    param([Parameter(Mandatory = $true)][string]$Path)

    $buildRoot = [IO.Path]::GetFullPath((Join-Path $Root 'build\MUI'))
    $target = [IO.Path]::GetFullPath($Path)
    # This build owns build\MUI completely; never let a bad path turn cleanup into a tree wipe.
    if (!$target.Equals($buildRoot, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to clean unexpected MUI output path: $target"
    }
    if (Test-Path $target) {
        Remove-Item -LiteralPath $target -Recurse -Force
    }
}

function Invoke-MuiPackage {
    param(
        [Parameter(Mandatory = $true)]$SelectedLanguages,
        [Parameter(Mandatory = $true)]$Built
    )

    $muiRoot = Join-Path $Root 'build\MUI'
    $baseLanguage = Get-LanguageByLocale -Name 'en-US'

    Clear-MuiOutput -Path $muiRoot

    foreach ($language in $SelectedLanguages) {
        Invoke-HelpBuild -Locale $language.Locale
    }

    foreach ($arch in @('x86', 'x64')) {
        $archOutput = Join-Path $muiRoot $arch
        if ($arch -eq 'x86') {
            Invoke-Build `
                -Arch 'x86' `
                -OutputDir $archOutput `
                -Defines ('/DWINVER=0x0400 /D_WIN32_WINNT=0x0400 /D_WIN32_WINDOWS=0x0400' + $productionDefine + ' /DREVERSI_SINGLE_LANGUAGE /D' + $baseLanguage.Define) `
                -CompilerOptions '/arch:IA32' `
                -Subsystem '5.01' `
                -OsVersion '5.01'
            Set-PeVersion -Path (Join-Path $archOutput 'REVERSI.exe') -Major 4 -Minor 0
        } else {
            Invoke-Build `
                -Arch 'x64' `
                -OutputDir $archOutput `
                -Defines ('/DUNICODE /D_UNICODE /DWINVER=0x0600 /D_WIN32_WINNT=0x0600' + $productionDefine + ' /DREVERSI_SINGLE_LANGUAGE /D' + $baseLanguage.Define) `
                -Optimization '/O1' `
                -CompilerOptions '/Gy /Gw' `
                -LinkerOptions '/OPT:REF /OPT:ICF' `
                -Subsystem '6.0' `
                -OsVersion '6.0'
        }
        Remove-ArchHelpFiles -OutputDir $archOutput
        Remove-IntermediateFiles -OutputDir $archOutput
        $Built.Add((Join-Path $archOutput 'REVERSI.exe')) | Out-Null

        foreach ($language in $SelectedLanguages) {
            $localeDir = Join-Path $archOutput $language.Locale
            $legacyLocaleDir = Join-Path $archOutput ("MUI\$($language.Hex)")
            $localeDefine = ' /DREVERSI_SINGLE_LANGUAGE /D' + $language.Define
            $commonDefines = if ($arch -eq 'x86') {
                '/DWINVER=0x0400 /D_WIN32_WINNT=0x0400 /D_WIN32_WINDOWS=0x0400'
            } else {
                '/DUNICODE /D_UNICODE /DWINVER=0x0600 /D_WIN32_WINNT=0x0600'
            }
            $muiPath = Join-Path $localeDir 'REVERSI.exe.mui'
            Invoke-MuiResourceBuild `
                -Arch $arch `
                -OutputPath $muiPath `
                -Defines ($commonDefines + $productionDefine + $localeDefine)
            if ($arch -eq 'x86') {
                Set-PeVersion -Path $muiPath -Major 4 -Minor 0
            }
            $copiedHelp = Copy-LocaleHelpFiles -OutputDir $localeDir -Locale $language.Locale
            foreach ($helpFile in $copiedHelp) {
                $Built.Add($helpFile) | Out-Null
            }
            Remove-IntermediateFiles -OutputDir $localeDir

            # Vista+ uses named locale folders; XP MUI commonly uses hexadecimal LANGID folders.
            New-Item -ItemType Directory -Force -Path $legacyLocaleDir | Out-Null
            Copy-Item -LiteralPath $muiPath -Destination (Join-Path $legacyLocaleDir 'REVERSI.exe.mui') -Force
            foreach ($file in @('REVERSI.HLP', 'REVERSI.CNT', 'REVERSI.CHM')) {
                $sourcePath = Join-Path $localeDir $file
                if (Test-Path $sourcePath) {
                    Copy-Item -LiteralPath $sourcePath -Destination (Join-Path $legacyLocaleDir $file) -Force
                    $Built.Add((Join-Path $legacyLocaleDir $file)) | Out-Null
                }
            }
            $Built.Add($muiPath) | Out-Null
            $Built.Add((Join-Path $legacyLocaleDir 'REVERSI.exe.mui')) | Out-Null
        }
    }
}

$productionDefine = if ($Production) { ' /DREVERSI_PRODUCTION' } else { '' }
if ($ListLocales) {
    Show-Languages
    exit 0
}

$selectedLanguages = Select-Languages
$built = @()

if ($Clean -or $Rebuild) {
    Invoke-Clean -SelectedLanguages $selectedLanguages -MuiBuild:$Mui
    if ($Clean) {
        exit 0
    }
}

if ($Mui) {
    $built = New-Object System.Collections.Generic.List[string]
    Invoke-MuiPackage -SelectedLanguages $selectedLanguages -Built $built
    Write-Host ''
    Write-Host 'Done:'
    foreach ($path in $built) {
        Write-Host ('  ' + $path)
    }
    exit 0
}

foreach ($language in $selectedLanguages) {
    $localeDefine = ' /DREVERSI_SINGLE_LANGUAGE /D' + $language.Define
    $localeOutput = Join-Path $Root ("build\$($language.Locale)")
    $x86Output = Join-Path $Root ("build\$($language.Locale)\x86")
    $x64Output = Join-Path $Root ("build\$($language.Locale)\x64")

    Invoke-HelpBuild -Locale $language.Locale
    Remove-ArchHelpFiles -OutputDir $localeOutput

    Invoke-Build `
        -Arch 'x86' `
        -OutputDir $x86Output `
        -Defines ('/DWINVER=0x0400 /D_WIN32_WINNT=0x0400 /D_WIN32_WINDOWS=0x0400' + $productionDefine + $localeDefine) `
        -CompilerOptions '/arch:IA32' `
        -Subsystem '5.01' `
        -OsVersion '5.01'
    Set-PeVersion -Path (Join-Path $x86Output 'REVERSI.exe') -Major 4 -Minor 0
    if ($KeepIntermediate) {
        Copy-LocaleResourceFiles -OutputDir $localeOutput -Locale $language.Locale -ResourcePath (Join-Path $x86Output 'obj\reversi.res')
    } else {
        Remove-IntermediateFiles -OutputDir $localeOutput
    }
    Remove-ArchHelpFiles -OutputDir $x86Output
    $copiedHelp = Copy-LocaleHelpFiles -OutputDir $x86Output -Locale $language.Locale
    foreach ($helpFile in $copiedHelp) {
        $built += $helpFile
    }
    Remove-IntermediateFiles -OutputDir $x86Output
    $built += Join-Path $x86Output 'REVERSI.exe'

    Invoke-Build `
        -Arch 'x64' `
        -OutputDir $x64Output `
        -Defines ('/DUNICODE /D_UNICODE /DWINVER=0x0600 /D_WIN32_WINNT=0x0600' + $productionDefine + $localeDefine) `
        -Optimization '/O1' `
        -CompilerOptions '/Gy /Gw' `
        -LinkerOptions '/OPT:REF /OPT:ICF' `
        -Subsystem '6.0' `
        -OsVersion '6.0'
    Remove-ArchHelpFiles -OutputDir $x64Output
    $copiedHelp = Copy-LocaleHelpFiles -OutputDir $x64Output -Locale $language.Locale
    foreach ($helpFile in $copiedHelp) {
        $built += $helpFile
    }
    Remove-IntermediateFiles -OutputDir $x64Output
    $built += Join-Path $x64Output 'REVERSI.exe'
}

Write-Host ''
Write-Host 'Done:'
foreach ($path in $built) {
    Write-Host ('  ' + $path)
}
