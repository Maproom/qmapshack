#!/usr/bin/env python3
"""Icon review sheet -- every UI icon in light AND dark, themed exactly as CSvgtIconEngine,
at a selectable pixel size, with a name filter and a per-icon flag+comment collector you
can export.

No rasterising: each icon is embedded as a light and a dark SVG data URI and the browser
scales them, so switching size is instant and stays crisp. Colours come from the engine's
fixed role palette, so the dark column is what QMapShack actually draws -- not a guess.

  ./contactsheet.py --dir ../../src/icons --out /tmp/iconsheet.html

engine_recolor / ENGINE_LIGHT / ENGINE_DARK are importable -- other tools theme with them.
"""
import argparse
import base64
import json
import os
import re
import sys

# CSvgtIconEngine::roleColor -- the fixed light/dark pair per role. Light == authored.
ENGINE_LIGHT = {"paper": "#ffffff", "lead": "#000000", "mark": "#0000ff", "ink": "#000080"}
ENGINE_DARK = {"paper": "#353535", "lead": "#e0e0e0", "mark": "#66aaff", "ink": "#ccccff"}

SCHEME_RE = re.compile(r'(<style\b[^>]*\bid="current-color-scheme"[^>]*>)(.*?)(</style>)', re.S)
RULE_RE = re.compile(r"\.([A-Za-z0-9_-]+)\s*\{([^}]*)\}")
STRIP = [
    (re.compile(r"<metadata\b.*?</metadata>", re.S), ""),
    (re.compile(r"<!--.*?-->", re.S), ""),
    (re.compile(r"<\?xml\b.*?\?>", re.S), ""),
]


def role_color(token, pal):
    """A '-'-token names its role by prefix: paper*/lead*/mark*, anything else -> ink."""
    for role in ("paper", "lead", "mark"):
        if token.startswith(role):
            return pal[role]
    return pal["ink"]


def engine_recolor(svg, palette):
    """Recolour the current-color-scheme block by role, exactly as CSvgtIconEngine does.

    An icon without the block is un-themed and returned as-is -- which is also what the
    engine does with it (the fixed-appearance OPTOUT icons render identically in both).
    """
    m = SCHEME_RE.search(svg)
    if not m:
        return svg

    def fix_rule(rm):
        cls, body = rm.group(1), rm.group(2)
        roles = cls.split("-")
        out, i = [], 0
        for decl in (d for d in body.split(";") if ":" in d):
            prop = decl.split(":", 1)[0].strip()
            if i < len(roles):
                out.append(f"{prop}:{role_color(roles[i], palette)}")
                i += 1
            else:
                out.append(decl.strip())
        return f".{cls}{{{';'.join(out)}}}"

    rules = RULE_RE.sub(fix_rule, m.group(2))
    return svg[:m.start(2)] + rules + svg[m.end(2):]


def uri(svg, palette):
    s = engine_recolor(svg, palette)
    for pat, rep in STRIP:
        s = pat.sub(rep, s)
    return "data:image/svg+xml;base64," + base64.b64encode(s.strip().encode()).decode()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dir", default=os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
    ap.add_argument("--out", required=True, help="html file to write")
    args = ap.parse_args()

    svgs = sorted(f for f in os.listdir(args.dir) if f.endswith(".svg"))
    if not svgs:
        sys.exit(f"no svgs in {args.dir}")

    data = []
    for f in svgs:
        raw = open(os.path.join(args.dir, f), encoding="utf-8").read()
        data.append({"n": f[:-4], "l": uri(raw, ENGINE_LIGHT), "d": uri(raw, ENGINE_DARK)})

    html = TEMPLATE.replace("/*DATA*/", json.dumps(data, separators=(",", ":")))
    with open(args.out, "w", encoding="utf-8") as fh:
        fh.write(html)
    print(f"{len(data)} icons -> {args.out} ({os.path.getsize(args.out) / 1e6:.1f} MB)")


TEMPLATE = r"""<!doctype html><html lang=en><head><meta charset=utf-8>
<meta name=viewport content="width=device-width,initial-scale=1">
<title>QMapShack icon review</title>
<style>
:root{--bg:#14161c;--fg:#e6e8ef;--dim:#949cb0;--line:#2c3040;--card:#191c24;--accent:#4a90d9}
@media (prefers-color-scheme:light){:root{--bg:#f7f8fa;--fg:#14161c;--dim:#5c6273;--line:#dcdfe8;--card:#fff}}
*{box-sizing:border-box}
body{margin:0;background:var(--bg);color:var(--fg);font:14px system-ui,-apple-system,"Segoe UI",Roboto,sans-serif;line-height:1.4}
header{padding:16px 20px 6px}
h1{margin:0;font-size:1.2rem;letter-spacing:-.01em}
.sub{color:var(--dim);font-size:.85rem;margin-top:2px}
.bar{position:sticky;top:0;z-index:5;background:var(--bg);border-bottom:1px solid var(--line);
     padding:10px 20px;display:flex;gap:12px;flex-wrap:wrap;align-items:center}
input[type=search],select,button{font:inherit;color:var(--fg);background:var(--card);
     border:1px solid var(--line);border-radius:7px;padding:6px 10px}
input[type=search]{min-width:200px}
:focus-visible{outline:2px solid var(--accent);outline-offset:1px}
.seg{display:flex;border:1px solid var(--line);border-radius:7px;overflow:hidden}
.seg button{border:0;border-right:1px solid var(--line);border-radius:0;padding:6px 12px;cursor:pointer;background:var(--card);color:var(--dim);font-variant-numeric:tabular-nums}
.seg button:last-child{border-right:0}
.seg button[aria-pressed=true]{background:var(--accent);color:#fff}
.count{margin-left:auto;color:var(--dim);font-size:.85rem;font-variant-numeric:tabular-nums}
button.act{cursor:pointer}
button.primary{background:var(--accent);border-color:var(--accent);color:#fff}
button:disabled{opacity:.45;cursor:default}
main{padding:16px 20px 90px;display:grid;grid-template-columns:repeat(auto-fill,minmax(196px,1fr));gap:14px}
.card{border:1px solid var(--line);border-radius:10px;background:var(--card);padding:10px;display:flex;flex-direction:column;gap:8px}
.card.flag{border-color:var(--accent);box-shadow:inset 0 0 0 1px var(--accent)}
.nm{font-family:ui-monospace,"Cascadia Mono",Menlo,monospace;font-size:.8rem;display:flex;align-items:center;gap:6px}
.nm span{overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.fl{margin-left:auto;cursor:pointer;border:1px solid var(--line);border-radius:6px;padding:1px 7px;background:transparent;color:var(--dim);font-size:.9rem;line-height:1.3}
.fl[aria-pressed=true]{background:var(--accent);border-color:var(--accent);color:#fff}
.tiles{display:flex;gap:8px}
.tile{flex:1;display:grid;place-items:center;height:84px;border-radius:7px;overflow:hidden}
.tile.l{background:#efefef}.tile.d{background:#353535}
.tile img{display:block}
.note{width:100%;font:inherit;font-size:.78rem;padding:5px 8px;border-radius:6px;
      border:1px solid var(--line);background:var(--bg);color:var(--fg);resize:vertical;height:32px;min-height:32px}
.note:not(.on){display:none}
dialog{border:1px solid var(--line);border-radius:12px;background:var(--card);color:var(--fg);padding:18px;max-width:680px;width:92vw}
dialog::backdrop{background:rgba(0,0,0,.5)}
dialog h2{margin:0 0 8px;font-size:1rem}
dialog textarea{width:100%;height:300px;font-family:ui-monospace,monospace;font-size:.78rem;
      border:1px solid var(--line);border-radius:8px;padding:10px;background:var(--bg);color:var(--fg)}
.dlgbar{display:flex;gap:8px;justify-content:flex-end;margin-top:10px}
footer{color:var(--dim);font-size:.8rem;padding:0 20px 40px}
</style></head><body>
<header><h1>QMapShack icon review</h1><div class=sub id=sub></div></header>
<div class=bar>
  <input type=search id=q placeholder="Filter by name…" aria-label="Filter by name" autofocus>
  <div class=seg id=size role=group aria-label="Icon size">
    <button data-s=16>16</button><button data-s=24>24</button>
    <button data-s=32 aria-pressed=true>32</button><button data-s=48>48</button><button data-s=64>64</button>
  </div>
  <select id=sort aria-label="Sort order"><option value=name>Name A–Z</option><option value=flag>Flagged first</option></select>
  <button class="act primary" id=report disabled>Copy comments</button>
  <button class=act id=clearbar disabled title="Clear every flag and comment">Clear</button>
  <span class=count id=count></span>
</div>
<main id=grid></main>
<footer>Each icon: <b>light</b> (left) vs <b>dark</b> (right), themed as the app renders it.
  ⚑ a row and type a comment; <b>Copy comments</b> exports them. Flags are saved in this browser.</footer>
<dialog id=dlg><h2>Icon comments</h2>
  <p style="margin:0 0 10px;color:var(--dim)">Copied to your clipboard — paste it back into the conversation.</p>
  <textarea id=out readonly></textarea>
  <div class=dlgbar><button class=act id=clear>Clear all</button><button class="act primary" id=close>Done</button></div>
</dialog>
<script>
const DATA=/*DATA*/;
const KEY="qms-icon-review-v2";
let size=32,sort="name",query="",flags={};
try{flags=JSON.parse(localStorage.getItem(KEY)||"{}")}catch(e){flags={}}
const grid=document.getElementById("grid"),countEl=document.getElementById("count"),
      reportBtn=document.getElementById("report"),clearBtn=document.getElementById("clearbar");
document.getElementById("sub").textContent=DATA.length+" icons · dark column = engine-themed (#ccccff / #353535 / #e0e0e0 / #66aaff)";

function save(){try{localStorage.setItem(KEY,JSON.stringify(flags))}catch(e){}
  const n=Object.keys(flags).length;reportBtn.disabled=!n;reportBtn.textContent=n?"Copy comments ("+n+")":"Copy comments";
  clearBtn.disabled=!n;clearBtn.textContent=n?"Clear ("+n+")":"Clear";}
function clearAll(){
  if(!Object.keys(flags).length)return;
  if(confirm("Clear all flags and comments?")){flags={};save();render();}}
function esc(s){return s.replace(/[&<>]/g,c=>({"&":"&amp;","<":"&lt;",">":"&gt;"}[c]));}

function cardHTML(it){
  const on=flags[it.n]!==undefined, note=on?flags[it.n]:"";
  return '<div class="card'+(on?' flag':'')+'" data-n="'+it.n+'">'
    +'<div class=nm><span title="'+it.n+'">'+it.n+'</span>'
    +'<button class=fl aria-pressed="'+on+'" title="Flag for comment">⚑</button></div>'
    +'<div class=tiles>'
    +'<div class="tile l"><img src="'+it.l+'" width='+size+' height='+size+' alt=""></div>'
    +'<div class="tile d"><img src="'+it.d+'" width='+size+' height='+size+' alt=""></div></div>'
    +'<textarea class="note'+(on?' on':'')+'" placeholder="comment…">'+esc(note)+'</textarea></div>';
}
function render(){
  const rows=DATA.filter(it=>it.n.toLowerCase().includes(query));
  rows.sort((a,b)=> sort==="flag"
    ? ((flags[b.n]!==undefined)-(flags[a.n]!==undefined))||a.n.localeCompare(b.n)
    : a.n.localeCompare(b.n));
  grid.innerHTML=rows.map(cardHTML).join("");
  countEl.textContent=rows.length+(rows.length===DATA.length?"":" / "+DATA.length)+" shown";
}
grid.addEventListener("click",e=>{
  const b=e.target.closest(".fl"); if(!b)return;
  const card=b.closest(".card"),n=card.dataset.n,t=card.querySelector(".note");
  if(flags[n]!==undefined){delete flags[n];card.classList.remove("flag");b.setAttribute("aria-pressed","false");t.classList.remove("on");}
  else{flags[n]="";card.classList.add("flag");b.setAttribute("aria-pressed","true");t.classList.add("on");t.focus();}
  save();
});
grid.addEventListener("input",e=>{
  if(!e.target.classList.contains("note"))return;
  flags[e.target.closest(".card").dataset.n]=e.target.value;save();
});
document.getElementById("q").addEventListener("input",e=>{query=e.target.value.toLowerCase();render();});
document.getElementById("size").addEventListener("click",e=>{
  const b=e.target.closest("button"); if(!b)return; size=+b.dataset.s;
  [...e.currentTarget.children].forEach(x=>x.setAttribute("aria-pressed",x===b?"true":"false")); render();});
document.getElementById("sort").addEventListener("change",e=>{sort=e.target.value;render();});
reportBtn.addEventListener("click",()=>{
  const names=Object.keys(flags).sort();
  const lines=["Icon review — "+names.length+" flagged",""];
  for(const n of names)lines.push("- "+n+(flags[n]?": "+flags[n]:""));
  const txt=lines.join("\n");
  document.getElementById("out").value=txt;
  document.getElementById("dlg").showModal();
  if(navigator.clipboard)navigator.clipboard.writeText(txt).catch(()=>{});
});
document.getElementById("close").addEventListener("click",()=>document.getElementById("dlg").close());
document.getElementById("clear").addEventListener("click",()=>{
  clearAll();document.getElementById("dlg").close();});
clearBtn.addEventListener("click",clearAll);
save();render();
</script></body></html>"""


if __name__ == "__main__":
    main()
