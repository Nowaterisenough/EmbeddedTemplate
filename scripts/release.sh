#!/bin/bash

# ==============================================================================
# Release Script - 手动发布辅助工具
# ==============================================================================
#
# 功能：
# 1. 显示当前版本和即将发布的版本
# 2. 预览自上次发布以来的 commits
# 3. 确认后创建 tag 并推送
#
# 用法：
#   ./scripts/release.sh              # 交互式发布
#   ./scripts/release.sh v1.2.3       # 指定版本号发布
# ==============================================================================

set -e

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# ==============================================================================
# 辅助函数
# ==============================================================================

print_header() {
    echo -e "${BLUE}===================================================${NC}"
    echo -e "${BLUE}  $1${NC}"
    echo -e "${BLUE}===================================================${NC}"
}

print_info() {
    echo -e "${GREEN}✓${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}⚠${NC} $1"
}

print_error() {
    echo -e "${RED}✗${NC} $1"
}

# ==============================================================================
# 检查 Git 状态
# ==============================================================================

check_git_status() {
    # 检查是否在 Git 仓库中
    if ! git rev-parse --is-inside-work-tree > /dev/null 2>&1; then
        print_error "不在 Git 仓库中"
        exit 1
    fi

    # 检查是否在 main 分支
    CURRENT_BRANCH=$(git branch --show-current)
    if [ "$CURRENT_BRANCH" != "main" ]; then
        print_warning "当前分支: ${CURRENT_BRANCH}，建议在 main 分支发布"
        read -p "是否继续？(y/N) " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            exit 1
        fi
    fi

    # 检查工作区是否干净
    if ! git diff-index --quiet HEAD --; then
        print_warning "工作区有未提交的更改"
        git status --short
        read -p "是否继续？(y/N) " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            exit 1
        fi
    fi
}

# ==============================================================================
# 获取版本信息
# ==============================================================================

get_version_info() {
    # 获取最新 tag
    LAST_TAG=$(git describe --tags --abbrev=0 2>/dev/null || echo "")

    if [ -z "$LAST_TAG" ]; then
        LAST_TAG="(无)"
        COMMITS_COUNT=$(git rev-list --count HEAD)
    else
        COMMITS_COUNT=$(git rev-list --count ${LAST_TAG}..HEAD)
    fi

    # 解析版本号
    if [ "$LAST_TAG" != "(无)" ]; then
        LAST_VERSION=${LAST_TAG#v}
        LAST_MAJOR=$(echo $LAST_VERSION | cut -d. -f1)
        LAST_MINOR=$(echo $LAST_VERSION | cut -d. -f2)
        LAST_PATCH=$(echo $LAST_VERSION | cut -d. -f3)
    else
        LAST_MAJOR=0
        LAST_MINOR=0
        LAST_PATCH=0
    fi
}

# ==============================================================================
# 分析 commit 类型
# ==============================================================================

analyze_commits() {
    if [ "$LAST_TAG" = "(无)" ]; then
        COMMITS=$(git log --pretty=format:"%s")
    else
        COMMITS=$(git log ${LAST_TAG}..HEAD --pretty=format:"%s")
    fi

    HAS_FEAT=false
    HAS_FIX=false
    HAS_BREAKING=false

    while IFS= read -r commit; do
        if [[ "$commit" =~ ^feat(\(.+\))?!?: ]] || [[ "$commit" =~ BREAKING[[:space:]]CHANGE ]]; then
            HAS_BREAKING=true
            HAS_FEAT=true
        elif [[ "$commit" =~ ^feat(\(.+\))?: ]]; then
            HAS_FEAT=true
        elif [[ "$commit" =~ ^fix(\(.+\))?: ]]; then
            HAS_FIX=true
        fi
    done <<< "$COMMITS"
}

# ==============================================================================
# 建议版本号
# ==============================================================================

suggest_version() {
    # 从 workflow 读取 MAJOR 版本
    WORKFLOW_FILE=".github/workflows/auto-release.yml"
    if [ -f "$WORKFLOW_FILE" ]; then
        MAJOR=$(grep -m 1 "MAJOR=" "$WORKFLOW_FILE" | grep -oP 'MAJOR=\K\d+' || echo "$LAST_MAJOR")
    else
        MAJOR=$LAST_MAJOR
    fi

    # 根据 commit 类型建议版本
    if [ "$HAS_BREAKING" = true ]; then
        SUGGESTED_TYPE="BREAKING CHANGE (建议升级 MAJOR)"
        SUGGESTED_MAJOR=$((MAJOR + 1))
        SUGGESTED_MINOR=0
        SUGGESTED_PATCH=0
    elif [ "$HAS_FEAT" = true ]; then
        SUGGESTED_TYPE="新功能 (MINOR)"
        SUGGESTED_MAJOR=$MAJOR
        SUGGESTED_MINOR=$((LAST_MINOR + 1))
        SUGGESTED_PATCH=0
    elif [ "$HAS_FIX" = true ]; then
        SUGGESTED_TYPE="Bug修复 (PATCH)"
        SUGGESTED_MAJOR=$MAJOR
        SUGGESTED_MINOR=$LAST_MINOR
        SUGGESTED_PATCH=$((LAST_PATCH + 1))
    else
        SUGGESTED_TYPE="其他更改 (PATCH)"
        SUGGESTED_MAJOR=$MAJOR
        SUGGESTED_MINOR=$LAST_MINOR
        SUGGESTED_PATCH=$((LAST_PATCH + 1))
    fi

    SUGGESTED_VERSION="v${SUGGESTED_MAJOR}.${SUGGESTED_MINOR}.${SUGGESTED_PATCH}"
}

# ==============================================================================
# 显示发布预览
# ==============================================================================

show_preview() {
    print_header "发布预览"

    echo -e "${YELLOW}当前版本:${NC}     $LAST_TAG"
    echo -e "${GREEN}建议版本:${NC}     $SUGGESTED_VERSION"
    echo -e "${BLUE}版本类型:${NC}     $SUGGESTED_TYPE"
    echo -e "${BLUE}新增提交:${NC}     $COMMITS_COUNT 个"
    echo

    if [ $COMMITS_COUNT -gt 0 ]; then
        print_header "提交历史"
        if [ "$LAST_TAG" = "(无)" ]; then
            git log --pretty=format:"%C(yellow)%h%Creset %s %C(dim)(%cr)%Creset" --abbrev-commit | head -20
        else
            git log ${LAST_TAG}..HEAD --pretty=format:"%C(yellow)%h%Creset %s %C(dim)(%cr)%Creset" --abbrev-commit
        fi
        echo
    fi

    echo
}

# ==============================================================================
# 创建发布
# ==============================================================================

create_release() {
    VERSION=$1

    print_header "创建发布: ${VERSION}"

    # 检查 tag 是否已存在
    if git rev-parse "$VERSION" >/dev/null 2>&1; then
        print_error "Tag ${VERSION} 已存在"
        exit 1
    fi

    # 创建 tag
    print_info "创建 tag: ${VERSION}"
    git tag -a "$VERSION" -m "Release $VERSION"

    # 推送 tag
    print_info "推送到远程..."
    git push origin "$VERSION"

    print_header "✅ 发布成功"
    echo
    echo "Tag ${VERSION} 已推送到远程仓库"
    echo "GitHub Actions 将自动构建并创建 Release"
    echo
    echo "查看发布进度："
    echo "https://github.com/$(git config --get remote.origin.url | sed 's/.*github.com[:/]\(.*\)\.git/\1/')/actions"
    echo
}

# ==============================================================================
# 主流程
# ==============================================================================

main() {
    print_header "🚀 发布工具"
    echo

    # 检查 Git 状态
    check_git_status

    # 获取版本信息
    get_version_info

    # 分析 commits
    analyze_commits

    # 建议版本号
    suggest_version

    # 显示预览
    show_preview

    # 确定版本号
    if [ -n "$1" ]; then
        # 如果命令行指定了版本号
        NEW_VERSION="$1"
        # 确保版本号以 v 开头
        if [[ ! "$NEW_VERSION" =~ ^v ]]; then
            NEW_VERSION="v${NEW_VERSION}"
        fi
        print_info "使用指定版本: ${NEW_VERSION}"
    else
        # 交互式输入
        echo
        read -p "输入版本号 [默认: ${SUGGESTED_VERSION}]: " INPUT_VERSION

        if [ -z "$INPUT_VERSION" ]; then
            NEW_VERSION="$SUGGESTED_VERSION"
        else
            # 确保版本号以 v 开头
            if [[ ! "$INPUT_VERSION" =~ ^v ]]; then
                NEW_VERSION="v${INPUT_VERSION}"
            else
                NEW_VERSION="$INPUT_VERSION"
            fi
        fi
    fi

    # 验证版本号格式
    if [[ ! "$NEW_VERSION" =~ ^v[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
        print_error "版本号格式错误，应该是: vX.Y.Z"
        exit 1
    fi

    # 最终确认
    echo
    print_warning "即将创建并推送 tag: ${NEW_VERSION}"
    read -p "确认继续？(y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        print_info "已取消"
        exit 0
    fi

    # 创建发布
    create_release "$NEW_VERSION"
}

# ==============================================================================
# 脚本入口
# ==============================================================================

main "$@"
