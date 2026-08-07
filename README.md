# QMapShack

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey)](#installation)

**QMapShack** is a free, open-source desktop application for planning and analysing GPS tracks, routes, and waypoints. It supports vector, raster, and online maps, digital elevation models, and multiple routing engines. QMapShack is the successor to [QLandkarteGT](http://www.qlandkarte.org/).

![QMapShack main window](https://github.com/Maproom/qmapshack/wiki/images/maproom1.png)

> **Full documentation:** [QMapShack Wiki](https://github.com/Maproom/qmapshack/wiki/DocMain)

---

## Features

- **Maps** — Garmin vector maps (`.img`, `.jnx`), GDAL raster maps (GeoTIFF, OziExplorer, …), and online maps via TMS/WMTS. Multiple maps can be displayed simultaneously, reprojected on-the-fly.
- **Elevation data** — Digital elevation models (DEM) independent from the base map, with hillshading and slope colouring.
- **Tracks & routes** — Full-featured track editor with unlimited undo/redo; cut, copy, combine, reverse; rich filter set for smoothing, resampling, speed, elevation, and timestamps.
- **Routing** — Offline routing via [Routino](http://www.routino.org/); offline and online routing via [BRouter](https://brouter.de/) (local process or HTTP API); vector/track snapping for precise alignment.
- **Waypoints & areas** — Create, edit, and organise waypoints, points of interest, and area overlays.
- **Database** — Project-oriented data model with persistent database storage, diary support, and structured folder organisation.
- **GPS devices** — Read/write support for modern Garmin and TwoNav devices; project-oriented synchronisation.
- **Analysis** — Track profile graphs, elevation, speed, slope, and heart rate analysis; realtime data display.
- **Interface** — Multi-canvas workspace with HiDPI support, light and dark colour schemes, and configurable keyboard shortcuts; available in English, German, French, Spanish, Czech, Italian, Dutch, Catalan, Croatian, and Russian.

---

## Installation

### Pre-built packages (recommended)

**Windows** — 64-bit installer available on the [releases page](https://github.com/Maproom/qmapshack/releases).

**macOS** — Binary bundle on the [releases page](https://github.com/Maproom/qmapshack/releases).

**Linux — distribution packages:**

| Distribution | Package |
|---|---|
| Debian | [packages.debian.org](https://packages.debian.org/testing/qmapshack) |
| Ubuntu | [packages.ubuntu.com](https://packages.ubuntu.com/search?keywords=qmapshack) |
| openSUSE | [software.opensuse.org](https://software.opensuse.org/package/qmapshack) |
| Arch Linux | [archlinux.org](https://archlinux.org/packages/extra/x86_64/qmapshack/) / [AUR (dev)](https://aur.archlinux.org/packages/qmapshack-git) |
| Other (deb, rpm, …) | [pkgs.org](https://pkgs.org/download/qmapshack) |

**Linux — AppImages** for Fedora, openSUSE, and Ubuntu are available from the [QMapShack AppImage project](https://github.com/kkarsten62/QMapShack.AppImage/releases). See the [AppImage wiki page](https://github.com/Maproom/qmapshack/wiki/DocQMSAppImage) for details.

---

## Building from Source

### Dependencies

| Library | Minimum version | Purpose |
|---|---|---|
| [Qt6](https://www.qt.io/) | 6.8 | Core, Widgets, Svg, SvgWidgets, Xml, Sql, Network, PrintSupport, WebEngineWidgets, Qml, UiTools, Help, LinguistTools |
| [GDAL](https://gdal.org/) | 3.10.0 | Map and raster data I/O |
| [PROJ](https://proj.org/) | 9.4.0 | Coordinate reference system transformations |
| [Routino](http://www.routino.org/) | 3.4 | Offline routing |
| [QuaZip](https://github.com/stachenov/quazip) | 1.x (Qt6 build) | ZIP archive support |
| libjpeg | — | JNX and JPEG raster support |
| [ALGLIB](https://www.alglib.net/) | — | Optional; a bundled copy is used when not found |
| CMake | 3.20 | Build system |
| C++20 compiler | — | GCC ≥ 10, Clang ≥ 12, MSVC 2022 |

Install dependencies via your distribution's package manager and make sure the **development packages** (`-dev` / `-devel`) are included. On Linux, device support additionally requires **DBus** and **UDisk2**.

Distro-specific setup guides are on the wiki:
[Ubuntu 24.04](https://github.com/Maproom/qmapshack/wiki/Build-Ubuntu-24-04-3) ·
[Ubuntu 22.04](https://github.com/Maproom/qmapshack/wiki/BuildUbuntu22-04) ·
[Fedora](https://github.com/Maproom/qmapshack/wiki/BuildFedora) ·
[macOS](https://github.com/Maproom/qmapshack/wiki/BuildOSX) ·
[Windows](https://github.com/Maproom/qmapshack/wiki/BuildWindowsVisualStudio)

### Linux / Unix

```bash
git clone https://github.com/Maproom/qmapshack.git QMapShack
cd QMapShack && git checkout dev && cd ..
mkdir build_QMapShack && cd build_QMapShack
cmake ../QMapShack
cmake --build . --target qmapshack -j$(nproc)
sudo cmake --install .
```

Use `ccmake .` or `cmake-gui .` in the build directory to review optional build settings before compiling.

Alternatively, use the shipped CMake presets — no manual build directory, no options to remember:

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug --target qmapshack -j$(nproc)
```

See [README_PRESETS.md](README_PRESETS.md) for the full list of presets and how to add your own.

**Updating:**

```bash
cd QMapShack && git pull
cd ../build_QMapShack && cmake --build . --target qmapshack -j$(nproc)
```

<details>
<summary><strong>macOS</strong></summary>

The macOS build uses Homebrew (recommended) or MacPorts. A scripted environment handles all dependencies. The bundle targets macOS 12 or later and is built for the architecture of the build host (Intel or Apple Silicon).

```bash
mkdir ~/QMSBuild && cd ~/QMSBuild
git clone https://github.com/Maproom/qmapshack.git
# Review settings:
vi qmapshack/MacOSX/config.sh
# Build everything:
sh qmapshack/MacOSX/build-all.sh | tee log.txt
```

Finished app bundles appear in the `release/` subdirectory. See [MacOSX/README.md](MacOSX/README.md) for the full guide.

</details>

<details>
<summary><strong>Windows</strong></summary>

Windows builds use MSVC 2022 (64-bit). Build scripts and step-by-step instructions are in the `msvc_64/` directory and on the [wiki](https://github.com/Maproom/qmapshack/wiki/BuildWindowsVisualStudio).

</details>

---

## Getting Started

After installation, follow the [Quick Start guide](https://github.com/Maproom/qmapshack/wiki/DocQuickStartEnglish) to set up maps, elevation data, and routing, then load your first GPX file.

Quick starts are also available in
[German](https://github.com/Maproom/qmapshack/wiki/DocQuickStartGerman),
[French](https://github.com/Maproom/qmapshack/wiki/DocQuickStartFrench),
[Spanish](https://github.com/Maproom/qmapshack/wiki/DocQuickStartSpanish), and
[Russian](https://github.com/Maproom/qmapshack/wiki/DocQuickStartRussian).

---

## Contributing

- **Bug reports & feature requests:** [GitHub Issues](https://github.com/Maproom/qmapshack/issues) — please use the provided issue templates.
- **Code contributions:** Pull requests target the `dev` branch. See the [coding guideline](https://github.com/Maproom/qmapshack/wiki/DeveloperCodingGuideline) and [commit guide](https://github.com/Maproom/qmapshack/wiki/DeveloperCommitCode) on the wiki.
- **Translations:** See [Add translations](https://github.com/Maproom/qmapshack/wiki/DeveloperTranslate) on the wiki.
- **Documentation:** Wiki contributions are welcome — see [Developing Documentation](https://github.com/Maproom/qmapshack/wiki/DevelopingDocumentation).

---

## License

QMapShack is free software released under the [GNU General Public License v3.0 or later](LICENSE).
