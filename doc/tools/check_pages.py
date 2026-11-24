#!/usr/bin/env python3
"""Check every language's index.md and pages/*.md under doc/<lang>/ against the style guide, as a compiler would.

The word lists come from the tables "Words" and "Names" in doc/STYLE_GUIDE.md. Prints one line per
mistake, `<file>:<line>: <message>`, and returns the number of mistakes. Needs nothing but Python 3.9.

  python3 doc/tools/check_pages.py [<page.md> ...]
"""

import ast
import fnmatch
import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
DOC_DIR = REPO / "doc"
STYLE_GUIDE = DOC_DIR / "STYLE_GUIDE.md"
# The style guide's word lists are English: other languages get the rules that do not depend on words.
WORD_LANGUAGE = "en"

MAX_SENTENCE_WORDS = 25
MAX_PARAGRAPH_SENTENCES = 4
# Directives whose body is text a reader reads.
PROSE_DIRECTIVES = {"note", "tip", "hint", "warning", "important", "caution", "attention", "danger", "error",
                    "admonition", "seealso"}
FIRST_PERSON = re.compile(r"\b(we|We|our|Our|ours|us)\b")
# A fence can open a list item: "- ```{image} …".
FENCE = re.compile(r"^(\s*(?:[-*+]\s+|\d+[.)]\s+)?)(`{3,}|~{3,}|:{3,})\s*(\{([\w-]+)\})?\s*(.*)$")
HEADING = re.compile(r"^(#{1,6})\s+(.*?)\s*$")
SETEXT = re.compile(r"^\s*(=+|-+)\s*$")
REFERENCE_DEFINITION = re.compile(r"^\[([^\]^]+)\]:\s*\S+")
REFERENCE_USE = re.compile(r"\]\[([^\]]+)\]")
INLINE_IMAGE = re.compile(r"!\[([^\]]*)\]\(([^)\s]+)")
IMAGE_PATH = "../images/{page}/"
# Formats a browser displays: screenshots PNG, drawings SVG, photos JPG (doc/STYLE_GUIDE.md).
IMAGE_EXTENSIONS = ("png", "jpg", "svg", "gif", "webp")
IMAGE_NAME = re.compile(r"[a-z0-9]+(-[a-z0-9]+)*\.(" + "|".join(IMAGE_EXTENSIONS) + ")")
PAGE_NAME = re.compile(r"[a-z0-9]+(-[a-z0-9]+)*")
HTML = re.compile(r"<!--|</?[A-Za-z][\w-]*(\s[^<>]*)?/?>")
VERSION_NOTE = re.compile(r"\*(Since|Removed in) QMapShack \d+\.\d+(\.\d+)?\*")
# A whole line in italics that starts like a version note.
VERSION_NOTE_LIKE = re.compile(r"^\*(Since|Removed|Deprecated|Applies|Added|New)\b[^*]*\*$")
WORD = re.compile(r"[A-Za-z][\w'’-]*")
CODE = re.compile(r"(`+).+?\1")
URL = re.compile(r"\b[a-z][a-z0-9+.-]*://[^\s)\]>]+")
QUOTED = re.compile(r'"[^"]*"|“[^”]*”')
# A sentence starts with any capital letter (Ä, É, …), a digit, markup or an opening mark (¿ ¡ „ «).
CAPITALS = "".join(chr(c) for c in range(0x250) if chr(c).isupper())
SENTENCE_END = re.compile(r"(?<=[.!?])\s+(?=[" + CAPITALS + r"0-9*`\[(\"“„«§¿¡])")


def strip_inline(text):
    """Text as a reader reads it: without code, link targets and roles; a UI label becomes '§'."""
    text = CODE.sub(" ", text)
    text = URL.sub(" ", text)
    text = re.sub(r"\{[\w:-]+\}", " ", text)
    # Not a gap: "Click **Apply** on" must not read as "click on".
    text = re.sub(r"\*\*[^*]+\*\*", " § ", text)
    text = INLINE_IMAGE.sub(" ", text)
    text = re.sub(r"\]\([^)]*\)", "]", text)
    text = re.sub(r"\]\[[^\]]*\]", "]", text)
    return text


class Rules:
    """What the style guide and the user interface define."""

    def __init__(self):
        self.mistakes = []
        guide = STYLE_GUIDE.read_text(encoding="utf-8").splitlines()
        words, self.words_lines = self._table(guide, "Words")
        names, self.names_lines = self._table(guide, "Names")

        uses = {row[0] for row in words}
        self.wrong_words = []
        for row in words:
            for wrong in re.sub(r"\([^)]*\)", "", row[1]).split(","):
                wrong = wrong.strip()
                if not wrong:
                    continue
                variants = [wrong]
                capitalized = wrong[0].upper() + wrong[1:]
                if capitalized != wrong and capitalized not in uses:
                    variants.append(capitalized)
                for variant in variants:
                    # Each word in any inflection: "double clicked", "coloured".
                    words = [re.escape(word) + r"(?:s|es|d|ed|ing)?" for word in variant.split()]
                    pattern = r"(?<!\w)" + r"\s+".join(words) + r"(?![\w-])"
                    self.wrong_words.append((re.compile(pattern), wrong, row[0]))

        self.names = {}
        for row in names:
            for name in row[0].split(","):
                name = name.strip()
                if name:
                    self.names[name.lower()] = name

    def _table(self, lines, heading):
        """The rows of the first table below `## <heading>`, without the header, and its line numbers."""
        start = None
        for i, line in enumerate(lines):
            if line.strip() == "## " + heading:
                start = i
                break
        if start is None:
            sys.exit(f"{STYLE_GUIDE.relative_to(REPO)} has no section '## {heading}'; the checks need its table.")
        rows, numbers = [], set()
        for i in range(start + 1, len(lines)):
            line = lines[i].strip()
            if line.startswith("##"):
                break
            if line.startswith("|"):
                numbers.add(i + 1)
                cells = [cell.strip() for cell in line.strip("|").split("|")]
                if not re.fullmatch(r"[-:| ]+", line):
                    rows.append(cells)
            elif rows:
                break
        return rows[1:], numbers

class Page:
    def __init__(self, path, rules):
        self.path = path
        self.rules = rules
        self.name = path.relative_to(REPO).as_posix() if path.is_relative_to(REPO) else str(path)
        self.is_guide = path.resolve() == STYLE_GUIDE.resolve()
        # doc/<lang>/…; the guide itself and a page outside doc/ count as English.
        resolved = path.resolve()
        parts = resolved.relative_to(DOC_DIR.resolve()).parts if resolved.is_relative_to(DOC_DIR.resolve()) else ()
        self.words_apply = len(parts) < 2 or parts[0] == WORD_LANGUAGE

    def report(self, line, message):
        self.rules.mistakes.append(f"{self.name}:{line}: {message}")

    def check(self):
        raw = self.path.read_bytes()
        try:
            text = raw.decode("utf-8")
        except UnicodeDecodeError as e:
            self.report(text_line(raw, e.start), "not UTF-8; save the file as UTF-8")
            return
        if self.path.parent.name == "pages" and not PAGE_NAME.fullmatch(self.path.stem):
            self.report(1, f"file name '{self.path.name}': use lower case letters, digits and hyphens")
        self.lines = text.splitlines()
        for number, line in enumerate(self.lines, 1):
            if "\t" in line:
                self.report(number, "tab character; indent with four spaces")
        self.scan()

    def scan(self):
        headings = {}
        titles = 0
        paragraph = []
        in_list = [False]
        fence = None  # (marker, directive, start line)
        index_pending = None
        definitions_started = None
        defined, used = set(), []
        skip = self.rules.words_lines | self.rules.names_lines if self.is_guide else set()
        previous = ""

        def end_paragraph():
            if paragraph:
                self.check_paragraph(paragraph, in_list[0])
                paragraph.clear()
            in_list[0] = False

        front_matter_end = 0
        if self.lines and self.lines[0].strip() == "---":
            for i in range(1, len(self.lines)):
                if self.lines[i].strip() == "---":
                    front_matter_end = i + 1
                    break

        for number, line in enumerate(self.lines, 1):
            stripped = line.strip()
            if number <= front_matter_end:
                continue

            if fence is not None:
                marker, directive, start = fence
                if stripped.startswith(marker[0] * len(marker)) and stripped.strip(marker[0]) == "":
                    fence = None
                    end_paragraph()
                    if directive == "index":
                        index_pending = start
                    previous = ""
                    continue
                if directive == "index":
                    self.check_index(number, stripped)
                elif directive == "table" and stripped.startswith("|"):
                    self.check_table_row(number, line, skip)
                elif (directive in PROSE_DIRECTIVES or directive == "figure") and not stripped.startswith(":"):
                    if stripped:
                        self.check_words(number, line)
                        paragraph.append((number, stripped))
                    else:
                        end_paragraph()
                continue

            match = FENCE.match(line)
            if match:
                end_paragraph()
                marker, directive, argument = match.group(2), match.group(4), match.group(5).strip()
                if index_pending is not None and directive != "index":
                    self.report(index_pending, "index entry not followed by a heading; put it directly above "
                                               "the heading of its section")
                    index_pending = None
                fence = (marker, directive, number)
                if directive == "index" and argument:
                    self.check_index(number, argument)
                elif directive in ("figure", "image"):
                    self.check_image_path(number, argument)
                continue

            if not stripped:
                end_paragraph()
                previous = ""
                continue

            if index_pending is not None:
                if not HEADING.match(line):
                    self.report(index_pending, "index entry not followed by a heading; put it directly above "
                                               "the heading of its section")
                index_pending = None

            if definitions_started is not None and not REFERENCE_DEFINITION.match(stripped):
                self.report(definitions_started, "link reference defined before the end of the page; move "
                                                 "the definitions to the end")
                definitions_started = None

            heading = HEADING.match(line)
            if heading:
                end_paragraph()
                level, title = len(heading.group(1)), heading.group(2)
                if level == 1:
                    titles += 1
                    if titles == 2:
                        self.report(number, "second title ('#'); a page has one, use '##' for a section")
                self.check_heading(number, title, headings)
                if number < len(self.lines) and self.lines[number].strip():
                    self.report(number, "no empty line after the heading")
                previous = ""
                continue

            if SETEXT.match(line) and previous and not previous.startswith("|"):
                self.report(number, "heading underlined with '=' or '-'; start it with '#' instead")
                previous = ""
                end_paragraph()
                continue

            definition = REFERENCE_DEFINITION.match(stripped)
            if definition:
                end_paragraph()
                defined.add(definition.group(1).lower())
                if definitions_started is None:
                    definitions_started = number
                previous = stripped
                continue

            indent = len(line) - len(line.lstrip(" "))
            if indent % 4:
                self.report(number, f"indented by {indent} spaces; indent by a multiple of four")
            if re.match(r"\s*[*+]\s", line):
                self.report(number, "bullet starts with '*' or '+'; start it with '-'")
            for label in REFERENCE_USE.findall(line):
                used.append((number, label))
            if re.search(r"\]\(https?://", line):
                self.report(number, "external link written inline; write [text][NAME] and define NAME at "
                                    "the end of the page")
            if "[^" in line:
                self.report(number, "footnote; write the note into the text")
            self.check_images(number, line)
            self.check_menu_separators(number, line)

            if VERSION_NOTE_LIKE.match(stripped) and not VERSION_NOTE.fullmatch(stripped):
                self.report(number, "version note: write '*Since QMapShack <version>*' or "
                                    "'*Removed in QMapShack <version>*'")

            if stripped.startswith("|"):
                end_paragraph()
                self.check_table_row(number, line, skip)
                previous = stripped
                continue

            if number not in skip:
                self.check_words(number, line)
            if re.match(r"\s*(-|\d+\.)\s", line):
                end_paragraph()
                paragraph.append((number, re.sub(r"^\s*(-|\d+\.)\s+", "", stripped)))
                in_list[0] = True
            elif not stripped.startswith("{") and stripped != "---":
                paragraph.append((number, stripped))
            previous = stripped

        end_paragraph()
        if fence is not None:
            self.report(fence[2], "block is not closed")
        if titles == 0:
            self.report(1, "page has no title; start it with '# <title>'")
        for number, label in used:
            if label.lower() not in defined:
                self.report(number, f"link reference [{label}] is not defined at the end of the page")

    def check_heading(self, number, title, headings):
        if title.endswith("#"):
            self.report(number, "heading ends with '#'; remove it")
        elif re.search(r"[.:;,!?]$", title):
            self.report(number, "heading ends with a punctuation mark; remove it")
        if re.search(r"[*`]|(^|\s)_", title):
            self.report(number, "formatted heading; write it as plain text")
        self.check_sentence_case(number, title, "heading")
        self.check_words(number, title)
        key = title.lower()
        if key in headings:
            self.report(number, f"heading '{title}' is already used in line {headings[key]}; make it unique")
        else:
            headings[key] = number

    def check_sentence_case(self, number, text, what):
        if not self.words_apply:
            return
        # A quoted user interface label keeps its capitals: ## The "Load GIS Data" dialog
        words = WORD.findall(QUOTED.sub(" ", strip_inline(text.replace(";", " "))))
        for word in words[1:]:
            simple_capital = word[0].isupper() and word[1:] == word[1:].lower() and len(word) > 1
            if simple_capital and word.lower() not in self.rules.names:
                self.report(number, f"{what} '{text}': write '{word}' in lower case, or add it to the "
                                    f"names in the style guide")

    def check_index(self, number, entry):
        entry = entry.strip()
        if not entry or entry.startswith(":"):
            return
        kind, _, term = entry.partition(":")
        if kind.strip() != "single":
            self.report(number, f"index entry '{entry}': use 'single:'")
            if kind.strip() != "pair":
                return
        if re.search(r"[*`]", term):
            self.report(number, f"index entry '{entry}': write it as plain text")
        self.check_sentence_case(number, term.strip(), "index entry")
        self.check_words(number, term)

    def check_table_row(self, number, line, skip):
        cells = line.strip().strip("|").split("|")
        if "![" in line or re.search(r"<br\s*/?>", line, re.IGNORECASE) or any(
                re.match(r"\s*(-|\*|\d+\.)\s", cell) for cell in cells):
            self.report(number, "list, image or line break in a table cell; keep cells to short text")
        if number not in skip:
            self.check_words(number, line)

    def check_image_path(self, number, path):
        page = self.path.stem
        # doc/index.md sits one level above the pages.
        prefix = IMAGE_PATH.format(page=page)
        if self.path.parent.name != "pages":
            prefix = prefix[len("../"):]
        if not path.startswith(prefix):
            lang = (self.path.parent.parent if self.path.parent.name == "pages" else self.path.parent).name
            self.report(number, f"image '{path}': put it in doc/{lang}/images/{page}/ and write '{prefix}<name>.<ext>'")
        elif not IMAGE_NAME.fullmatch(path[len(prefix):]):
            self.report(number, f"image '{path}': name it '<subject>[-<variant>].<ext>' in lower case with hyphens, "
                        f"<ext> one of {', '.join(IMAGE_EXTENSIONS)}")
        # The HTML build copies every page's images into one folder by file name.
        elif not path[len(prefix):].startswith(page + "-"):
            self.report(number, f"image '{path}': start its name with the page name, '{prefix}{page}-<name>.<ext>'")

    def check_images(self, number, line):
        for _, path in INLINE_IMAGE.findall(CODE.sub(" ", line)):
            self.check_image_path(number, path)

    def check_menu_separators(self, number, line):
        for label in re.findall(r"\*\*([^*]+)\*\*", CODE.sub(" ", line)):
            if re.search(r"\s(>|->|=>|/)\s", label):
                self.report(number, f"menu path '{label}': separate the entries with ' → '")

    def check_words(self, number, line):
        if not self.words_apply:
            return
        # A quoted word is mentioned, not used.
        text = QUOTED.sub(" ", strip_inline(line))
        for pattern, wrong, right in self.rules.wrong_words:
            if pattern.search(text):
                self.report(number, f"'{wrong}': write '{right}'")
        for word in WORD.findall(text):
            name = self.rules.names.get(word.lower())
            if name and word != name and not word.islower():
                self.report(number, f"'{word}': write '{name}'")
        if FIRST_PERSON.search(text):
            self.report(number, "first person; address the reader as 'you'")
        if "..." in text:
            self.report(number, "three dots; write '…'")
        if HTML.search(text):
            self.report(number, "HTML or a placeholder outside code; write a placeholder as `<name>`")

    def check_paragraph(self, lines, is_list=False):
        text, starts = "", []
        for number, line in lines:
            starts.append((len(text), number))
            text += strip_inline(line) + " "
        sentences, offset = [], 0
        for part in SENTENCE_END.split(text):
            position = text.find(part, offset)
            offset = position + len(part)
            if part.strip():
                sentences.append((position, part))
        for position, sentence in sentences:
            count = len(WORD.findall(sentence))
            if count > MAX_SENTENCE_WORDS:
                number = max(n for start, n in starts if start <= position)
                self.report(number, f"sentence of {count} words; keep it to {MAX_SENTENCE_WORDS}")
        if not is_list and len(sentences) > MAX_PARAGRAPH_SENTENCES:
            self.report(lines[0][0], f"paragraph of {len(sentences)} sentences; keep it to "
                                     f"{MAX_PARAGRAPH_SENTENCES}")


def text_line(raw, offset):
    return raw[:offset].count(b"\n") + 1


def excluded_patterns():
    """Sphinx's exclude_patterns from doc/conf.py, so a page Sphinx skips is not checked either."""
    tree = ast.parse((DOC_DIR / "conf.py").read_text(encoding="utf-8"))
    for node in tree.body:
        if isinstance(node, ast.Assign) and any(getattr(t, "id", None) == "exclude_patterns" for t in node.targets):
            return ast.literal_eval(node.value)
    return []


def pages():
    """Every language folder's pages, doc/<lang>/index.md and pages/*.md, less Sphinx's exclude_patterns."""
    patterns = excluded_patterns()
    found = []
    for index in sorted(DOC_DIR.glob("*/index.md")):
        source = index.parent
        found += [p for p in [index] + sorted((source / "pages").glob("*.md"))
                  if not any(fnmatch.fnmatch(p.relative_to(source).as_posix(), pattern) for pattern in patterns)]
    return found


def check(paths=None):
    """Check the pages; print each mistake and return how many there are."""
    rules = Rules()
    for path in paths or pages():
        Page(Path(path).resolve(), rules).check()
    for mistake in rules.mistakes:
        print(mistake)
    sys.stdout.flush()
    return len(rules.mistakes)


def main():
    count = check(sys.argv[1:])
    if count:
        sys.exit(f"{count} mistake(s) against the style guide (doc/STYLE_GUIDE.md).")


if __name__ == "__main__":
    main()
