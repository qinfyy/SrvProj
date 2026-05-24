# 获取脚本所在目录
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

# 相对于脚本目录的 vcpkg 安装路径
$Candidates = @(
    (Join-Path $ScriptDir "..\..\vcpkg_installed\x64-windows\x64-windows\tools\protobuf\protoc.exe"),
    (Join-Path $ScriptDir "..\..\vcpkg_installed\x86-windows\x86-windows\tools\protobuf\protoc.exe")
)

$Protoc = Try-Paths $Candidates

# 通过环境变量 VCPKG_ROOT
if (-not $Protoc -and $Env:VCPKG_ROOT) {

    $VcpkgRoot = $Env:VCPKG_ROOT

    $EnvCandidates = @(
        (Join-Path $VcpkgRoot "packages\protobuf_x64-windows\tools\protobuf\protoc.exe"),
        (Join-Path $VcpkgRoot "packages\protobuf_x86-windows\tools\protobuf\protoc.exe")
    )

    $Protoc = Try-Paths $EnvCandidates
}

# 在 PATH 中找到 vcpkg.exe，并推测其包路径
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

# 若仍未找到 protoc，则报错退出
if (-not $Protoc) {
    Write-Host "[ERROR] 找不到 protobuf 的 protoc.exe" -ForegroundColor Red
    Write-Host ""
    Write-Host "已尝试以下位置（按顺序）："
    Write-Host "  - ..\..\vcpkg_installed\x64-windows\x64-windows\tools\protobuf"
    Write-Host "  - ..\..\vcpkg_installed\x86-windows\x86-windows\tools\protobuf"
    Write-Host "  - VCPKG_ROOT\packages\protobuf_x64-windows\tools\protobuf"
    Write-Host "  - VCPKG_ROOT\packages\protobuf_x86-windows\tools\protobuf"
    Write-Host "  - vcpkg.exe 所在目录的 packages\protobuf_x64-windows\tools\protobuf"
    Write-Host "  - vcpkg.exe 所在目录的 packages\protobuf_x86-windows\tools\protobuf"
    Write-Host ""
    Pause
    exit 1
}

Write-Host "[OK] 使用 protoc：" -ForegroundColor Green
Write-Host "    $Protoc"
Write-Host ""

# 清理根目录上一次生成的 C++ 文件
<# Remove-Item `
    (Join-Path $ScriptDir "dump.pb.h"),
    (Join-Path $ScriptDir "dump.pb.cc"),
    (Join-Path $ScriptDir "NetMsgId.pb.h"),
    (Join-Path $ScriptDir "NetMsgId.pb.cc") `
    -Force `
    -ErrorAction SilentlyContinue

# 编译根目录下的 dump.proto 和 NetMsgId.proto
Start-Process `
    -FilePath $Protoc `
    -ArgumentList @(
        "--cpp_out=$ScriptDir",
        "dump.proto",
        "NetMsgId.proto"
    ) `
    -WorkingDirectory $ScriptDir `
    -NoNewWindow `
    -Wait #>

function Compile-Protobuf-Directory {
    param(
        [string]$SourceDirName,
        [string]$OutputDirName
    )

    # 解析源目录和输出目录的绝对路径
    $SourceDir = (Resolve-Path (Join-Path $ScriptDir $SourceDirName)).Path
    $OutputDir = Join-Path $ScriptDir $OutputDirName

    # 若输出目录不存在则创建
    if (-not (Test-Path $OutputDir)) {
        New-Item -ItemType Directory -Path $OutputDir | Out-Null
    }
    $OutputDir = (Resolve-Path $OutputDir).Path

    # 清理输出目录中上次生成的文件
    Remove-Item `
        (Join-Path $OutputDir "*.pb.h"),
        (Join-Path $OutputDir "*.pb.cc") `
        -Force `
        -ErrorAction SilentlyContinue

    # 进入源目录
    Push-Location $SourceDir

    # 收集所有 .proto 文件（排除 google 目录下的文件）
    $ProtoFiles = Get-ChildItem -Recurse -Filter "*.proto" |
        Where-Object {
            $_.FullName -notmatch '[\\/]google[\\/]'
        } |
        ForEach-Object {
            $_.FullName.Substring($SourceDir.Length + 1)
        }

    # 构建 protoc 参数
    $Args = @(
        "--proto_path=.",
        "--cpp_out=$OutputDir"
    )

    $Args += $ProtoFiles

    # 执行编译
    Start-Process -FilePath $Protoc `
        -ArgumentList $Args `
        -WorkingDirectory $SourceDir `
        -NoNewWindow `
        -Wait

    # 返回脚本原始目录
    Pop-Location
}

# 编译 proto 文件夹
#Compile-Protobuf-Directory "proto" "proto_cpp"

# 编译 ServerProto 文件夹
Compile-Protobuf-Directory "ServerProto" "ServerProto_cpp"

Write-Host ""
Write-Host "[OK] Protobuf 代码生成完毕。" -ForegroundColor Green
Write-Host ""

Pause
exit 0
