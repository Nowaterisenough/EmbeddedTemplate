# ==============================================================================
# Release Script - 手动发布辅助工具 (PowerShell 版本)
# ==============================================================================
#
# 功能：
# 1. 显示当前版本和即将发布的版本
# 2. 预览自上次发布以来的 commits
# 3. 确认后创建 tag 并推送
#
# 用法：
#   .\scripts\release.ps1              # 交互式发布
#   .\scripts\release.ps1 v1.2.3       # 指定版本号发布
# ==============================================================================

param(
    [string]$Version = ""
)

$ErrorActionPreference = "Stop"

# ==============================================================================
# 辅助函数
# ==============================================================================

function Write-Header {
    param([string]$Message)
    Write-Host ""
    Write-Host "===================================================" -ForegroundColor Blue
    Write-Host "  $Message" -ForegroundColor Blue
    Write-Host "===================================================" -ForegroundColor Blue
}

function Write-Info {
    param([string]$Message)
    Write-Host "✓ $Message" -ForegroundColor Green
}

function Write-Warning {
    param([string]$Message)
    Write-Host "⚠ $Message" -ForegroundColor Yellow
}

function Write-ErrorMsg {
    param([string]$Message)
    Write-Host "✗ $Message" -ForegroundColor Red
}

# ==============================================================================
# 检查 Git 状态
# ==============================================================================

function Test-GitStatus {
    # 检查是否在 Git 仓库中
    try {
        git rev-parse --is-inside-work-tree | Out-Null
    } catch {
        Write-ErrorMsg "不在 Git 仓库中"
        exit 1
    }

    # 检查是否在 main 分支
    $currentBranch = git branch --show-current
    if ($currentBranch -ne "main") {
        Write-Warning "当前分支: $currentBranch，建议在 main 分支发布"
        $response = Read-Host "是否继续？(y/N)"
        if ($response -ne "y" -and $response -ne "Y") {
            exit 1
        }
    }

    # 检查工作区是否干净
    $status = git status --porcelain
    if ($status) {
        Write-Warning "工作区有未提交的更改"
        git status --short
        $response = Read-Host "是否继续？(y/N)"
        if ($response -ne "y" -and $response -ne "Y") {
            exit 1
        }
    }
}

# ==============================================================================
# 获取版本信息
# ==============================================================================

function Get-VersionInfo {
    # 获取最新 tag
    $script:lastTag = git describe --tags --abbrev=0 2>$null

    if (-not $lastTag) {
        $script:lastTag = "(无)"
        $script:commitsCount = git rev-list --count HEAD
    } else {
        $script:commitsCount = git rev-list --count "$lastTag..HEAD"
    }

    # 解析版本号
    if ($lastTag -ne "(无)") {
        $lastVersion = $lastTag.TrimStart('v')
        $parts = $lastVersion.Split('.')
        $script:lastMajor = [int]$parts[0]
        $script:lastMinor = [int]$parts[1]
        $script:lastPatch = [int]$parts[2]
    } else {
        $script:lastMajor = 0
        $script:lastMinor = 0
        $script:lastPatch = 0
    }
}

# ==============================================================================
# 分析 commit 类型
# ==============================================================================

function Get-CommitAnalysis {
    if ($lastTag -eq "(无)") {
        $commits = git log --pretty=format:"%s"
    } else {
        $commits = git log "$lastTag..HEAD" --pretty=format:"%s"
    }

    $script:hasFeat = $false
    $script:hasFix = $false
    $script:hasBreaking = $false

    foreach ($commit in $commits) {
        if ($commit -match '^feat(\(.+\))?!?:' -or $commit -match 'BREAKING\s+CHANGE') {
            $script:hasBreaking = $true
            $script:hasFeat = $true
        } elseif ($commit -match '^feat(\(.+\))?:') {
            $script:hasFeat = $true
        } elseif ($commit -match '^fix(\(.+\))?:') {
            $script:hasFix = $true
        }
    }
}

# ==============================================================================
# 建议版本号
# ==============================================================================

function Get-SuggestedVersion {
    # 从 workflow 读取 MAJOR 版本
    $workflowFile = ".github\workflows\auto-release.yml"
    if (Test-Path $workflowFile) {
        $content = Get-Content $workflowFile -Raw
        if ($content -match 'MAJOR=(\d+)') {
            $major = [int]$Matches[1]
        } else {
            $major = $lastMajor
        }
    } else {
        $major = $lastMajor
    }

    # 根据 commit 类型建议版本
    if ($hasBreaking) {
        $script:suggestedType = "BREAKING CHANGE (建议升级 MAJOR)"
        $script:suggestedMajor = $major + 1
        $script:suggestedMinor = 0
        $script:suggestedPatch = 0
    } elseif ($hasFeat) {
        $script:suggestedType = "新功能 (MINOR)"
        $script:suggestedMajor = $major
        $script:suggestedMinor = $lastMinor + 1
        $script:suggestedPatch = 0
    } elseif ($hasFix) {
        $script:suggestedType = "Bug修复 (PATCH)"
        $script:suggestedMajor = $major
        $script:suggestedMinor = $lastMinor
        $script:suggestedPatch = $lastPatch + 1
    } else {
        $script:suggestedType = "其他更改 (PATCH)"
        $script:suggestedMajor = $major
        $script:suggestedMinor = $lastMinor
        $script:suggestedPatch = $lastPatch + 1
    }

    $script:suggestedVersion = "v$suggestedMajor.$suggestedMinor.$suggestedPatch"
}

# ==============================================================================
# 显示发布预览
# ==============================================================================

function Show-Preview {
    Write-Header "发布预览"

    Write-Host "当前版本:     " -NoNewline -ForegroundColor Yellow
    Write-Host $lastTag
    Write-Host "建议版本:     " -NoNewline -ForegroundColor Green
    Write-Host $suggestedVersion
    Write-Host "版本类型:     " -NoNewline -ForegroundColor Blue
    Write-Host $suggestedType
    Write-Host "新增提交:     " -NoNewline -ForegroundColor Blue
    Write-Host "$commitsCount 个"
    Write-Host ""

    if ($commitsCount -gt 0) {
        Write-Header "提交历史"
        if ($lastTag -eq "(无)") {
            git log --pretty=format:"%C(yellow)%h%Creset %s %C(dim)(%cr)%Creset" --abbrev-commit | Select-Object -First 20
        } else {
            git log "$lastTag..HEAD" --pretty=format:"%C(yellow)%h%Creset %s %C(dim)(%cr)%Creset" --abbrev-commit
        }
        Write-Host ""
    }

    Write-Host ""
}

# ==============================================================================
# 创建发布
# ==============================================================================

function New-Release {
    param([string]$NewVersion)

    Write-Header "创建发布: $NewVersion"

    # 检查 tag 是否已存在
    $existingTag = git rev-parse $NewVersion 2>$null
    if ($existingTag) {
        Write-ErrorMsg "Tag $NewVersion 已存在"
        exit 1
    }

    # 创建 tag
    Write-Info "创建 tag: $NewVersion"
    git tag -a $NewVersion -m "Release $NewVersion"

    # 推送 tag
    Write-Info "推送到远程..."
    git push origin $NewVersion

    Write-Header "✅ 发布成功"
    Write-Host ""
    Write-Host "Tag $NewVersion 已推送到远程仓库"
    Write-Host "GitHub Actions 将自动构建并创建 Release"
    Write-Host ""

    # 获取仓库 URL
    $remoteUrl = git config --get remote.origin.url
    if ($remoteUrl -match 'github\.com[:/](.+?)(?:\.git)?$') {
        $repoPath = $Matches[1]
        Write-Host "查看发布进度："
        Write-Host "https://github.com/$repoPath/actions"
    }
    Write-Host ""
}

# ==============================================================================
# 主流程
# ==============================================================================

Write-Header "🚀 发布工具"
Write-Host ""

# 检查 Git 状态
Test-GitStatus

# 获取版本信息
Get-VersionInfo

# 分析 commits
Get-CommitAnalysis

# 建议版本号
Get-SuggestedVersion

# 显示预览
Show-Preview

# 确定版本号
if ($Version) {
    # 如果命令行指定了版本号
    $newVersion = $Version
    # 确保版本号以 v 开头
    if ($newVersion -notmatch '^v') {
        $newVersion = "v$newVersion"
    }
    Write-Info "使用指定版本: $newVersion"
} else {
    # 交互式输入
    Write-Host ""
    $inputVersion = Read-Host "输入版本号 [默认: $suggestedVersion]"

    if (-not $inputVersion) {
        $newVersion = $suggestedVersion
    } else {
        # 确保版本号以 v 开头
        if ($inputVersion -notmatch '^v') {
            $newVersion = "v$inputVersion"
        } else {
            $newVersion = $inputVersion
        }
    }
}

# 验证版本号格式
if ($newVersion -notmatch '^v\d+\.\d+\.\d+$') {
    Write-ErrorMsg "版本号格式错误，应该是: vX.Y.Z"
    exit 1
}

# 最终确认
Write-Host ""
Write-Warning "即将创建并推送 tag: $newVersion"
$confirm = Read-Host "确认继续？(y/N)"
if ($confirm -ne "y" -and $confirm -ne "Y") {
    Write-Info "已取消"
    exit 0
}

# 创建发布
New-Release $newVersion
