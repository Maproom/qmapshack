#!/usr/bin/env python3
"""Mark up a UI icon for theming: ink/paper -> currentColor + a style block.

The phase-7a transform. Two themed roles, because 87 icons pair a navy body with
a light label (Save.svg) and those must move in OPPOSITE directions:

  .ink    the drawing colour   -- #000080 navy today
  .paper  what ink sits on     -- white/#ececec/#cccccc/#f9f9f9 today

Everything else is left alone. That is the point: a fill that is not
currentColor never moves, so semantic red/green survive with no carve-out.

Output shape:

  <style id="current-color-scheme">.ink{color:#000080}.paper{color:#ececec}</style>
  <path class="ink" style="fill:currentColor" .../>

The style block's DEFAULTS are today's colours, so an unthemed render -- inkscape,
mkicon, Qt's own qsvgicon -- is unchanged. Verified: both inkscape and Qt resolve
the two classes correctly. Do not drop the defaults; currentColor with no colour
set renders BLACK in both, which would silently blacken every exported PNG.

Preview (writes nothing):
  ./themesvg.py --dir src/icons --stats
Migrate in place:
  ./themesvg.py --dir src/icons --write
"""

import argparse
import os
import re
import sys
import xml.etree.ElementTree as ET

# The 21 self-backgrounded icons (blue Mime tiles, project badges, Off, ToWksUnchecked). Their
# content sits on their OWN colour, not the app background, so the ink/paper model mislabels it;
# they are left un-themed. Single source of truth is palette.py, shared with the accent pass.
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from palette import KEEP_BLUE, OPTOUT  # noqa: E402

SVG_NS = "http://www.w3.org/2000/svg"
ET.register_namespace("", SVG_NS)

INK = {"#000080"}
PAPER = {"#ffffff", "#ececec", "#f9f9f9", "#cccccc"}
# Black is a THIRD role, not more ink. 142 icons draw with both navy and black; sharing a
# role collapses them to one colour on dark and the drawing loses its detail. Reviewed on
# the A/B sheet -- separate won.
LEAD = {"#000000"}
# A second, brighter blue that coexists with navy in 23 of its 31 icons -- folding it
# into `ink` would collapse that distinction. Used to mark the element being acted on.
# Both spellings: palette.py normalises #0000ff -> #5555ff, and svghygiene runs that BEFORE
# this transform, so an icon reaching here carries the accent value. Recognising only the
# pre-normalisation one silently dropped the role on 29 icons and left them literal blue.
MARK = {"#0000ff", "#5555ff"}

STYLE_ID = "current-color-scheme"

# Placeholder left where an existing scheme block sat, so a re-theme puts the new
# block back in the same spot with the same surrounding whitespace.
SCHEME_MARK = "<!--qms-scheme-->"

# Each distinct source colour keeps its OWN class and its OWN default, so the
# unthemed render is byte-identical to today. Collapsing the 4 light shades onto
# one default would silently restyle the 12 icons that use more than one
# (Save.svg pairs #ececec with #cccccc) -- and "the light render never moves" is
# the property that makes this safe to ship.


def norm(c):
    c = (c or "").strip().lower()
    if re.fullmatch(r"#[0-9a-f]{3}", c):
        c = "#" + "".join(ch * 2 for ch in c[1:])
    return c


def role_of(color, keep=()):
    """Themed role for a source colour, or None to leave it literal.

    `keep` names colours this particular icon must not theme -- where the colour is a
    swatch in a palette rather than part of the drawing.
    """
    c = norm(color)
    if c in keep:
        return None
    if c in INK:
        return "ink"
    if c in PAPER:
        return "paper"
    if c in LEAD:
        return "lead"
    if c in MARK:
        return "mark"
    return None


def style_to_dict(s):
    out = {}
    for part in (s or "").split(";"):
        if ":" in part:
            k, v = part.split(":", 1)
            out[k.strip()] = v.strip()
    return out


def dict_to_style(d):
    return ";".join(f"{k}:{v}" for k, v in d.items())


SHAPES = {f"{{{SVG_NS}}}{t}" for t in
          ("path", "rect", "circle", "ellipse", "polygon", "polyline", "line")}


def local_fill(el):
    for part in (el.get("style") or "").split(";"):
        if part.strip().startswith("fill:"):
            return part.split(":", 1)[1].strip().lower()
    return (el.get("fill") or "").lower() or None


def implied_black_ids(el, inherited=None, acc=None):
    """ids of shapes that render black only because SVG's default fill is black.

    40 shapes across 29 icons (AreaMove, LineMove, WptMove, WptEditProx, ...)
    declare no fill anywhere up their tree and lean on that default. Nothing can
    recolour a colour that was never written down, so they stay black on a dark
    ground whatever the theme does -- this is what "some of the arrow heads are
    still black" was.

    Fill is INHERITED, so only a shape with no fill on ANY ancestor defaults to
    black: a path inside <g fill="#ff0000"> is red. Hence the recursion.
    """
    if acc is None:
        acc = []
    f = local_fill(el) or inherited
    if el.tag in SHAPES and f is None and el.get("id"):
        acc.append(el.get("id"))
    for child in el:
        implied_black_ids(child, f, acc)
    return acc


def add_fill_by_id(text, el_id, color="#000000"):
    """Insert an explicit fill into ONE element's opening tag, touching nothing else.

    Deliberately a text edit, not a parse/reserialise round-trip: ElementTree
    rewrites the whole document (drops the xml declaration and Inkscape's
    namespaces, reflows every attribute), which turned a 40-attribute fix into a
    2000-line diff nobody could review. Attribute values here never contain '>',
    so [^>]* is safe across Inkscape's multi-line tags.
    """
    tag_re = re.compile(
        r"<(?:path|rect|circle|ellipse|polygon|polyline|line)\b[^>]*?"
        r'id="' + re.escape(el_id) + r'"[^>]*?/?>', re.S)
    m = tag_re.search(text)
    if not m:
        return text, False
    tag = m.group(0)

    style_re = re.compile(r'style="([^"]*)"')
    sm = style_re.search(tag)
    if sm:
        fixed = tag[:sm.start(1)] + (sm.group(1) + ";fill:" + color).lstrip(";") + tag[sm.end(1):]
    else:
        end = -2 if tag.endswith("/>") else -1
        fixed = tag[:end] + f' fill="{color}"' + tag[end:]
    return text[:m.start()] + fixed + text[m.end():], True


def token_for(color, tokens):
    """Stable role token per distinct source colour: ink, paper, paper2, lead, ..."""
    c = norm(color)
    if c in tokens:
        return tokens[c]
    base = role_of(c)
    assert base, f"no role for {c}"
    n = sum(1 for v in tokens.values() if v.rstrip("0123456789") == base)
    name = base if n == 0 else f"{base}{n + 1}"
    tokens[c] = name
    return name


def class_for(decls, tokens, rules):
    """Name a rule from the colours it themes, and register it.

    `decls` is an ordered list of (property, source_colour). The class name is the
    role tokens joined by '-', in declaration order, so the engine can assign a
    role positionally without parsing colours:

        [("color", "#000080")]                        -> .ink        {color:#000080}
        [("fill", "#ffffff"), ("color", "#000080")]   -> .paper-ink  {fill:#ffffff;color:#000080}

    The two-declaration form is the two-colour element: an element has a single
    `color`, so a themed fill and a differently themed stroke cannot both be
    currentColor. The fill is served from the class and `color` feeds an inline
    stroke="currentColor". Verified in Qt and inkscape.
    """
    name = "-".join(token_for(c, tokens) for _, c in decls)
    rules[name] = decls
    return name


# --- text-level tag editing -----------------------------------------------------------------
# The markup rewrites each element's opening tag in the ORIGINAL text, never by reserialising
# the document: ET.tostring() drops the xml declaration and Inkscape's namespaces and reflows
# every attribute (a 40-attribute fix once became a 2000-line diff). Attribute values in these
# icons never contain '>', so [^>] bounds a tag safely -- the same assumption add_fill_by_id
# relies on. A role is decided from an element's OWN fill/stroke, so no tree is needed here.

TAG_RE = re.compile(r"<[a-zA-Z][\w:.-]*\b[^>]*?/?>", re.S)


def _attr_re(name):
    return re.compile(r"\s" + re.escape(name) + r'\s*=\s*"[^"]*"')


def get_attr(tag, name):
    m = _attr_re(name).search(tag)
    return re.search(r'"([^"]*)"', m.group(0)).group(1) if m else None


def set_attr(tag, name, value):
    ins = f' {name}="{value}"'
    r = _attr_re(name)
    if r.search(tag):
        return r.sub(lambda _m: ins, tag, count=1)
    if tag.endswith("/>"):
        return tag[:-2].rstrip() + ins + "/>"
    return tag[:-1].rstrip() + ins + ">"


def remove_attr(tag, name):
    return _attr_re(name).sub("", tag, count=1)


def retag_tag(tag, tokens, rules, keep=()):
    """Give one opening-tag string a themed role. Returns (new_tag, class_name|None).

    Four shapes, by what is themable on the element:

      fill only, or fill == stroke   one class, currentColor inline (the original case)
      stroke only                    ditto -- outline-only art, or an accent fill
                                     (#80b3ff, red) that must keep its own colour
      fill and stroke, different     the two-colour element: fill from the class,
                                     stroke inline against the class's `color`
    """
    # style="fill:#000080;..." is the hygiened shape; fill="#000080" also occurs.
    had_style = get_attr(tag, "style") is not None
    sd = style_to_dict(get_attr(tag, "style"))
    fill = sd.get("fill", get_attr(tag, "fill"))
    stroke = sd.get("stroke", get_attr(tag, "stroke"))
    fill_role, stroke_role = role_of(fill, keep), role_of(stroke, keep)

    if not fill_role and not stroke_role:
        return tag, None

    def set_prop(tag, prop, value):
        if prop in sd:
            sd[prop] = value
        else:
            tag = set_attr(tag, prop, value)
        return tag

    two_colour = fill_role and stroke_role and norm(fill) != norm(stroke)

    if two_colour:
        # The class supplies the fill, so the element must NOT declare one: an inline
        # style beats a CSS declaration for the properties it names. Drop it in both
        # spellings, then let `color` reach the stroke.
        sd.pop("fill", None)
        tag = remove_attr(tag, "fill")
        tag = set_prop(tag, "stroke", "currentColor")
        name = class_for([("fill", fill), ("color", stroke)], tokens, rules)
    else:
        # One themed colour: whichever properties carry it become currentColor.
        target = fill if fill_role else stroke
        if fill_role:
            tag = set_prop(tag, "fill", "currentColor")
        if stroke_role and norm(stroke) == norm(target):
            tag = set_prop(tag, "stroke", "currentColor")
        name = class_for([("color", target)], tokens, rules)

    # Inkscape leaves a vestigial `color:#000000` on many paths. It was dead
    # metadata while nothing used currentColor -- but an INLINE style beats a
    # class selector, so it silently overrides .ink/.paper and the icon renders
    # black. Caught by --verify on 13 icons (AreaMove, DatabaseConvert, ...).
    # Safe to drop: no source used currentColor before this transform.
    sd.pop("color", None)
    tag = remove_attr(tag, "color")

    if had_style:
        tag = set_attr(tag, "style", dict_to_style(sd)) if sd else remove_attr(tag, "style")

    cls = get_attr(tag, "class")
    tag = set_attr(tag, "class", f"{cls} {name}".strip() if cls else name)
    return tag, name


def explicit(text):
    """Make every implied black fill explicit. Returns (svg_text, n_fixed).

    Analyse with a parser (fill inheritance needs the tree), then patch the
    ORIGINAL text by id -- so the file keeps its declaration, comments,
    namespaces and formatting, and the diff shows only the attributes added.
    """
    ids = implied_black_ids(ET.fromstring(text))
    if not ids:
        return None, 0
    out = text
    n = 0
    for el_id in ids:
        out, ok = add_fill_by_id(out, el_id)
        n += ok
    return (out, n) if n else (None, 0)


def parse_scheme(text):
    """-> {class_name: {property: colour}} for an existing scheme block, else {}."""
    m = re.search(r'<style\b[^>]*\bid="' + re.escape(STYLE_ID) + r'"[^>]*>(.*?)</style>',
                  text, re.S)
    if not m:
        return {}
    out = {}
    for name, body in re.findall(r"\.([\w-]+)\s*\{([^}]*)\}", m.group(1)):
        decls = {}
        for part in body.split(";"):
            if ":" in part:
                k, v = part.split(":", 1)
                decls[k.strip()] = norm(v)
        out[name] = decls
    return out


def untheme(text):
    """Undo a previous theme() pass, restoring the literal source colours.

    Without this the transform is not re-runnable: theme() drops the scheme block,
    so a second pass over an already-themed icon would strip the rules while
    leaving `class="ink" fill="currentColor"` behind -- and currentColor with no
    colour set renders BLACK. The fill batch and this stroke batch touch the same
    icons, so that is not hypothetical.

    Restores three things per element: a currentColor fill/stroke becomes the
    class's `color` again, a class-supplied `fill` becomes inline again, and the
    role token is removed from the class attribute (a class we did not add stays).
    """
    scheme = parse_scheme(text)
    if not scheme:
        return text

    def repl(m):
        tag = m.group(0)
        classes = (get_attr(tag, "class") or "").split()
        mine = [c for c in classes if c in scheme]
        if not mine:
            return tag
        decls = scheme[mine[-1]]
        sd = style_to_dict(get_attr(tag, "style"))
        had_style = get_attr(tag, "style") is not None

        for prop in ("fill", "stroke"):
            if norm(sd.get(prop, get_attr(tag, prop) or "")) != "currentcolor":
                continue
            lit = decls.get("color")
            if lit is None:
                continue
            if prop in sd:
                sd[prop] = lit
            else:
                tag = set_attr(tag, prop, lit)
        # A fill served from the class has to come back as an inline one, or the
        # colour is lost with the rule.
        if "fill" in decls:
            if had_style or sd:
                sd["fill"] = decls["fill"]
            else:
                tag = set_attr(tag, "fill", decls["fill"])

        if had_style or sd:
            tag = set_attr(tag, "style", dict_to_style(sd)) if sd else remove_attr(tag, "style")
        # Blank the attribute rather than delete it, so theme() rewrites it IN PLACE.
        # Deleting and re-appending moves it to the end of the tag and rstrips the
        # newline before it -- a formatting churn on every re-themed element, which
        # buries the real change and hurts the file for editing in Inkscape.
        # A leftover class="" is swept up at the end of theme().
        rest = [c for c in classes if c not in scheme]
        return set_attr(tag, "class", " ".join(rest))

    text = TAG_RE.sub(repl, text)
    # Same reasoning for the block: leave a marker where it was, so the replacement
    # keeps its indentation instead of being dropped in after <svg> as a new line.
    return re.sub(r'<style\b[^>]*\bid="' + re.escape(STYLE_ID) + r'"[^>]*>.*?</style>',
                  SCHEME_MARK, text, flags=re.S)


def keep_for(stem):
    """Colours this icon must leave literal. See palette.KEEP_BLUE."""
    return MARK if stem in KEEP_BLUE else frozenset()


def bare(text):
    """untheme() for standalone use: no scheme marker, no empty class attributes.

    untheme() leaves both behind on purpose -- theme() replaces the marker in place and
    sweeps the empty classes -- so an opt-out icon, which never reaches theme(), needs
    them cleaned up here.
    """
    out = untheme(text).replace(SCHEME_MARK, "")
    return re.sub(r'\s+class=""', "", out)


def theme(text, remap=None, keep=()):
    """Mark up one icon by editing the ORIGINAL text (no reserialise).

    remap: optional {source_color: themed_color}. Omit it and each class keeps
    its ORIGINAL colour as its default -- which must render identically to the
    untransformed icon. That identity is the gate; see --verify.

    Returns (svg_text, tokens) where tokens is {source_color: role_token},
    or (None, {}) when the icon has nothing themable.
    """
    # Back to literal colours first, so a re-run re-derives every role from the art
    # rather than stacking scheme blocks or orphaning last pass's classes.
    text = untheme(text)

    tokens = {}
    rules = {}

    def repl(m):
        new_tag, _ = retag_tag(m.group(0), tokens, rules, keep)
        return new_tag

    out = TAG_RE.sub(repl, text)
    if not tokens:
        return None, {}

    def value(src):
        return (remap or {}).get(norm(src), src)

    body = "".join(
        f".{name}{{" + ";".join(f"{prop}:{value(src)}" for prop, src in decls) + "}"
        for name, decls in rules.items())
    style = f'<style id="{STYLE_ID}">{body}</style>'

    out = re.sub(r'\s+class=""', "", out)

    # Back where the old block was, or -- first time round -- as the first child of <svg>.
    if SCHEME_MARK in out:
        return out.replace(SCHEME_MARK, style), tokens
    m = re.search(r"<svg\b[^>]*?>", out, re.S)
    if not m:
        return None, {}
    return out[:m.end()] + style + out[m.end():], tokens


def inheritance_conflicts(text):
    """Shapes whose inherited currentColor would resolve against the WRONG colour.

    The one thing a per-tag text edit cannot see. `fill` is inherited, and an
    inherited `fill:currentColor` resolves against the colour of the element that
    USES it -- so a shape inheriting a themed fill from <g class="paper"> while
    carrying its own class="ink" silently flips from paper to ink.

    Measured zero across the set today (154 shapes inherit a fill, none with a
    class of their own), which is why the transform can stay a text patch. It is
    not a property of the format though, only of these icons -- so assert it
    rather than assume it, because the next role added is what would break it.

    Analysed with a real parser, on purpose: inheritance needs the tree. Only the
    EDITING stays textual -- reserialising rewrites the whole document (see
    add_fill_by_id).
    """
    scheme = parse_scheme(text)
    if not scheme:
        return []
    out = []

    def own(el, prop):
        for part in (el.get("style") or "").split(";"):
            if part.strip().startswith(prop + ":"):
                return norm(part.split(":", 1)[1])
        return norm(el.get(prop) or "") or None

    def walk(el, inherited_fill):
        classes = [c for c in (el.get("class") or "").split() if c in scheme]
        fill = own(el, "fill")
        if classes and not fill and "fill" in scheme[classes[-1]]:
            fill = scheme[classes[-1]]["fill"]
        if el.tag in SHAPES and not fill and inherited_fill == "currentcolor" and classes:
            out.append(el.get("id") or "<no id>")
        for child in el:
            walk(child, fill or inherited_fill)

    walk(ET.fromstring(text), None)
    return out


def render(svg_text, size, tag):
    """Render svg source to an RGBA image via inkscape."""
    import subprocess
    import tempfile

    from PIL import Image

    with tempfile.TemporaryDirectory() as td:
        src = os.path.join(td, f"{tag}.svg")
        out = os.path.join(td, f"{tag}.png")
        with open(src, "w", encoding="utf-8") as fh:
            fh.write(svg_text)
        r = subprocess.run(
            ["inkscape", "-D", "-w", str(size), "-h", str(size), src,
             "--export-type=png", f"--export-filename={out}"],
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if r.returncode != 0 or not os.path.exists(out):
            return None
        return Image.open(out).convert("RGBA").copy()


def verify_one(args):
    """The gate: themed-with-own-defaults must render exactly like the original."""
    path, size = args
    with open(path, encoding="utf-8") as fh:
        src = fh.read()
    out, classes = theme(src, keep=keep_for(os.path.basename(path)[:-4]))
    if out is None:
        return (os.path.basename(path), "skip", 0.0)

    conflicts = inheritance_conflicts(out)
    if conflicts:
        return (os.path.basename(path), "INHERIT-CONFLICT " + ",".join(conflicts[:3]), 0.0)

    a = render(src, size, "orig")
    b = render(out, size, "themed")
    if a is None or b is None:
        return (os.path.basename(path), "RENDER-FAILED", 0.0)

    pa, pb = a.load(), b.load()
    w, h = a.size
    worst = 0.0
    for y in range(h):
        for x in range(w):
            ca, cb = pa[x, y], pb[x, y]
            # premultiply: rgb is undefined where alpha=0
            aa, ab = ca[3] / 255, cb[3] / 255
            for i in range(3):
                worst = max(worst, abs(ca[i] * aa - cb[i] * ab))
            worst = max(worst, abs(ca[3] - cb[3]))
    return (os.path.basename(path), "ok" if worst <= 1 else "CHANGED", worst)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dir", default=os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
    ap.add_argument("--write", action="store_true", help="rewrite in place")
    ap.add_argument("--verify", action="store_true",
                    help="prove the light render does not move (renders every icon twice)")
    ap.add_argument("--explicit", action="store_true",
                    help="write down implied black fills (the black-arrowhead fix)")
    ap.add_argument("--size", type=int, default=48)
    args = ap.parse_args()

    svgs = sorted(f for f in os.listdir(args.dir) if f.endswith(".svg"))
    if not svgs:
        sys.exit(f"no svgs in {args.dir}")

    if args.explicit:
        total = files = 0
        for name in svgs:
            path = os.path.join(args.dir, name)
            with open(path, encoding="utf-8") as fh:
                src = fh.read()
            out, n = explicit(src)
            if not n:
                continue
            total += n
            files += 1
            print(f"  {n:3} implied black fill(s)  {name}")
            if args.write:
                with open(path, "w", encoding="utf-8") as fh:
                    fh.write(out)
        print(f"\n{total} shapes in {files} icons rely on SVG's implicit black default")
        if not args.write:
            print("(dry run -- nothing written; add --write)")
        return

    if args.verify:
        from concurrent.futures import ProcessPoolExecutor
        paths = [(os.path.join(args.dir, f), args.size) for f in svgs if f[:-4] not in OPTOUT]
        with ProcessPoolExecutor() as ex:
            rows = list(ex.map(verify_one, paths))
        bad = [r for r in rows if r[1] not in ("ok", "skip")]
        for name, status, worst in bad:
            print(f"{status:14} worst={worst:6.1f}  {name}")
        ok = sum(1 for r in rows if r[1] == "ok")
        skip = sum(1 for r in rows if r[1] == "skip")
        print(f"\n{ok} identical | {skip} nothing to theme | {len(bad)} CHANGED")
        sys.exit(1 if bad else 0)

    touched = multi = untouched = optout = 0
    shades = {}
    for name in svgs:
        if name[:-4] in OPTOUT:
            optout += 1
            # Strip any markup a previous run left behind, so adding a name to OPTOUT
            # actually un-themes it. Skipping alone would leave the icon themed forever.
            path = os.path.join(args.dir, name)
            with open(path, encoding="utf-8") as fh:
                src = fh.read()
            stripped = bare(src)
            if stripped != src:
                print(f"  un-themed (opt-out)  {name}")
                if args.write:
                    with open(path, "w", encoding="utf-8") as fh:
                        fh.write(stripped)
            continue
        path = os.path.join(args.dir, name)
        with open(path, encoding="utf-8") as fh:
            src = fh.read()
        out, classes = theme(src, keep=keep_for(name[:-4]))
        if out is None:
            untouched += 1
            continue
        touched += 1
        if len(classes) > 2:
            multi += 1
        for src_color, cls in classes.items():
            shades[src_color] = shades.get(src_color, 0) + 1
        if args.write:
            with open(path, "w", encoding="utf-8") as fh:
                fh.write(out)

    print(f"{len(svgs)} icons")
    print(f"  themed          {touched}")
    print(f"    3+ classes    {multi}")
    print(f"  opt-out         {optout}   <- self-backgrounded; left un-themed (palette.py:OPTOUT)")
    print(f"  untouched       {untouched}   <- no navy, no paper: nothing to theme")
    print("\nsource colour -> class, by icon count:")
    for c, n in sorted(shades.items(), key=lambda kv: -kv[1]):
        print(f"  {c}  {n}")
    if not args.write:
        print("\n(dry run -- nothing written; pass --write to migrate)")


if __name__ == "__main__":
    main()
