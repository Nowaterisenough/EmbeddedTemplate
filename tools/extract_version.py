#!/usr/bin/env python3
"""
extract_version.py - 从固件二进制文件中提取版本信息

支持的文件格式：
- .elf (通过 pyelftools)
- .bin (直接扫描魔法数字)
- .hex (Intel HEX 格式)

用法：
    python extract_version.py firmware.elf
    python extract_version.py firmware.bin
"""

import sys
import struct
import argparse
from pathlib import Path

# 版本信息结构定义（与 version.h 保持一致）
FW_VERSION_MAGIC = 0x46574556  # "FWVE"

def extract_from_bin(bin_path):
    """从 BIN 文件中提取版本信息"""
    with open(bin_path, 'rb') as f:
        data = f.read()

    # 搜索所有魔法数字匹配
    magic_bytes = struct.pack('<I', FW_VERSION_MAGIC)
    matches = []
    pos = 0
    while True:
        pos = data.find(magic_bytes, pos)
        if pos < 0:
            break
        matches.append(pos)
        pos += 1

    if not matches:
        print(f"[ERROR] Version info not found (magic: 0x{FW_VERSION_MAGIC:08X})")
        return None

    # 如果有多个匹配，尝试验证每一个
    valid_infos = []
    for offset in matches:
        try:
            info = parse_version_from_offset(data, offset)
            if info and is_version_info_valid(info):
                valid_infos.append((offset, info))
        except:
            continue

    if not valid_infos:
        print(f"[ERROR] Found {len(matches)} magic number(s), but none contain valid version data")
        return None

    if len(valid_infos) > 1:
        print(f"[WARNING] Found {len(valid_infos)} valid version info locations, using first one")

    offset, info = valid_infos[0]
    print(f"[OK] Found version info at offset 0x{offset:08X}")
    return info


def parse_version_from_offset(data, offset):
    """从指定偏移解析版本信息"""
    # uint32_t magic
    magic, = struct.unpack_from('<I', data, offset)
    offset += 4

    # uint8_t major, minor; uint16_t patch
    major, minor, patch = struct.unpack_from('<BBH', data, offset)
    offset += 4

    # uint32_t build_number
    build_number, = struct.unpack_from('<I', data, offset)
    offset += 4

    # char git_commit[41]
    git_commit = data[offset:offset+41].decode('ascii', errors='ignore').rstrip('\x00')
    offset += 41

    # char git_branch[32]
    git_branch = data[offset:offset+32].decode('ascii', errors='ignore').rstrip('\x00')
    offset += 32

    # uint8_t is_dirty; uint8_t reserved1[3]
    is_dirty, = struct.unpack_from('<B', data, offset)
    offset += 4

    # char build_date[12]
    build_date = data[offset:offset+12].decode('ascii', errors='ignore').rstrip('\x00')
    offset += 12

    # char build_time[9]
    build_time = data[offset:offset+9].decode('ascii', errors='ignore').rstrip('\x00')
    offset += 9

    # uint32_t build_timestamp
    build_timestamp, = struct.unpack_from('<I', data, offset)
    offset += 4

    # char compiler[32]
    compiler = data[offset:offset+32].decode('ascii', errors='ignore').rstrip('\x00')
    offset += 32

    # char board_name[32]
    board_name = data[offset:offset+32].decode('ascii', errors='ignore').rstrip('\x00')
    offset += 32

    # uint32_t crc32
    crc32, = struct.unpack_from('<I', data, offset)

    return {
        'magic': magic,
        'version': f"v{major}.{minor}.{patch}",
        'major': major,
        'minor': minor,
        'patch': patch,
        'build_number': build_number,
        'git_commit': git_commit,
        'git_branch': git_branch,
        'is_dirty': bool(is_dirty),
        'build_date': build_date,
        'build_time': build_time,
        'build_timestamp': build_timestamp,
        'compiler': compiler,
        'board_name': board_name,
        'crc32': crc32,
    }


def is_version_info_valid(info):
    """验证版本信息的合理性"""
    # 检查版本号范围（通常不会超过 255）
    if info['major'] > 255 or info['minor'] > 255 or info['patch'] > 65535:
        return False

    # 检查字符串字段是否包含可打印字符（至少有一些合理内容）
    # board_name 和 git_branch 至少要有一些字母数字字符
    if not any(c.isalnum() for c in info.get('board_name', '')):
        return False

    # git_commit 应该是十六进制字符串（至少前几个字符）
    git_commit = info.get('git_commit', '')
    if len(git_commit) > 0:
        # 检查前7个字符是否都是十六进制
        short_commit = git_commit[:7]
        if not all(c in '0123456789abcdef' for c in short_commit.lower()):
            return False

    # build_date 应该符合 YYYY-MM-DD 格式
    build_date = info.get('build_date', '')
    if len(build_date) == 10:
        parts = build_date.split('-')
        if len(parts) != 3:
            return False
        try:
            year = int(parts[0])
            if year < 2020 or year > 2100:  # 合理的年份范围
                return False
        except:
            return False

    return True

def extract_from_elf(elf_path):
    """从 ELF 文件中提取版本信息（通过 .version section）"""
    try:
        from elftools.elf.elffile import ELFFile
    except ImportError:
        print("[WARNING] pyelftools not installed, try using .bin format")
        print("    pip install pyelftools")
        return None

    with open(elf_path, 'rb') as f:
        elf = ELFFile(f)

        # 查找 .version section
        version_section = elf.get_section_by_name('.version')
        if not version_section:
            print("[ERROR] .version section not found in ELF file")
            return None

        data = version_section.data()
        print(f"[OK] Found version info in .version section (0x{version_section['sh_addr']:08X})")

        # 使用相同的解析逻辑
        return extract_from_bin_data(data)

def extract_from_bin_data(data):
    """从二进制数据中解析版本信息"""
    offset = 0
    try:
        # uint32_t magic
        magic, = struct.unpack_from('<I', data, offset)
        if magic != FW_VERSION_MAGIC:
            print(f"[ERROR] Magic number mismatch: 0x{magic:08X} != 0x{FW_VERSION_MAGIC:08X}")
            return None
        offset += 4

        # uint8_t major, minor; uint16_t patch
        major, minor, patch = struct.unpack_from('<BBH', data, offset)
        offset += 4

        # uint32_t build_number
        build_number, = struct.unpack_from('<I', data, offset)
        offset += 4

        # char git_commit[41]
        git_commit = data[offset:offset+41].decode('ascii', errors='ignore').rstrip('\x00')
        offset += 41

        # char git_branch[32]
        git_branch = data[offset:offset+32].decode('ascii', errors='ignore').rstrip('\x00')
        offset += 32

        # uint8_t is_dirty; uint8_t reserved1[3]
        is_dirty, = struct.unpack_from('<B', data, offset)
        offset += 4

        # char build_date[12]
        build_date = data[offset:offset+12].decode('ascii', errors='ignore').rstrip('\x00')
        offset += 12

        # char build_time[9]
        build_time = data[offset:offset+9].decode('ascii', errors='ignore').rstrip('\x00')
        offset += 9

        # uint32_t build_timestamp
        build_timestamp, = struct.unpack_from('<I', data, offset)
        offset += 4

        # char compiler[32]
        compiler = data[offset:offset+32].decode('ascii', errors='ignore').rstrip('\x00')
        offset += 32

        # char board_name[32]
        board_name = data[offset:offset+32].decode('ascii', errors='ignore').rstrip('\x00')
        offset += 32

        # uint32_t crc32
        crc32, = struct.unpack_from('<I', data, offset)

        return {
            'magic': magic,
            'version': f"v{major}.{minor}.{patch}",
            'major': major,
            'minor': minor,
            'patch': patch,
            'build_number': build_number,
            'git_commit': git_commit,
            'git_branch': git_branch,
            'is_dirty': bool(is_dirty),
            'build_date': build_date,
            'build_time': build_time,
            'build_timestamp': build_timestamp,
            'compiler': compiler,
            'board_name': board_name,
            'crc32': crc32,
        }

    except Exception as e:
        print(f"[ERROR] Failed to parse version info: {e}")
        return None

def print_version_info(info):
    """Print version information"""
    if not info:
        return

    print("\n" + "="*80)
    print("  Firmware Version Information")
    print("="*80)
    print(f"  Version:       {info['version']}+build.{info['build_number']}.{info['git_commit'][:7]}")
    print(f"  Board:         {info['board_name']}")
    print(f"  Build Date:    {info['build_date']} {info['build_time']}")
    print(f"  Git Branch:    {info['git_branch']}")
    print(f"  Git Commit:    {info['git_commit']}")
    print(f"  Dirty:         {'Yes' if info['is_dirty'] else 'No'}")
    print(f"  Compiler:      {info['compiler']}")
    print(f"  Magic:         0x{info['magic']:08X}")
    print(f"  CRC32:         0x{info['crc32']:08X}")
    print("="*80 + "\n")

def main():
    parser = argparse.ArgumentParser(
        description="从固件二进制文件中提取版本信息",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  python extract_version.py firmware.elf
  python extract_version.py firmware.bin
  python extract_version.py firmware.hex
        """
    )
    parser.add_argument('firmware', help='固件文件路径 (.elf/.bin/.hex)')
    parser.add_argument('-j', '--json', action='store_true', help='以 JSON 格式输出')

    args = parser.parse_args()

    firmware_path = Path(args.firmware)
    if not firmware_path.exists():
        print(f"[ERROR] File not found: {firmware_path}")
        return 1

    # 根据文件扩展名选择提取方法
    suffix = firmware_path.suffix.lower()

    if suffix == '.elf':
        info = extract_from_elf(firmware_path)
    elif suffix == '.bin':
        info = extract_from_bin(firmware_path)
    elif suffix == '.hex':
        print("[ERROR] HEX format not supported, please use .elf or .bin")
        return 1
    else:
        print(f"[ERROR] Unsupported file format: {suffix}")
        return 1

    if not info:
        return 1

    if args.json:
        import json
        print(json.dumps(info, indent=2, ensure_ascii=False))
    else:
        print_version_info(info)

    return 0

if __name__ == '__main__':
    sys.exit(main())
