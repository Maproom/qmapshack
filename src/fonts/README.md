# Bundled fonts

`DejaVuSans.ttf` and `DejaVuSans-Bold.ttf`, version 2.37, from
<https://dejavu-fonts.github.io/>. `LICENSE` is the upstream file and has to travel with them.

They are here for the documentation images. A `--shoot` run draws through Qt's offscreen platform,
which on Windows uses the freetype font database, and that one populates itself from a font
directory Qt no longer ships: without a registered family every glyph renders as an empty box.
`IAppSetup::processArguments()` registers both faces for a shoot or documentation run, so the
family `shots.py` pins is the application's own on every platform rather than something the
machine happens to have.

Nothing else uses them, and a normal run does not load them.
