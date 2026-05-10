import re
from pathlib import Path

def parse_lua(text: str):
    id_block = re.search(r'NetMsgId\.Id\s*=\s*\{([\s\S]*?)\}', text)

    name_to_id = {}

    if id_block:
        entries = re.findall(r'(\w+)\s*=\s*(-?\d+)', id_block.group(1))
        for name, id_ in entries:
            name_to_id[name] = int(id_)

    return name_to_id


def generate_proto_enum(name_to_id: dict):
    lines = []

    has_zero = any(id_ == 0 for id_ in name_to_id.values())

    lines.append("syntax = \"proto3\";")
    lines.append("")
    lines.append("enum NetMsgId {")

    used_ids = set()
    used_names = set()

    if not has_zero:
        lines.append("    // None")
        lines.append("    None = 0;")
        lines.append("")

        used_ids.add(0)
        used_names.add("None")

    for name, id_ in name_to_id.items():

        safe_name = re.sub(r'\W', '_', name)

        if safe_name in used_names:
            print(f"[WARN] duplicate name: {safe_name}")
            continue

        if id_ in used_ids:
            print(f"[WARN] duplicate id: {id_} ({safe_name})")
            continue

        used_names.add(safe_name)
        used_ids.add(id_)

        lines.append(f"    {safe_name} = {id_};")

    lines.append("}")
    lines.append("")

    return "\n".join(lines)


def main():
    lua_path = Path("NetMsgId.lua")
    lua_text = lua_path.read_text(encoding="utf-8")

    name_to_id = parse_lua(lua_text)

    proto_text = generate_proto_enum(name_to_id)

    out_path = Path("NetMsgId.proto")
    out_path.write_text(proto_text, encoding="utf-8")

    print(f"Generated: {out_path}")


if __name__ == "__main__":
    main()
    
