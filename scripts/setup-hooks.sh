#!/bin/bash

# 设置 Git hooks

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
GITHOOKS_DIR="$PROJECT_ROOT/.githooks"
GIT_HOOKS_DIR="$PROJECT_ROOT/.git/hooks"

echo "正在设置 Git hooks..."

# 检查 .githooks 目录是否存在
if [ ! -d "$GITHOOKS_DIR" ]; then
    echo "错误: .githooks 目录不存在"
    exit 1
fi

# 复制所有 hooks 到 .git/hooks
for hook in "$GITHOOKS_DIR"/*; do
    if [ -f "$hook" ]; then
        hook_name=$(basename "$hook")
        echo "  安装: $hook_name"
        cp "$hook" "$GIT_HOOKS_DIR/$hook_name"
        chmod +x "$GIT_HOOKS_DIR/$hook_name"
    fi
done

echo "Git hooks 安装完成"
echo ""
echo "已安装的 hooks:"
ls -1 "$GIT_HOOKS_DIR" | grep -v '\.sample$'
