# LevelDB 检测与安装指南

> **生成时间**: 2026-07-15
> **适用版本**: mcgen v1.0+
> **目标平台**: Ubuntu/Debian, macOS, Windows, ARM64

---

## 目录

1. [背景知识](#背景知识)
2. [快速检测](#快速检测)
3. [Ubuntu/Debian 安装](#ubuntudebian-安装)
4. [macOS 安装](#macos-安装)
5. [Windows 安装](#windows-安装)
6. [ARM64 设备安装](#arm64-设备安装)
7. [从源码构建 leveldb-mcpe](#从源码构建-leveldb-mcpe)
8. [CMake 集成检测](#cmake-集成检测)
9. [故障排除](#故障排除)

---

## 背景知识

### 为什么需要特殊版本的 LevelDB？

Minecraft Bedrock Edition 使用了一个修改版的 Google LevelDB，主要区别包括：citeweb_search:3#5web_search:3#7

| 特性 | Google 原版 LevelDB | Mojang leveldb-mcpe |
|------|---------------------|---------------------|
| 默认压缩 | Snappy | **ZlibRaw** |
| Windows 支持 | 有限 | 完整 |
| Boost 依赖 | 无 | 需要 (旧版本) |
| Bedrock 兼容性 | ❌ 不兼容 | ✅ 完全兼容 |

**关键区别**: Bedrock Edition 使用 ZlibRaw 压缩格式存储世界数据。如果使用原版 LevelDB（仅支持 Snappy），生成的世界文件将无法被 Minecraft 正确读取。citeweb_search:5#9

### 仓库选择

- **官方 Mojang 分支**: `https://github.com/Mojang/leveldb` (旧版，使用 Makefile)
- **Amulet-Team 维护版**: `https://github.com/Amulet-Team/leveldb-mcpe` (CMake 支持，推荐)citeweb_search:3#6
- **bedrock-viz 版**: `https://github.com/bedrock-viz/leveldb-mcpe` (vcpkg 支持)citeweb_search:5#4

---

## 快速检测

### 运行检测脚本

```bash
# 在项目根目录执行
./scripts/check-leveldb.sh
```

### 手动检测

```bash
# 检查系统是否已安装 LevelDB
pkg-config --exists leveldb && echo "✅ LevelDB found" || echo "❌ LevelDB not found"

# 检查头文件
ls /usr/local/include/leveldb/db.h 2>/dev/null || ls /usr/include/leveldb/db.h 2>/dev/null || echo "❌ Header not found"

# 检查库文件
ldconfig -p | grep libleveldb || find /usr -name "libleveldb*" 2>/dev/null
```

---

## Ubuntu/Debian 安装

### 方法 A: 快速安装（使用 apt）⚠️ 不推荐用于生产

```bash
sudo apt update
sudo apt install -y libleveldb-dev libleveldb1d
```

**注意**: Ubuntu apt 提供的 `libleveldb-dev` 是 Google 原版，**不支持 ZlibRaw 压缩**。这会导致生成的 Bedrock 世界无法被 Minecraft 读取。仅用于开发测试或作为 Java 版输出。citeweb_search:5#0

### 方法 B: 从源码构建 leveldb-mcpe（推荐）

#### 步骤 1: 安装依赖

```bash
sudo apt update
sudo apt install -y build-essential cmake git libsnappy-dev zlib1g-dev     libboost-all-dev  # 旧版 Mojang fork 需要
```

#### 步骤 2: 克隆并构建

**选项 1: 使用 Amulet-Team 的 CMake 版本（推荐）**

```bash
git clone https://github.com/Amulet-Team/leveldb-mcpe.git
cd leveldb-mcpe
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DLEVELDB_BUILD_TESTS=OFF ..
make -j$(nproc)
sudo make install
sudo ldconfig
```

**选项 2: 使用原版 Mojang fork（需要补丁）**

```bash
git clone https://github.com/Mojang/leveldb-mcpe.git
cd leveldb-mcpe

# 应用已知修复（snappy 头文件路径问题）
sed -i 's|<snappy/snappy.h>|<snappy.h>|g' db/snappy_compressor.cc

# 构建（使用 Makefile）
make

# 手动安装
sudo cp -r include/leveldb /usr/local/include/
sudo cp libleveldb.a /usr/local/lib/
sudo ldconfig
```

**已知问题**: Mojang 原版 fork 需要两个修复才能编译：citeweb_search:5#9
1. `db/snappy_compressor.cc`: 将 `#include <snappy/snappy.h>` 改为 `#include <snappy.h>`
2. `table/table_test.cc`: 注释掉 `port::Snappy_Compress` 和 `leveldb::SnappyCompressor` 相关代码（测试文件，不影响库功能）

---

## macOS 安装

### 使用 Homebrew（原版 LevelDB）

```bash
brew install leveldb
```

**注意**: 这是 Google 原版，不支持 ZlibRaw。

### 从源码构建 leveldb-mcpe

```bash
# 安装依赖
brew install cmake snappy zlib boost

# 构建（同 Ubuntu 步骤）
git clone https://github.com/Amulet-Team/leveldb-mcpe.git
cd leveldb-mcpe
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(sysctl -n hw.ncpu)
sudo make install
```

---

## Windows 安装

### 使用 vcpkg（推荐）

```powershell
# 安装 vcpkg 后
vcpkg install leveldb-mcpe

# 或在 CMake 中集成
# -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]\scriptsuildsystemscpkg.cmake
```

### 使用 Visual Studio

参考 bedrock-viz 的构建说明：citeweb_search:5#4

```powershell
git clone https://github.com/bedrock-viz/leveldb-mcpe.git
cd leveldb-mcpe
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:cpkg\scriptsuildsystemscpkg.cmake
cmake --build . --config Release
```

---

## ARM64 设备安装

### Raspberry Pi / Apple Silicon

```bash
# 安装依赖（与 x64 相同）
sudo apt install -y build-essential cmake git libsnappy-dev zlib1g-dev

# 构建（CMake 会自动检测架构）
git clone https://github.com/Amulet-Team/leveldb-mcpe.git
cd leveldb-mcpe
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DLEVELDB_BUILD_TESTS=OFF ..
make -j$(nproc)
sudo make install
sudo ldconfig
```

**注意**: ARM64 构建不需要特殊配置，CMake 会自动处理交叉编译。

---

## 从源码构建 leveldb-mcpe

### 完整构建脚本

创建 `scripts/build-leveldb-mcpe.sh`:

```bash
#!/bin/bash
set -e

LEVELDB_VERSION="master"
BUILD_TYPE="Release"
INSTALL_PREFIX="/usr/local"

echo "=== Building leveldb-mcpe for Bedrock Edition support ==="

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
    CORES=$(nproc)
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
    CORES=$(sysctl -n hw.ncpu)
else
    OS="unknown"
    CORES=4
fi

echo "Detected OS: $OS, using $CORES cores"

# Install dependencies
echo "=== Installing dependencies ==="
if [[ "$OS" == "linux" ]]; then
    if command -v apt &> /dev/null; then
        sudo apt update
        sudo apt install -y build-essential cmake git libsnappy-dev zlib1g-dev
    elif command -v yum &> /dev/null; then
        sudo yum install -y gcc-c++ cmake git snappy-devel zlib-devel
    elif command -v pacman &> /dev/null; then
        sudo pacman -S --needed base-devel cmake git snappy zlib
    fi
elif [[ "$OS" == "macos" ]]; then
    brew install cmake snappy zlib
fi

# Clone
echo "=== Cloning leveldb-mcpe ==="
if [ ! -d "leveldb-mcpe" ]; then
    git clone https://github.com/Amulet-Team/leveldb-mcpe.git
fi
cd leveldb-mcpe
git pull

# Build
echo "=== Building ==="
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE       -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX       -DLEVELDB_BUILD_TESTS=OFF       -DLEVELDB_BUILD_BENCHMARKS=OFF       ..
make -j$CORES

# Install
echo "=== Installing ==="
sudo make install
sudo ldconfig 2>/dev/null || true

echo "=== leveldb-mcpe installed successfully ==="
echo "Library: $INSTALL_PREFIX/lib"
echo "Headers: $INSTALL_PREFIX/include/leveldb"
```

---

## CMake 集成检测

### mcgen 的 CMake 检测逻辑

```cmake
# CMakeLists.txt 中的 LevelDB 检测部分

# 1. 尝试查找系统 LevelDB
find_library(LEVELDB_LIBRARY 
    NAMES leveldb 
    PATHS /usr/local/lib /usr/lib /opt/local/lib
    DOC "LevelDB library"
)

find_path(LEVELDB_INCLUDE_DIR 
    NAMES leveldb/db.h 
    PATHS /usr/local/include /usr/include /opt/local/include
    DOC "LevelDB headers"
)

# 2. 检测是否支持 ZlibRaw（检查头文件中的 ZlibCompressor）
set(MCGEN_HAS_ZLIBRAW OFF)
if(LEVELDB_INCLUDE_DIR AND LEVELDB_LIBRARY)
    # 检查头文件中是否有 ZlibCompressor 类
    if(EXISTS "${LEVELDB_INCLUDE_DIR}/leveldb/zlib_compressor.h")
        set(MCGEN_HAS_ZLIBRAW ON)
        message(STATUS "✅ Found leveldb-mcpe with ZlibRaw support")
    else()
        message(WARNING "⚠️  Found LevelDB but no ZlibRaw support (likely Google原版). Bedrock output may be incompatible.")
    endif()

    set(MCGEN_LEVELDB_BACKEND "system")
    target_compile_definitions(mcgen_bedrock PRIVATE MCGEN_HAS_LEVELDB=1)
    target_link_libraries(mcgen_bedrock PRIVATE ${LEVELDB_LIBRARY})
    target_include_directories(mcgen_bedrock PRIVATE ${LEVELDB_INCLUDE_DIR})
else()
    message(STATUS "ℹ️  LevelDB not found, using memory stub backend")
    set(MCGEN_LEVELDB_BACKEND "stub")
    target_compile_definitions(mcgen_bedrock PRIVATE MCGEN_HAS_LEVELDB=0)
endif()

# 3. 报告状态
message(STATUS "LevelDB backend: ${MCGEN_LEVELDB_BACKEND}")
message(STATUS "ZlibRaw support: ${MCGEN_HAS_ZLIBRAW}")
```

### 编译时输出示例

```
-- Found LevelDB: /usr/local/lib/libleveldb.a
-- ✅ Found leveldb-mcpe with ZlibRaw support
-- LevelDB backend: system
-- ZlibRaw support: ON
```

或

```
-- Could NOT find LevelDB (missing: LEVELDB_LIBRARY LEVELDB_INCLUDE_DIR)
-- ℹ️  LevelDB not found, using memory stub backend
-- LevelDB backend: stub
-- ZlibRaw support: OFF
```

---

## 故障排除

### 问题 1: `undefined reference to leveldb::ZlibCompressor`

**原因**: 链接的是原版 LevelDB，缺少 ZlibRaw 支持。

**解决**: 重新安装 leveldb-mcpe（见上文"从源码构建"部分）。

### 问题 2: `fatal error: snappy/snappy.h: No such file`

**原因**: Mojang 原版 fork 的头文件路径假设与系统不一致。

**解决**:
```bash
# 方法 1: 创建符号链接
sudo ln -s /usr/include /usr/include/snappy

# 方法 2: 修改源码
sed -i 's|<snappy/snappy.h>|<snappy.h>|g' db/snappy_compressor.cc
```

### 问题 3: `cannot find -lleveldb`

**原因**: 库文件路径未加入链接器搜索路径。

**解决**:
```bash
sudo ldconfig
# 或手动指定
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

### 问题 4: ARM64 构建失败

**原因**: 可能需要指定工具链。

**解决**:
```bash
# 使用 mcgen 提供的工具链文件
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/arm64-toolchain.cmake ..
```

### 问题 5: 生成的世界无法被 Minecraft 打开

**原因**: 使用了原版 LevelDB（Snappy 压缩），而非 leveldb-mcpe（ZlibRaw 压缩）。

**验证**:
```bash
# 检查生成的 db 文件夹中的文件头
xxd world/db/000001.ldb | head -5
# 如果看到 snappy 相关标记，说明压缩格式错误
```

**解决**: 确保 `MCGEN_HAS_ZLIBRAW=ON`，并重新构建 mcgen。

---

## 附录: 一键安装脚本

### `scripts/install-deps.sh` 中的 LevelDB 部分

```bash
#!/bin/bash
# 已包含在 mcgen 的 install-deps.sh 中

install_leveldb_mcpe() {
    echo "Installing leveldb-mcpe for Bedrock Edition support..."

    # 检查是否已安装且支持 ZlibRaw
    if [ -f "/usr/local/include/leveldb/zlib_compressor.h" ] ||        [ -f "/usr/include/leveldb/zlib_compressor.h" ]; then
        echo "✅ leveldb-mcpe already installed"
        return 0
    fi

    # 检查是否有原版 LevelDB
    if pkg-config --exists leveldb 2>/dev/null ||        ldconfig -p | grep -q libleveldb; then
        echo "⚠️  Found system LevelDB but may not support ZlibRaw"
        echo "   Consider rebuilding from source for full Bedrock compatibility"
    fi

    # 询问是否从源码构建
    read -p "Build leveldb-mcpe from source? [Y/n]: " response
    if [[ ! "$response" =~ ^[Nn]$ ]]; then
        ./scripts/build-leveldb-mcpe.sh
    fi
}

install_leveldb_mcpe
```

---

## 参考资源

- [Mojang/leveldb-mcpe](https://github.com/Mojang/leveldb-mcpe) - 官方 Mojang fork
- [Amulet-Team/leveldb-mcpe](https://github.com/Amulet-Team/leveldb-mcpe) - CMake 维护版
- [bedrock-viz/leveldb-mcpe](https://github.com/bedrock-viz/leveldb-mcpe) - vcpkg 支持版
- [Minecraft Wiki - Bedrock Level Format](https://minecraft.wiki/w/Bedrock_Edition_level_format) - 技术规范

---

*文档版本: 1.0 | 最后更新: 2026-07-15*
