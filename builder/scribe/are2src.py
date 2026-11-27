#!/usr/bin/env python3
"""Import DD4 area data into Scribe, checking the result before saving it."""
import argparse
from dataclasses import dataclass
from pathlib import Path
import os
import re
import subprocess
import tempfile


class AreaError(ValueError):
    pass


class Reader:
    def __init__(self, text):
        self.text = text
        self.pos = 0

    def skip(self):
        while self.pos < len(self.text) and self.text[self.pos].isspace():
            self.pos += 1

    def word(self):
        self.skip()
        match = re.match(r"\S+", self.text[self.pos:])
        if not match:
            raise AreaError("Unexpected end of area file")
        self.pos += len(match[0])
        return match[0]

    def peek(self):
        old = self.pos
        try:
            return self.word()
        finally:
            self.pos = old

    def string(self):
        self.skip()
        end = self.text.find("~", self.pos)
        if end < 0:
            raise AreaError("Unterminated area string")
        value = self.text[self.pos:end]
        self.pos = end + 1
        return value.rstrip("\r\n")

    def line(self):
        self.skip()
        end = self.text.find("\n", self.pos)
        if end < 0:
            end = len(self.text)
        value = self.text[self.pos:end]
        self.pos = min(end + 1, len(self.text))
        return value.rstrip("\r")

    def number(self):
        self.skip()
        match = re.match(r"[+-]?\d+(?:\|[+-]?\d+)*", self.text[self.pos:])
        if not match:
            raise AreaError("Expected a number near " + repr(self.text[self.pos:self.pos + 24]))
        value = match[0]
        self.pos += len(value)
        try:
            result = 0
            for part in value.split("|"):
                result |= int(part)
            return result
        except ValueError:
            raise AreaError("Expected a number, found " + repr(value)) from None


@dataclass
class Section:
    name: str
    raw: str
    data: object


def mobiles(reader):
    result = []
    while True:
        token = reader.word()
        if token == "#0":
            return result
        if not re.fullmatch(r"#\d+", token):
            raise AreaError("Expected mobile vnum, found " + token)
        row = {"vnum": int(token[1:]), "text": [reader.string() for _ in range(4)]}
        row["flags"] = [reader.number() for _ in range(3)]
        row["format"] = reader.word()
        row["level"] = reader.number()
        row["combat"] = [reader.word() for _ in range(4)]
        row["body"] = reader.number()
        row["gold"] = reader.word()
        row["positions"] = [reader.word(), reader.word()]
        row["sex"] = reader.number()
        row["programs"], row["metadata"], row["other"] = [], [], []
        row["selection"] = None
        while not reader.peek().startswith("#"):
            token = reader.word()
            if token.startswith(">"):
                kind = token[1:] or reader.word()
                argument = reader.string()
                script = None if kind == "in_file_prog" else reader.string()
                row["programs"].append([kind, argument, script])
            elif token == "|":
                continue
            elif token == "<":
                row["selection"] = [reader.string(), reader.string()]
            elif token.startswith("Mob"):
                value = reader.line()
                row["metadata"].append([token, value])
            else:
                row["other"].append(token + " " + reader.line())
        result.append(row)


def objects(reader):
    result = []
    while True:
        token = reader.word()
        if token == "#0":
            return result
        if not re.fullmatch(r"#\d+", token):
            raise AreaError("Expected object vnum, found " + token)
        row = {"vnum": int(token[1:]), "text": [reader.string() for _ in range(4)]}
        row["type"] = reader.number()
        row["extra"] = reader.number()
        row["trap"] = [reader.number() for _ in range(3)] if row["extra"] & (1 << 21) else None
        row["wear"] = reader.number()
        row["values"] = [reader.string() for _ in range(4)]
        row["tail"] = [reader.number() for _ in range(3)]
        row["extra_descriptions"], row["applies"], row["other"] = [], [], []
        row["material"] = None
        while not reader.peek().startswith("#"):
            token = reader.word()
            if token == "E":
                row["extra_descriptions"].append([reader.string(), reader.string()])
            elif token == "A":
                row["applies"].append([reader.number(), reader.number()])
            elif token == "M":
                row["material"] = reader.word()
            else:
                row["other"].append(token + " " + reader.line())
        result.append(row)


def rooms(reader):
    result = []
    while True:
        token = reader.word()
        if token == "#0":
            return result
        if not re.fullmatch(r"#\d+", token):
            raise AreaError("Expected room vnum, found " + token)
        row = {"vnum": int(token[1:]), "text": [reader.string(), reader.string()]}
        row["numbers"] = [reader.number() for _ in range(3)]
        row["exits"], row["extra_descriptions"], row["other"] = [], [], []
        while True:
            token = reader.word()
            if token == "S":
                break
            if token.startswith("D"):
                direction = int(token[1:]) if len(token) > 1 else reader.number()
                row["exits"].append([direction, reader.string(), reader.string(),
                                     reader.number(), reader.number(), reader.number()])
            elif token == "E":
                row["extra_descriptions"].append([reader.string(), reader.string()])
            else:
                row["other"].append(token + " " + reader.line())
        row["exits"].sort(key=lambda x: x[0])
        result.append(row)


def read_sections(text):
    reader = Reader(text)
    sections = []
    while True:
        token = reader.word()
        if token == "#$":
            reader.skip()
            if reader.pos != len(text):
                raise AreaError("Unexpected data after #$")
            return sections
        if not token.startswith("#"):
            raise AreaError("Expected section header, found " + token)
        name = token[1:]
        start = reader.pos
        if name == "AREA":
            data = [reader.string(), reader.string()] + [reader.number() for _ in range(4)]
        elif name == "MOBILES":
            data = mobiles(reader)
        elif name == "OBJECTS":
            data = objects(reader)
        elif name == "ROOMS":
            data = rooms(reader)
        elif name == "HELPS":
            data = []
            while True:
                level, keywords = reader.number(), reader.string()
                if keywords == "$":
                    break
                data.append([level, keywords, reader.string()])
        elif name == "OBJECT_SETS":
            data = []
            while reader.word() != "#0":
                reader.string()
                reader.string()
                reader.line()
                reader.line()
                while not reader.peek().startswith("#"):
                    reader.line()
        elif name == "RECALL":
            data = reader.number()
        elif name in {"AREA_SPECIAL", "ROOMS_AMBIENT", "EXITS_SFX"}:
            data = []
            while True:
                line = reader.line()
                if line == "$":
                    break
                if name == "AREA_SPECIAL" and line.startswith("reset_msg"):
                    # The message can include a line containing a section name.
                    reader.pos -= len(line) + 1
                    reader.word()
                    reader.string()
        elif name == "SHOPS":
            data = []
            while reader.number() != 0:
                for _ in range(9):
                    reader.number()
                end = reader.text.find("\n", reader.pos)
                reader.pos = end + 1 if end >= 0 else len(reader.text)
        elif name in {"RESETS", "SPECIALS", "GAMES", "MOBPROGS"}:
            end = "S"
            data = []
            while reader.line().split()[0] != end:
                pass
        else:
            raise AreaError("Unsupported area section " + name)
        raw = text[start:reader.pos].strip("\r\n")
        sections.append(Section(name, raw, data if name in {"AREA", "MOBILES", "OBJECTS", "ROOMS", "HELPS", "RECALL"} else raw.strip()))


def field(name, value):
    value = str(value)
    if "\n" in value or "\r" in value or "~" in value:
        raise AreaError("This field needs native syntax")
    return name + " " + value + "\n"


def text_field(name, value):
    if re.search(r"(?m)^\$\w+|^~", value):
        raise AreaError("This text needs native syntax")
    return name + "\n" + value + "\n~\n"


MOB_FIELDS = {
    "MobResists": "res", "MobVulnerabilities": "vuln", "MobImmunes": "imm",
    "MobAttackParts": "atk", "MobHPMod": "hpmod", "MobDamMod": "dammod",
    "MobCritMod": "critmod", "MobSwiftMod": "swiftmod", "MobHeight": "height",
    "MobLength": "length", "MobWeight": "weight", "MobSize": "size",
    "MobLanguage": "language", "MobGhostPhase": "ghostphase",
}


def friendly(section):
    output = []
    if section.name == "AREA":
        keys = ["au", "ti", "ls", "us", "le", "ue"]
        return "$area\nbv 0\n" + "".join(field(k, v) for k, v in zip(keys, section.data))
    if section.name == "MOBILES":
        for row in section.data:
            if (row["combat"] != ["0", "0", "0d0+0", "0d0+0"]
                    or row["positions"] != ["0", "0"] or row["gold"] != "0"
                    or row["format"] != "S" or row["other"]):
                raise AreaError("Mobile contains native legacy fields")
            names = row["text"]
            output.append("$mob\n" + field("vn", row["vnum"]))
            output.extend(field(k, v) for k, v in zip(["nm", "sh", "lo"], names[:3]))
            output.append(text_field("de", names[3]))
            output.extend(field(k, v) for k, v in zip(["act", "aff", "al"], row["flags"]))
            output.extend([field("bf", row["body"]), field("lv", row["level"]), field("sx", row["sex"])])
            if row["selection"]:
                output.append(field("archetype", row["selection"][0]))
                output.append(field("rnk", row["selection"][1]))
            for key, value in row["metadata"]:
                if key not in MOB_FIELDS:
                    raise AreaError("Mobile extension needs native syntax")
                output.append(field(MOB_FIELDS[key], value))
            for kind, argument, program in row["programs"]:
                if program is None:
                    raise AreaError("External program needs native syntax")
                output.append(text_field("mp " + kind + " " + argument, program))
    elif section.name == "OBJECTS":
        for row in section.data:
            if row["text"][3] or row["tail"][1] or row["other"]:
                raise AreaError("Object contains native legacy fields")
            output.append("$obj\n" + field("vn", row["vnum"]))
            output.extend(field(k, v) for k, v in zip(["nm", "sh", "lo"], row["text"][:3]))
            output.extend([field("ty", row["type"]), field("ex", row["extra"]), field("we", row["wear"]),
                           field("wg", row["tail"][0]), field("osl", row["tail"][2])])
            if row["trap"]:
                trigger, damage, charges = row["trap"]
                output.extend([field("trt", trigger), field("trd", damage + 1), field("trc", charges)])
            for i, value in enumerate(row["values"]):
                output.append(field("v" + str(i), value))
            for key, description in row["extra_descriptions"]:
                output.append(text_field("ed " + key, description))
            for location, modifier in row["applies"]:
                output.append(field("ap", str(location) + " " + str(modifier)))
            if row["material"] is not None:
                output.append(field("mi", row["material"]))
    elif section.name == "ROOMS":
        for row in section.data:
            if row["numbers"][0] or row["other"]:
                raise AreaError("Room contains native legacy fields")
            output.extend(["$room\n", field("vn", row["vnum"]), field("nm", row["text"][0]),
                           text_field("de", row["text"][1]), field("rf", row["numbers"][1]),
                           field("st", row["numbers"][2])])
            for direction, description, keyword, locks, key, destination in row["exits"]:
                if direction < 0 or direction > 5:
                    raise AreaError("Exit direction needs native syntax")
                name = "neswud"[direction]
                output.extend([field(name, destination), text_field(name + "de", description),
                               field(name + "nm", keyword), field(name + "lo", locks),
                               field(name + "ke", key)])
            for keyword, description in row["extra_descriptions"]:
                output.append(text_field("ed " + keyword, description))
    else:
        raise AreaError("This section uses native source syntax")
    return "\n".join(output)


def native(section):
    marker = "END_NATIVE_" + section.name
    while marker in section.raw.splitlines():
        marker += "_"
    body = section.raw if section.raw.endswith("\n") else section.raw + "\n"
    return "$native " + section.name + " " + marker + "\n" + body + marker + "\n"


def compile_source(source, compiler, folder):
    input_path, output_path = folder / "check.src", folder / "check.are"
    input_path.write_bytes(source.encode())
    output_path.unlink(missing_ok=True)
    result = subprocess.run(["perl", str(compiler), str(input_path), str(output_path)],
                            text=True, capture_output=True)
    if result.returncode or not output_path.exists() or "Errors:" in result.stdout:
        raise AreaError(result.stdout.strip() or result.stderr.strip() or "Scribe rejected the source")
    return output_path.read_text(encoding="utf-8")


def convert(text, compiler):
    try:
        sections = read_sections(text)
    except AreaError:
        return retain_file(text, compiler)
    areas = [s for s in sections if s.name == "AREA"]
    if not areas:
        return retain_file(text, compiler)
    if len(areas) != 1:
        raise AreaError("Expected exactly one AREA header")
    header = friendly(areas[0])
    chunks, retained = [header], []
    with tempfile.TemporaryDirectory(prefix="dd4-are2src-") as directory:
        folder = Path(directory)
        for section in sections:
            if section.name == "AREA":
                continue
            try:
                candidate = friendly(section)
                checked = read_sections(compile_source(header + "\n" + candidate, compiler, folder))
                generated = [s for s in checked if s.name == section.name]
                if len(generated) != 1 or generated[0].data != section.data:
                    raise AreaError("Scribe would change native data")
                chunks.append(candidate)
            except AreaError:
                chunks.append(native(section))
                retained.append(section.name)
        result = "\n\n".join(chunks) + "\n"
        rebuilt = read_sections(compile_source(result, compiler, folder))
        # Section order may change; order inside a native reset table stays exact.
        expected = sorted((s.name, repr(s.data)) for s in sections)
        actual = sorted((s.name, repr(s.data)) for s in rebuilt)
        if expected != actual:
            raise AreaError("Round-trip comparison failed; destination was not written")
    return result, retained


def retain_file(text, compiler):
    """Keep unrepresentable legacy records visibly intact, without guessing."""
    if not re.search(r"(?m)^#(?:AREA|HELPS)\b", text) or not re.search(r"(?m)^#\$\s*$", text):
        raise AreaError("Expected a complete DD4 area or help file")
    result = native(Section("FILE", text, None))
    with tempfile.TemporaryDirectory(prefix="dd4-are2src-") as directory:
        rebuilt = compile_source(result, compiler, Path(directory))
    if rebuilt != text:
        raise AreaError("Native file comparison failed; destination was not written")
    return result, ["FILE (legacy data retained verbatim)"]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("area", type=Path)
    parser.add_argument("source", type=Path)
    parser.add_argument("--scribe", type=Path, default=Path(__file__).with_name("scribe.pl"))
    parser.add_argument("--force", action="store_true", help="replace an existing destination")
    args = parser.parse_args()
    try:
        if args.area.resolve() == args.source.resolve() or args.source.resolve() == args.scribe.resolve():
            raise AreaError("Choose a separate destination")
        if args.source.exists() and not args.force:
            raise AreaError("Destination exists; use --force to replace it")
        result, retained = convert(args.area.read_text(encoding="utf-8"), args.scribe.resolve())
        args.source.parent.mkdir(parents=True, exist_ok=True)
        temporary = None
        try:
            with tempfile.NamedTemporaryFile(dir=args.source.parent, prefix=".are2src-", delete=False) as stream:
                stream.write(result.encode())
                temporary = Path(stream.name)
            os.replace(temporary, args.source)
        finally:
            if temporary and temporary.exists():
                temporary.unlink()
        print("Converted and round-trip checked", args.area)
        if retained:
            print("Readable native sections:", ", ".join(retained))
    except (AreaError, OSError, UnicodeError) as error:
        parser.exit(1, str(error) + "\n")


if __name__ == "__main__":
    main()
