import json
import re
from pathlib import Path


_ID_ENTRY_RE = re.compile(r"([A-Za-z_]\w*)\s*=\s*(-?\d+)", re.ASCII)
_MSG_NAME_ENTRY_RE = re.compile(
    r'\[\s*(-?\d+)\s*\]\s*=\s*("(?:\\.|[^"\\])*")', re.ASCII
)


def _extract_block(text: str, table_name: str) -> str:
    match = re.search(
        rf"NetMsgId\.{re.escape(table_name)}\s*=\s*\{{([\s\S]*?)\}}", text
    )
    if not match:
        raise ValueError(f"未找到 NetMsgId.{table_name} 表")
    return match.group(1)


def parse_lua(text: str):
    id_block = _extract_block(text, "Id")
    msg_name_block = _extract_block(text, "MsgName")

    id_entries = [(name, int(value)) for name, value in _ID_ENTRY_RE.findall(id_block)]
    msg_name_entries = []
    for value, literal in _MSG_NAME_ENTRY_RE.findall(msg_name_block):
        try:
            name = json.loads(literal)
        except json.JSONDecodeError as exc:
            raise ValueError(f"MsgName 中存在无效字符串：{literal}") from exc
        msg_name_entries.append((int(value), name))

    if not id_entries:
        raise ValueError("NetMsgId.Id 表为空")
    if not msg_name_entries:
        raise ValueError("NetMsgId.MsgName 表为空")

    id_names = [name for name, _ in id_entries]
    id_values = [value for _, value in id_entries]
    msg_name_values = [value for value, _ in msg_name_entries]

    if len(id_names) != len(set(id_names)):
        raise ValueError("NetMsgId.Id 中存在重复消息名称")
    if len(id_values) != len(set(id_values)):
        raise ValueError("NetMsgId.Id 中存在重复消息 ID")
    if len(msg_name_values) != len(set(msg_name_values)):
        raise ValueError("NetMsgId.MsgName 中存在重复消息 ID")

    for name in id_names:
        if not re.fullmatch(r"[A-Za-z_]\w*", name, re.ASCII):
            raise ValueError(f"消息名称不是合法 C++ 标识符：{name}")

    id_set = set(id_values)
    msg_name_set = set(msg_name_values)
    if id_set != msg_name_set:
        missing = sorted(id_set - msg_name_set)
        extra = sorted(msg_name_set - id_set)
        raise ValueError(f"Id 与 MsgName 的 ID 不一致，缺少 {missing}，多出 {extra}")

    return id_entries, msg_name_entries


def _cpp_string(value: str) -> str:
    return json.dumps(value, ensure_ascii=True)


def generate_header(id_entries, msg_name_entries) -> str:
    lines = [
        "#pragma once",
        "",
        "#include <string>",
        "#include <unordered_map>",
        "",
        "enum NetMsgId : int {",
    ]

    if not any(value == 0 for _, value in id_entries):
        lines.append("\tNone = 0,")
    lines.extend(f"\t{name} = {value}," for name, value in id_entries)
    lines.extend(
        [
            "};",
            "",
            "inline const std::unordered_map<std::string, int> kNetMsgIdMap = {",
        ]
    )
    lines.extend(
        f"\t{{{_cpp_string(name)}, {value}}}," for name, value in id_entries
    )
    lines.extend(
        [
            "};",
            "",
            "inline const std::unordered_map<int, std::string> kNetMsgNameMap = {",
        ]
    )
    lines.extend(
        f"\t{{{value}, {_cpp_string(name)}}},"
        for value, name in msg_name_entries
    )
    lines.extend(["};", ""])
    return "\n".join(lines)


def main():
    base_path = Path(__file__).resolve().parent
    lua_path = base_path / "NetMsgId.lua"
    out_path = base_path / "NetMsgId.h"

    lua_text = lua_path.read_text(encoding="utf-8")
    id_entries, msg_name_entries = parse_lua(lua_text)
    out_path.write_text(
        generate_header(id_entries, msg_name_entries), encoding="utf-8"
    )

    print(f"Generated: {out_path}")


if __name__ == "__main__":
    main()
