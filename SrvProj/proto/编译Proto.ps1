$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$Protoc = $null

function Try-Paths($Paths) {
    foreach ($Path in $Paths) {
        if (Test-Path $Path) {
            return (Resolve-Path $Path).Path
        }
    }
    return $null
}

$Candidates = @(
    Join-Path $ScriptDir "..\..\vcpkg_installed\x64-windows\x64-windows\tools\protobuf\protoc.exe"
    Join-Path $ScriptDir "..\..\vcpkg_installed\x86-windows\x86-windows\tools\protobuf\protoc.exe"
)
$Protoc = Try-Paths $Candidates

if (-not $Protoc -and $Env:VCPKG_ROOT) {
    $VcpkgRoot = $Env:VCPKG_ROOT
    $EnvCandidates = @(
        Join-Path $VcpkgRoot "packages\protobuf_x64-windows\tools\protobuf\protoc.exe"
        Join-Path $VcpkgRoot "packages\protobuf_x86-windows\tools\protobuf\protoc.exe"
    )
    $Protoc = Try-Paths $EnvCandidates
}

if (-not $Protoc) {
    $vcpkgCmd = Get-Command vcpkg.exe -ErrorAction SilentlyContinue
    if ($vcpkgCmd) {
        $VcpkgRoot = Split-Path -Parent $vcpkgCmd.Source
        $VcpkgCandidates = @(
            Join-Path $VcpkgRoot "packages\protobuf_x64-windows\tools\protobuf\protoc.exe"
            Join-Path $VcpkgRoot "packages\protobuf_x86-windows\tools\protobuf\protoc.exe"
        )
        $Protoc = Try-Paths $VcpkgCandidates
    }
}

if (-not $Protoc) {
    Write-Host "[ERROR] Cannot find protobuf protoc.exe" -ForegroundColor Red
    Write-Host ""
    Write-Host "Checked locations (in order):"
    Write-Host "  - ..\..\vcpkg_installed\x64-windows\x64-windows\tools\protobuf"
    Write-Host "  - ..\..\vcpkg_installed\x86-windows\x86-windows\tools\protobuf"
    Write-Host "  - VCPKG_ROOT\packages\protobuf_x64-windows\tools\protobuf"
    Write-Host "  - VCPKG_ROOT\packages\protobuf_x86-windows\tools\protobuf"
    Write-Host "  - vcpkg.exe in PATH (x64 & x86)"
    Write-Host ""
    Pause
    exit 1
}

Write-Host "[OK] Using protoc:" -ForegroundColor Green
Write-Host "    $Protoc"
Write-Host ""

Remove-Item `
    dump.pb.h, dump.pb.cc, CmdId.pb.h, CmdId.pb.cc `
    -Force -ErrorAction SilentlyContinue

& $Protoc --cpp_out=./ dump.proto

Pause
exit 0
