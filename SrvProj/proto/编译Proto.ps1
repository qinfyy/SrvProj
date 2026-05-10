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
    (Join-Path $ScriptDir "..\..\vcpkg_installed\x64-windows\x64-windows\tools\protobuf\protoc.exe"),
    (Join-Path $ScriptDir "..\..\vcpkg_installed\x86-windows\x86-windows\tools\protobuf\protoc.exe")
)

$Protoc = Try-Paths $Candidates

if (-not $Protoc -and $Env:VCPKG_ROOT) {

    $VcpkgRoot = $Env:VCPKG_ROOT

    $EnvCandidates = @(
        (Join-Path $VcpkgRoot "packages\protobuf_x64-windows\tools\protobuf\protoc.exe"),
        (Join-Path $VcpkgRoot "packages\protobuf_x86-windows\tools\protobuf\protoc.exe")
    )

    $Protoc = Try-Paths $EnvCandidates
}

if (-not $Protoc) {

    $vcpkgCmd = Get-Command vcpkg.exe -ErrorAction SilentlyContinue

    if ($vcpkgCmd) {

        $VcpkgRoot = Split-Path -Parent $vcpkgCmd.Source

        $VcpkgCandidates = @(
            (Join-Path $VcpkgRoot "packages\protobuf_x64-windows\tools\protobuf\protoc.exe"),
            (Join-Path $VcpkgRoot "packages\protobuf_x86-windows\tools\protobuf\protoc.exe")
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
    (Join-Path $ScriptDir "dump.pb.h"),
    (Join-Path $ScriptDir "dump.pb.cc"),
    (Join-Path $ScriptDir "NetMsgId.pb.h"),
    (Join-Path $ScriptDir "NetMsgId.pb.cc") `
    -Force `
    -ErrorAction SilentlyContinue

Start-Process `
    -FilePath $Protoc `
    -ArgumentList @(
        "--cpp_out=$ScriptDir",
        "dump.proto",
        "NetMsgId.proto"
    ) `
    -WorkingDirectory $ScriptDir `
    -NoNewWindow `
    -Wait
	
$ProtoDir = (Resolve-Path (Join-Path $ScriptDir "proto")).Path

$ProtoCppDir = Join-Path $ScriptDir "proto_cpp"

if (-not (Test-Path -Path $ProtoCppDir -PathType Container)) {
    New-Item -ItemType Directory -Path $ProtoCppDir -Force | Out-Null
}

if (-not (Test-Path $ProtoCppDir)) {

    New-Item `
        -ItemType Directory `
        -Path $ProtoCppDir | Out-Null
}

$ProtoCppDir = (Resolve-Path $ProtoCppDir).Path

Remove-Item `
    (Join-Path $ProtoCppDir "*.pb.h"),
    (Join-Path $ProtoCppDir "*.pb.cc") `
    -Force `
    -ErrorAction SilentlyContinue

$ProtoDir = (Resolve-Path (Join-Path $ScriptDir "proto")).Path
$ProtoOut = (Resolve-Path (Join-Path $ScriptDir "proto_cpp")).Path

if (-not (Test-Path $ProtoOut)) {
    New-Item -ItemType Directory -Path $ProtoOut | Out-Null
}

Push-Location $ProtoDir

$ProtoFiles = Get-ChildItem -Recurse -Filter "*.proto" |
    Where-Object {
        $_.FullName -notmatch '[\\/]google[\\/]'
    } |
    ForEach-Object {
        $_.FullName.Substring($ProtoDir.Length + 1)
    }

$Args = @(
    "--proto_path=.",
    "--cpp_out=$ProtoOut"
)

$Args += $ProtoFiles

Start-Process -FilePath $Protoc `
    -ArgumentList $Args `
    -WorkingDirectory $ProtoDir `
    -NoNewWindow `
    -Wait

Pop-Location

Write-Host ""
Write-Host "[OK] Protobuf generation completed." -ForegroundColor Green
Write-Host ""

Pause
exit 0
