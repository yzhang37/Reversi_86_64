param(
    [switch]$SkipDownloads,
    [switch]$RunInstallers
)

$ErrorActionPreference = 'Stop'

$ToolsDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$Root = Split-Path -Parent $ToolsDir
$ManifestPath = Join-Path $ToolsDir 'tools-manifest.json'
$Manifest = Get-Content -LiteralPath $ManifestPath -Raw | ConvertFrom-Json
$CacheDir = Join-Path $Root $Manifest.cacheDir
$VendorDir = Join-Path $Root $Manifest.vendorDir

function New-Directory {
    param([Parameter(Mandatory = $true)][string]$Path)
    New-Item -ItemType Directory -Force -Path $Path | Out-Null
}

function Resolve-RepoPath {
    param([Parameter(Mandatory = $true)][string]$Path)
    return Join-Path $Root $Path
}

function Test-Sha256 {
    param(
        [Parameter(Mandatory = $true)][string]$Path,
        [Parameter(Mandatory = $true)][string]$Expected
    )

    if (!(Test-Path -LiteralPath $Path)) {
        return $false
    }

    $actual = (Get-FileHash -Algorithm SHA256 -LiteralPath $Path).Hash
    return [String]::Equals($actual, $Expected, [StringComparison]::OrdinalIgnoreCase)
}

function Get-ToolArchive {
    param([Parameter(Mandatory = $true)]$Tool)

    $archive = Join-Path $CacheDir $Tool.archive
    if (Test-Sha256 -Path $archive -Expected $Tool.sha256) {
        Write-Host "Using cached $($Tool.archive)"
        return $archive
    }

    if ($SkipDownloads) {
        throw "$($Tool.archive) is missing or has the wrong SHA256. Rerun without -SkipDownloads."
    }

    Write-Host "Downloading $($Tool.name)"
    Invoke-WebRequest -Uri $Tool.url -OutFile $archive

    if (!(Test-Sha256 -Path $archive -Expected $Tool.sha256)) {
        Remove-Item -LiteralPath $archive -Force -ErrorAction SilentlyContinue
        throw "SHA256 mismatch for $($Tool.archive)"
    }

    return $archive
}

function Copy-DirectoryContents {
    param(
        [Parameter(Mandatory = $true)][string]$Source,
        [Parameter(Mandatory = $true)][string]$Destination
    )

    New-Directory $Destination
    Get-ChildItem -LiteralPath $Source -Force |
        Copy-Item -Destination $Destination -Recurse -Force
}

function Install-HelpWorkshop {
    param([Parameter(Mandatory = $true)]$Tool)

    $check = Resolve-RepoPath $Tool.installedCheck
    if (Test-Path -LiteralPath $check) {
        Write-Host "Help Workshop is ready at $check"
        return
    }

    $legacyHcw = Join-Path $ToolsDir 'hcw'
    if (Test-Path -LiteralPath (Join-Path $legacyHcw 'hcw.exe')) {
        Copy-DirectoryContents -Source $legacyHcw -Destination (Join-Path $VendorDir 'hcw')
        Write-Host "Migrated existing tools/hcw to tools/vendor/hcw"
        return
    }

    $archive = Get-ToolArchive $Tool
    $extracted = Join-Path $CacheDir 'hcwsetup_extracted'
    if (!(Test-Path -LiteralPath (Join-Path $extracted '_instpgm.ini'))) {
        New-Directory $extracted
        & tar.exe -xf $archive -C $extracted
        if ($LASTEXITCODE -ne 0) {
            throw "Failed to extract $archive"
        }
    }

    if ($RunInstallers) {
        Write-Host "Launching Help Workshop setup. Install or copy it to tools/vendor/hcw."
        Start-Process -FilePath $archive -Wait
    }

    if (!(Test-Path -LiteralPath $check)) {
        Write-Warning "Help Workshop installer was downloaded, but hcw.exe is not installed under tools/vendor/hcw."
        Write-Warning "Run tools/cache/Hcwsetup.exe or copy an existing Help Workshop install to tools/vendor/hcw."
    }
}

function Build-HelpDeco {
    param([Parameter(Mandatory = $true)][string]$SourceDir)

    $exe = Join-Path $SourceDir 'helpdeco.exe'
    if (Test-Path -LiteralPath $exe) {
        return
    }

    $cl = Get-Command cl.exe -ErrorAction SilentlyContinue
    if (!$cl) {
        Write-Warning "HelpDeco source is present, but cl.exe was not found. Build helpdeco.exe from a Visual Studio Developer Command Prompt if needed."
        return
    }

    Push-Location $SourceDir
    try {
        & cl.exe /nologo /O2 /Fe:helpdeco.exe helpdeco.c helpdec1.c compat.c
        if ($LASTEXITCODE -ne 0) {
            throw "cl.exe failed while building HelpDeco"
        }
    } finally {
        Pop-Location
    }
}

function Install-HelpDeco {
    param([Parameter(Mandatory = $true)]$Tool)

    $sourceDir = Resolve-RepoPath $Tool.sourceDir
    $check = Resolve-RepoPath $Tool.installedCheck

    if (!(Test-Path -LiteralPath $sourceDir)) {
        $legacySource = Join-Path $ToolsDir 'helpdeco-2.1.4'
        if (Test-Path -LiteralPath $legacySource) {
            Copy-DirectoryContents -Source $legacySource -Destination $sourceDir
            Write-Host "Migrated existing tools/helpdeco-2.1.4 to tools/vendor/helpdeco-2.1.4"
        } else {
            $archive = Get-ToolArchive $Tool
            & tar.exe -xzf $archive -C $VendorDir
            if ($LASTEXITCODE -ne 0) {
                throw "Failed to extract $archive"
            }
        }
    }

    Build-HelpDeco -SourceDir $sourceDir
    if (Test-Path -LiteralPath $check) {
        Write-Host "HelpDeco is ready at $check"
    }
}

New-Directory $CacheDir
New-Directory $VendorDir

foreach ($tool in $Manifest.tools) {
    switch ($tool.id) {
    'help-workshop' { Install-HelpWorkshop $tool; break }
    'helpdeco' { Install-HelpDeco $tool; break }
    default { Write-Warning "Unknown tool id: $($tool.id)" }
    }
}

Write-Host ''
Write-Host 'Tool bootstrap complete.'
