#!/bin/bash

# 项目构建脚本
# 功能：自动检查、下载、编译第三方依赖，然后构建项目

set -e  # 出错时终止脚本

# 配置项
PROJECT_DIR=$(pwd)
THIRD_PARTY_DIR="${PROJECT_DIR}/third_party"
BUILD_DIR="${PROJECT_DIR}/build"
LOG_FILE="${PROJECT_DIR}/build.log"

# 第三方库配置（可扩展）
declare -A DEPS
DEPS["rapidjson"]="https://github.com/Tencent/rapidjson.git master"
DEPS["faiss"]="https://github.com/facebookresearch/faiss.git v1.7.4"
DEPS["spdlog"]="git@github.com:gabime/spdlog.git"
DEPS["cpp-httplib"]="https://github.com/yhirose/cpp-httplib.git v0.12.5"

# 日志函数
log() {
    echo -e "\033[1;32m[$(date '+%Y-%m-%d %H:%M:%S')] $1\033[0m"
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" >> "$LOG_FILE"
}

err() {
    echo -e "\033[1;31m[$(date '+%Y-%m-%d %H:%M:%S')] ERROR: $1\033[0m" >&2
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] ERROR: $1" >> "$LOG_FILE"
    exit 1
}

# 检查命令是否存在
check_command() {
    command -v "$1" >/dev/null 2>&1 || err "需要安装 $1"
}

# 检查并安装依赖
check_dependency() {
    local dep_name=$1
    local dep_repo=$2
    local dep_branch=$3
    local dep_dir="${THIRD_PARTY_DIR}/${dep_name}"
    
    log "检查依赖: ${dep_name}"
    
    # 根据依赖类型执行不同的检查逻辑
    case $dep_name in
        "rapidjson")
            # Header-only库，检查头文件是否存在
            if [ ! -f "/usr/include/rapidjson/document.h" ] && [ ! -f "${dep_dir}/include/rapidjson/document.h" ]; then
                install_rapidjson "$dep_repo" "$dep_branch" "$dep_dir"
            else
                log "rapidjson 已安装"
            fi
            ;;
        "faiss")
            # 库文件检查
            if [ ! -f "/usr/local/lib/libfaiss.so" ] && [ ! -f "${dep_dir}/lib/libfaiss.so" ]; then
                install_faiss "$dep_repo" "$dep_branch" "$dep_dir"
            else
                log "faiss 已安装"
            fi
            ;;
        "spdlog")
            # 库文件检查
            if [ ! -f "/usr/local/lib/libspdlog.so" ] && [ ! -f "${dep_dir}/lib/libspdlog.so" ]; then
                install_spdlog "$dep_repo" "$dep_branch" "$dep_dir"
            else
                log "spdlog 已安装"
            fi
            ;;
        "cpp-httplib")
            # Header-only库，检查头文件是否存在
            if [ ! -f "/usr/include/httplib.h" ] && [ ! -f "${dep_dir}/httplib.h" ]; then
                install_cpp_httplib "$dep_repo" "$dep_branch" "$dep_dir"
            else
                log "cpp-httplib 已安装"
            fi
            ;;
        *)
            err "未知依赖: ${dep_name}"
            ;;
    esac
}

# 安装RapidJSON
install_rapidjson() {
    local repo=$1
    local branch=$2
    local dir=$3
    
    log "安装RapidJSON..."
    mkdir -p "$THIRD_PARTY_DIR"
    cd "$THIRD_PARTY_DIR"
    
    if [ ! -d "$dir" ]; then
        git clone --branch "$branch" "$repo" "$dir"
    fi
    
    log "RapidJSON 安装完成"
}

# 安装Faiss
install_faiss() {
    local repo=$1
    local branch=$2
    local dir=$3
    
    log "安装 Faiss..."
    mkdir -p "$THIRD_PARTY_DIR"
    cd "$THIRD_PARTY_DIR"
    
    if [ ! -d "$dir" ]; then
        git clone --branch "$branch" "$repo" "$dir"
    fi
    
    cd "$dir"
    mkdir -p build && cd build
    
    # 修正的 CMake 命令（确保每行以 \ 结尾，参数间有空格）
    cmake .. \
        -DFAISS_ENABLE_GPU=OFF \
        -DFAISS_ENABLE_PYTHON=OFF \
        -DCMAKE_INSTALL_PREFIX="${THIRD_PARTY_DIR}/faiss/install" \
        -DBUILD_SHARED_LIBS=ON
    
    make -j$(nproc)
    make install
    
    log "Faiss 安装完成"
}

# 安装spdlog
install_spdlog() {
    local repo=$1
    local branch=$2
    local dir=$3
    
    log "安装spdlog..."
    mkdir -p "$THIRD_PARTY_DIR"
    cd "$THIRD_PARTY_DIR"
    
    if [ ! -d "$dir" ]; then
        git clone --branch "$branch" "$repo" "$dir"
    fi
    
    cd "$dir"
    mkdir -p build && cd build
    cmake .. -DCMAKE_INSTALL_PREFIX="$dir/install"
    make -j$(nproc)
    make install
    
    log "spdlog 安装完成"
}

# 安装cpp-httplib
install_cpp_httplib() {
    local repo=$1
    local branch=$2
    local dir=$3
    
    log "安装cpp-httplib..."
    mkdir -p "$THIRD_PARTY_DIR"
    cd "$THIRD_PARTY_DIR"
    
    if [ ! -d "$dir" ]; then
        git clone --branch "$branch" "$repo" "$dir"
    fi
    
    log "cpp-httplib 安装完成"
}

# 编译项目
build_project() {
    log "开始编译项目..."
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # 配置CMake
    cmake "$PROJECT_DIR" \
        -DCMAKE_BUILD_TYPE=Release \
        -DTHIRD_PARTY_DIR="${THIRD_PARTY_DIR}"
    
    # 编译项目
    make -j$(nproc)
    
    log "项目编译完成"
}

# 主函数
main() {
    log "开始构建项目..."
    echo "" > "$LOG_FILE"  # 清空日志文件
    
    # 检查必要的命令
    check_command "git"
    check_command "cmake"
    check_command "make"
    
    # 检查并安装所有依赖
    for dep in "${!DEPS[@]}"; do
        IFS=' ' read -r -a parts <<< "${DEPS[$dep]}"
        check_dependency "$dep" "${parts[0]}" "${parts[1]}"
    done
    
    # 编译项目
    build_project
    
    log "项目构建成功！"
}

main