#!/usr/bin/env python3
"""Build the phase-7a review sheet: today's icon vs the themed one on dark.

Three columns per icon:

  1. original on light   -- what users see today, the reference
  2. original on dark    -- the problem: navy #000080 melting into #353535
  3. THEMED on dark      -- what the currentColor rule produces

Column 3 is the point. A sheet of raw icons on dark only proves "navy is
invisible" 329 times; what needs judging is the RULE, so the comparison has to
be original-on-light beside recoloured-on-dark.

  visibility = alpha-weighted mean |luma(icon over ground) - luma(ground)|,
               0..255. Low means the ink melts into that ground.

The rule: swap the NEUTRAL palette (house ink #000080/#000000 and the light
shades they sit on) by inverting HSL lightness, which keeps hue -- navy stays
blue, just light. Everything else is semantic and never moves, so red stays red
and green stays green. Override the ink with --ink '#rrggbb'.

Preview only -- writes nothing but the html. It substitutes colours directly
rather than requiring marked-up sources; see the plan's Phase 7a for the
markup-vs-substitution decision. Either route renders the same picture.

The svg is embedded (metadata stripped) rather than rasterized, so the page stays
zoomable -- what a reviewer needs when judging a 16px icon.

Requires: inkscape 1.x, python3-pil.
"""

import argparse
import base64
import colorsys
import json
import os
import re
import subprocess
import sys
import tempfile
from concurrent.futures import ProcessPoolExecutor

from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from palette import OPTOUT, normalise  # noqa: E402
from themesvg import explicit  # noqa: E402


# Qt Fusion's window colours, near enough for judging.
LIGHT = (239, 239, 239)
DARK = (53, 53, 53)
RENDER = 96

STRIP = (
    (re.compile(r"<metadata\b.*?</metadata>", re.S), ""),
    (re.compile(r"<!--.*?-->", re.S), ""),
    (re.compile(r"<\?xml\b.*?\?>", re.S), ""),
    (re.compile(r"\s*\n\s*"), " "),
)


def luma(rgb):
    r, g, b = rgb
    return 0.2126 * r + 0.7152 * g + 0.0722 * b


def visibility(img, ground):
    """How far the icon's own INK strays from its ground, 0..255.

    Alpha-weighted, NOT a plain mean over the tile: averaging over every pixel
    scores sparse line art (Grid: 6 hairlines on transparency) as invisible no
    matter how bright the lines are, because most of the tile is empty. That is
    a property of the drawing, not of its readability. Weighting by alpha asks
    the question that matters: where there IS ink, does it stand out?
    """
    bg = Image.new("RGBA", img.size, ground + (255,))
    comp = Image.alpha_composite(bg, img).convert("RGB")
    gl = luma(ground)
    total = weight = 0.0
    for p, a in zip(comp.getdata(), (q[3] for q in img.getdata())):
        w = a / 255
        total += abs(luma(p) - gl) * w
        weight += w
    return total / weight if weight else 0.0


def minify(text):
    for pat, rep in STRIP:
        text = pat.sub(rep, text)
    return text.strip()


def flip_lightness(hexcolor):
    """Invert HSL lightness, keep hue+saturation. Navy -> light blue, not white."""
    h = hexcolor.lstrip("#")
    r, g, b = (int(h[i:i + 2], 16) / 255 for i in (0, 2, 4))
    hh, ll, ss = colorsys.rgb_to_hls(r, g, b)
    r2, g2, b2 = colorsys.hls_to_rgb(hh, 1.0 - ll, ss)
    return "#{:02x}{:02x}{:02x}".format(round(r2 * 255), round(g2 * 255), round(b2 * 255))


# The neutral palette: house ink + the light shades it sits on. Everything else
# (red, green, the mime-type hues) is semantic and must never move.
THEMED = ["#000080", "#000000", "#ffffff", "#ececec", "#f9f9f9", "#cccccc"]

COLOR_RE = re.compile(r"#[0-9a-fA-F]{6}")


def substitute(text, mapping):
    """Swap the neutral palette, byte-wise. Fill and stroke alike, no markup.

    The engine rewrites the svg anyway, so it can remap colours directly instead
    of requiring every source to be marked up. Same picture either way; this
    route needs no migration. What it gives up is per-icon intent -- a white that
    is genuinely semantic (TrkProfile's backdrop) gets swapped along with every
    white that is paper.
    """
    def repl(m):
        return mapping.get(m.group(0).lower(), m.group(0))

    return COLOR_RE.sub(repl, text)


def dark_remap(ink_override):
    """Each neutral's dark-scheme colour, derived from its light one."""
    out = {}
    for src in THEMED:
        if ink_override and src == "#000080":
            out[src] = ink_override
        else:
            out[src] = flip_lightness(src)
    return out


def render_png(path_or_text, td, tag, is_text):
    src = os.path.join(td, f"{tag}.svg")
    out = os.path.join(td, f"{tag}.png")
    if is_text:
        with open(src, "w", encoding="utf-8") as fh:
            fh.write(path_or_text)
    else:
        src = path_or_text
    r = subprocess.run(
        ["inkscape", "-D", "-w", str(RENDER), "-h", str(RENDER), src,
         "--export-type=png", f"--export-filename={out}"],
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    if r.returncode != 0 or not os.path.exists(out):
        return None
    return Image.open(out).convert("RGBA").copy()


def pixdiff(a, b):
    """Mean premultiplied RGB difference, 0..255 -- how far the light look moved.

    Premultiplied because rgb is undefined where alpha=0; comparing it raw once
    reported 140/203 changed when the truth was 0.
    """
    if a is None or b is None or a.size != b.size:
        return None
    pa, pb = a.load(), b.load()
    w, h = a.size
    total = 0.0
    for y in range(h):
        for x in range(w):
            ca, cb = pa[x, y], pb[x, y]
            aa, ab = ca[3] / 255, cb[3] / 255
            total += sum(abs(ca[i] * aa - cb[i] * ab) for i in range(3)) / 3
    return total / (w * h)


def uri_of(text):
    return "data:image/svg+xml;base64," + base64.b64encode(minify(text).encode()).decode()


def one(args):
    svg, ink_override = args
    name = os.path.basename(svg)[:-4]
    with open(svg, "r", encoding="utf-8") as fh:
        raw = fh.read()

    # Self-backgrounded icons opt out of BOTH passes: their content sits on their
    # own background, not the app's, so normalising or theming it inverts the
    # relationship (MimeDemWCS = white content on #326480 -> black on mid-blue).
    optout = name in OPTOUT
    norm_text = raw if optout else normalise(raw)

    mapping = dark_remap(ink_override)
    # Implied black fills must be written down first: a colour that was never
    # written down cannot be remapped (this was the black-arrowhead bug).
    fixed, _ = explicit(norm_text)
    themed_text = raw if optout else substitute(fixed or norm_text, mapping)

    with tempfile.TemporaryDirectory() as td:
        img = render_png(svg, td, "orig", False)
        if img is None:
            return None
        nimg = render_png(norm_text, td, "norm", True) if norm_text != raw else img
        timg = render_png(themed_text, td, "themed", True) if themed_text != raw else img

        vis_l = visibility(img, LIGHT)
        vis_t = visibility(timg, DARK) if timg else None
        moved = pixdiff(img, nimg) if norm_text != raw else 0.0

    return {
        "name": name,
        "uri": uri_of(raw),
        "nuri": uri_of(norm_text) if norm_text != raw else "",
        "turi": uri_of(themed_text) if themed_text != raw else "",
        "light": round(vis_l, 1),
        "themed": round(vis_t, 1) if vis_t is not None else None,
        "moved": round(moved, 1) if moved is not None else 0.0,
        "optout": optout,
    }


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dir", default="src/icons")
    ap.add_argument("--out", required=True, help="html file to write")
    ap.add_argument("--ink", default="", help="dark-scheme ink instead of the lightness flip")
    args = ap.parse_args()

    svgs = sorted(
        os.path.join(args.dir, f) for f in os.listdir(args.dir) if f.endswith(".svg"))
    if not svgs:
        sys.exit(f"no svgs in {args.dir}")

    with ProcessPoolExecutor() as ex:
        rows = [r for r in ex.map(one, [(s, args.ink) for s in svgs]) if r]

    rows.sort(key=lambda r: -(r["moved"] or 0))
    ink = args.ink or flip_lightness("#000080")
    html = (TEMPLATE
            .replace("/*DATA*/", json.dumps(rows, separators=(",", ":")))
            .replace("INK_COLOR", ink))
    with open(args.out, "w", encoding="utf-8") as fh:
        fh.write(html)

    optout = [r for r in rows if r["optout"]]
    moved = [r for r in rows if (r["moved"] or 0) > 0]
    big = [r for r in rows if (r["moved"] or 0) >= 12]
    still = [r for r in rows if r["themed"] is not None and r["themed"] < 20]
    print(f"{len(rows)} icons -> {args.out} ({len(html)/1e6:.1f} MB)")
    print(f"ink on dark: {ink}")
    print(f"light look moved on {len(moved)} icons ({len(big)} by >=12) | "
          f"{len(optout)} opted out | {len(still)} still melting on dark")
    if big:
        print("moved most: " + ", ".join(f"{r['name']}({r['moved']:.0f})" for r in big[:8]))


TEMPLATE = r"""<title>QMapShack UI icons — light / dark review</title>
<style>
  :root {
    --ink: #14161c; --dim: #5c6273; --line: #dcdfe8; --page: #f7f8fa;
    --card: #ffffff; --navy: #000080; --warn: #b3261e; --ok: #1a7f37;
    --mono: ui-monospace, "SF Mono", "Cascadia Mono", Menlo, Consolas, monospace;
    --sans: ui-sans-serif, system-ui, "Segoe UI", Roboto, sans-serif;
  }
  @media (prefers-color-scheme: dark) {
    :root {
      --ink: #e6e8ef; --dim: #949cb0; --line: #2c3040; --page: #12141a;
      --card: #191c24; --navy: #93a7ff; --warn: #ff8a80; --ok: #5ec269;
    }
  }
  :root[data-theme="dark"] {
    --ink: #e6e8ef; --dim: #949cb0; --line: #2c3040; --page: #12141a;
    --card: #191c24; --navy: #93a7ff; --warn: #ff8a80; --ok: #5ec269;
  }
  :root[data-theme="light"] {
    --ink: #14161c; --dim: #5c6273; --line: #dcdfe8; --page: #f7f8fa;
    --card: #ffffff; --navy: #000080; --warn: #b3261e; --ok: #1a7f37;
  }
  body {
    margin: 0; background: var(--page); color: var(--ink);
    font-family: var(--sans); line-height: 1.5;
  }
  .wrap { max-width: 1100px; margin: 0 auto; padding: 32px 20px 80px; }
  header h1 {
    font-size: 1.5rem; margin: 0 0 6px; letter-spacing: -0.01em;
    text-wrap: balance;
  }
  header p { margin: 0 0 4px; color: var(--dim); max-width: 65ch; font-size: .92rem; }
  .bar {
    position: sticky; top: 0; z-index: 5; background: var(--page);
    border-bottom: 1px solid var(--line); padding: 14px 0 12px; margin-top: 22px;
    display: flex; gap: 10px; flex-wrap: wrap; align-items: center;
  }
  input[type=search], select {
    font: inherit; font-size: .85rem; color: var(--ink); background: var(--card);
    border: 1px solid var(--line); border-radius: 6px; padding: 6px 9px;
  }
  input[type=search] { min-width: 190px; }
  :focus-visible { outline: 2px solid var(--navy); outline-offset: 2px; }
  .grp { display: flex; gap: 0; border: 1px solid var(--line); border-radius: 6px; overflow: hidden; }
  .grp button {
    font: inherit; font-size: .82rem; padding: 6px 11px; border: 0; cursor: pointer;
    background: var(--card); color: var(--dim); border-right: 1px solid var(--line);
  }
  .grp button:last-child { border-right: 0; }
  .grp button[aria-pressed="true"] { background: var(--navy); color: #fff; }
  .count { margin-left: auto; color: var(--dim); font-size: .82rem; font-variant-numeric: tabular-nums; }
  .scroll { overflow-x: auto; }
  table { border-collapse: collapse; width: 100%; margin-top: 4px; }
  thead th {
    text-align: left; font-size: .68rem; text-transform: uppercase; letter-spacing: .08em;
    color: var(--dim); font-weight: 600; padding: 12px 8px; border-bottom: 1px solid var(--line);
    white-space: nowrap;
  }
  tbody td { border-bottom: 1px solid var(--line); padding: 5px 8px; vertical-align: middle; }
  tbody tr:hover { background: color-mix(in srgb, var(--navy) 6%, transparent); }
  .name { font-family: var(--mono); font-size: .8rem; white-space: nowrap; }
  .cell { width: 1%; }
  .tile {
    display: grid; place-items: center; width: 76px; height: 60px; border-radius: 5px;
  }
  .tile.l { background: #efefef; }
  .tile.d { background: #353535; }
  .tile img { display: block; image-rendering: auto; }
  .score {
    font-family: var(--mono); font-size: .76rem; text-align: right;
    font-variant-numeric: tabular-nums; color: var(--dim); width: 1%;
  }
  .score.bad { color: var(--warn); font-weight: 700; }
  .score.good { color: var(--ok); }
  .badge {
    display: inline-block; margin-left: 8px; padding: 1px 6px; border-radius: 9px;
    font-family: var(--sans); font-size: .62rem; letter-spacing: .04em;
    background: var(--line); color: var(--dim); vertical-align: 1px;
  }
  code.swatch {
    padding: 1px 5px; border-radius: 4px; border: 1px solid var(--line);
    background: var(--card);
  }
  /* column 3 is the verdict column -- mark it so the eye lands there */
  tbody td:nth-child(4) .tile, thead th:nth-child(4) {
    box-shadow: inset 0 0 0 1px color-mix(in srgb, var(--navy) 45%, transparent);
  }
  thead th:nth-child(4) { color: var(--navy); }
  footer { margin-top: 28px; color: var(--dim); font-size: .8rem; max-width: 65ch; }
  code { font-family: var(--mono); font-size: .85em; }

  /* review: flag an icon, add a note, export the lot */
  .flag {
    font: inherit; font-size: .95rem; line-height: 1; cursor: pointer;
    background: none; border: 1px solid var(--line); border-radius: 6px;
    color: var(--dim); padding: 4px 7px;
  }
  .flag[aria-pressed="true"] { background: var(--warn); border-color: var(--warn); color: #fff; }
  .note {
    font: inherit; font-size: .78rem; font-family: var(--mono); margin-left: 6px;
    width: 210px; padding: 4px 7px; border-radius: 6px; color: var(--ink);
    background: var(--card); border: 1px solid var(--line);
  }
  .note[hidden] { display: none; }
  tr.flagged { background: color-mix(in srgb, var(--warn) 9%, transparent); }
  tr.flagged td { border-bottom-color: color-mix(in srgb, var(--warn) 35%, var(--line)); }
  .act {
    font: inherit; font-size: .82rem; padding: 6px 11px; cursor: pointer;
    border: 1px solid var(--line); border-radius: 6px;
    background: var(--card); color: var(--ink);
  }
  .act.primary { background: var(--navy); border-color: var(--navy); color: #fff; }
  .act:disabled { opacity: .45; cursor: default; }
  dialog {
    border: 1px solid var(--line); border-radius: 10px; background: var(--card);
    color: var(--ink); padding: 18px; max-width: 640px; width: 90vw;
  }
  dialog::backdrop { background: rgba(0,0,0,.5); }
  dialog h2 { margin: 0 0 8px; font-size: 1rem; }
  dialog p { margin: 0 0 10px; color: var(--dim); font-size: .84rem; }
  dialog textarea {
    width: 100%; height: 260px; font-family: var(--mono); font-size: .76rem;
    border: 1px solid var(--line); border-radius: 8px; padding: 10px;
    background: var(--page); color: var(--ink); resize: vertical;
  }
  .dlgbar { display: flex; gap: 8px; margin-top: 10px; justify-content: flex-end; }
</style>

<div class="wrap">
  <header>
    <h1>Phase 7a — accent normalisation: did we break the light look?</h1>
    <p><strong>The risk has moved to the light side.</strong> Normalising the accents
       so one value per hue works on both grounds deliberately changes the look you
       already know: <code>#ff0000</code>&nbsp;→&nbsp;<code>#ff5555</code>,
       six greens&nbsp;→&nbsp;<code>#55d400</code>,
       <code>#0000ff</code>&nbsp;→&nbsp;<code>#5555ff</code>. Compare columns 2 and 3
       and say where that is too much. Sorted by how far each icon moved.</p>
    <p>Column&nbsp;4 is the payoff: the same icon themed on dark, ink <code
       class="swatch">INK_COLOR</code>. The 21 icons carrying their own coloured
       background (<code>Mime*</code>, <code>*Project</code>, <code>Off</code>) are
       marked <em>opt-out</em> and left alone entirely — your <code>MimeDemWCS</code>
       point.</p>
    <p><strong>Visibility</strong> is the alpha-weighted mean luma distance between
       the ink and its ground, 0–255. Under ~20 it is melting in. A ranking aid, not
       a verdict — trust your eyes.</p>
    <p><strong>Found one that needs work?</strong> Hit ⚑ on its row and say what's
       wrong. Then <em>Copy report</em> and paste it back into the conversation.
       Flags are kept in this browser, so you can stop and come back.</p>
  </header>

  <div class="bar">
    <input type="search" id="q" placeholder="Filter by name…" aria-label="Filter icons by name">
    <div class="grp" role="group" aria-label="Icon size">
      <button data-size="16">16</button>
      <button data-size="24">24</button>
      <button data-size="32" aria-pressed="true">32</button>
      <button data-size="48">48</button>
    </div>
    <select id="sort" aria-label="Sort order">
      <option value="moved">Light look moved most</option>
      <option value="themed">Worst on dark after theming</option>
      <option value="light">Worst on light today</option>
      <option value="flagged">Flagged first</option>
      <option value="name">Name (A–Z)</option>
    </select>
    <button class="act primary" id="export" disabled>Copy report</button>
    <span class="count" id="count"></span>
  </div>

  <div class="scroll">
    <table>
      <thead>
        <tr>
          <th>Icon</th>
          <th>Today, light</th>
          <th>Normalised, light</th>
          <th>Themed, dark</th>
          <th style="text-align:right">Light moved</th>
          <th style="text-align:right">Vis. dark</th>
          <th>Needs work?</th>
        </tr>
      </thead>
      <tbody id="rows"></tbody>
    </table>
  </div>

  <dialog id="dlg">
    <h2>Review report</h2>
    <p>Paste this back into the conversation. Flags and notes are saved in this
       browser, so you can close the page and carry on later.</p>
    <textarea id="out" readonly></textarea>
    <div class="dlgbar">
      <button class="act" id="clear">Clear all flags</button>
      <button class="act primary" id="close">Done</button>
    </div>
  </dialog>

  <footer>
    Generated by <code>.notes/icons-tools/contactsheet.py</code> (markup transform:
    <code>themesvg.py</code>). Icons are the live SVG sources, embedded — zoom in,
    they stay crisp. Re-run with <code>--ink '#rrggbb'</code> to try a different ink.
  </footer>
</div>

<script>
const DATA = /*DATA*/;
const INK = "INK_COLOR";
let size = 32, sortBy = "moved", query = "";
const rowsEl = document.getElementById("rows");
const countEl = document.getElementById("count");
const exportEl = document.getElementById("export");

// Flags live in the browser: the review spans sessions, and losing it to a
// stray reload would mean doing all 329 again.
const KEY = "qms-icon-review";
let review = {};
try { review = JSON.parse(localStorage.getItem(KEY) || "{}"); } catch (e) { review = {}; }

function save() {
  try { localStorage.setItem(KEY, JSON.stringify(review)); } catch (e) { /* private mode */ }
  const n = Object.keys(review).length;
  exportEl.disabled = n === 0;
  exportEl.textContent = n ? `Copy report (${n})` : "Copy report";
}

function report() {
  const names = Object.keys(review).sort();
  const lines = [
    `Icon review — ink ${INK}, ${names.length} of ${DATA.length} need work`,
    "",
    ...names.map(n => `- ${n}${review[n] ? ` — ${review[n]}` : ""}`),
  ];
  return lines.join("\n");
}

function tile(uri, cls, alt) {
  const td = document.createElement("td");
  td.className = "cell";
  const box = document.createElement("div");
  box.className = "tile " + cls;
  if (uri) {
    const img = document.createElement("img");
    img.src = uri;
    img.width = size; img.height = size;
    img.alt = alt;
    img.loading = "lazy";
    box.append(img);
  }
  td.append(box);
  return td;
}

function score(v, bad) {
  const td = document.createElement("td");
  td.className = "score" + (v !== null && bad(v) ? " bad" : "");
  td.textContent = v === null ? "—" : v.toFixed(1);
  return td;
}

function render() {
  const q = query.trim().toLowerCase();
  let list = DATA.filter(d => !q || d.name.toLowerCase().includes(q));
  const last = Number.MAX_SAFE_INTEGER;
  const cmp = {
    moved: (a, b) => (b.moved ?? 0) - (a.moved ?? 0),
    themed: (a, b) => (a.themed ?? last) - (b.themed ?? last),
    light: (a, b) => a.light - b.light,
    flagged: (a, b) => (b.name in review) - (a.name in review) || a.name.localeCompare(b.name),
    name: (a, b) => a.name.localeCompare(b.name),
  }[sortBy];
  list = [...list].sort(cmp);

  rowsEl.replaceChildren(...list.map(d => {
    const tr = document.createElement("tr");

    const nm = document.createElement("td");
    nm.className = "name";
    nm.textContent = d.name;
    if (d.optout) {
      nm.append(badge("opt-out", "Carries its own coloured background — neither normalised nor themed"));
    } else if (!d.turi) {
      nm.append(badge("no ink", "Nothing themable in this icon"));
    }
    tr.append(nm);

    tr.append(tile(d.uri, "l", d.name + " today, on light"));
    tr.append(tile(d.nuri || d.uri, "l", d.name + " normalised, on light"));
    tr.append(tile(d.turi || d.uri, "d", d.name + " themed, on dark"));

    const mv = document.createElement("td");
    mv.className = "score" + (d.moved >= 12 ? " bad" : "");
    mv.textContent = d.moved ? d.moved.toFixed(1) : "—";
    mv.title = "Mean pixel shift on LIGHT vs today. 0 = the familiar look is untouched.";
    tr.append(mv);

    tr.append(score(d.themed, v => v < 20));
    tr.append(reviewCell(d, tr));

    if (d.name in review) { tr.classList.add("flagged"); }
    return tr;
  }));
  countEl.textContent = list.length + " of " + DATA.length;
}

function reviewCell(d, tr) {
  const td = document.createElement("td");

  const btn = document.createElement("button");
  btn.className = "flag";
  btn.textContent = "⚑";
  btn.title = "Flag this icon as needing work";
  btn.setAttribute("aria-pressed", String(d.name in review));
  btn.setAttribute("aria-label", "Flag " + d.name);

  const note = document.createElement("input");
  note.className = "note";
  note.type = "text";
  note.placeholder = "what's wrong with it?";
  note.value = review[d.name] || "";
  note.hidden = !(d.name in review);
  note.setAttribute("aria-label", "Note for " + d.name);

  btn.addEventListener("click", () => {
    if (d.name in review) {
      delete review[d.name];
      btn.setAttribute("aria-pressed", "false");
      note.hidden = true;
      tr.classList.remove("flagged");
    } else {
      review[d.name] = note.value || "";
      btn.setAttribute("aria-pressed", "true");
      note.hidden = false;
      tr.classList.add("flagged");
      note.focus();
    }
    save();
  });

  note.addEventListener("input", () => { review[d.name] = note.value; save(); });

  td.append(btn, note);
  return td;
}

function badge(text, title) {
  const s = document.createElement("span");
  s.className = "badge";
  s.textContent = text;
  s.title = title;
  return s;
}

document.getElementById("q").addEventListener("input", e => { query = e.target.value; render(); });
document.getElementById("sort").addEventListener("change", e => { sortBy = e.target.value; render(); });
document.querySelectorAll(".grp button").forEach(b => {
  b.addEventListener("click", () => {
    size = +b.dataset.size;
    document.querySelectorAll(".grp button").forEach(o =>
      o.setAttribute("aria-pressed", String(o === b)));
    render();
  });
});

const dlg = document.getElementById("dlg");
const out = document.getElementById("out");
exportEl.addEventListener("click", async () => {
  out.value = report();
  dlg.showModal();
  out.select();
  try { await navigator.clipboard.writeText(out.value); } catch (e) { /* select+copy */ }
});
document.getElementById("close").addEventListener("click", () => dlg.close());
document.getElementById("clear").addEventListener("click", () => {
  if (!confirm("Clear all flags and notes?")) { return; }
  review = {};
  save();
  dlg.close();
  render();
});

save();
render();
</script>
"""

if __name__ == "__main__":
    main()
