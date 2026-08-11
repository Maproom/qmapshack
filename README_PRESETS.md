# Building with CMake presets

`CMakePresets.json` in the project root stores ready-made build configurations, so you do not have
to remember generator names, build types or dependency paths. Requires **CMake 3.21+** and, for the
Linux presets, **Ninja**.

Everything below is run from the **project root** — presets create their own build directory, you
never `mkdir build` or `cd` anywhere yourself.

## The three commands you need

```bash
cmake --list-presets                      # what is available on this machine
cmake --preset linux-debug                # configure (once, or after CMakeLists.txt changes)
cmake --build --preset linux-debug        # compile
```

Run the result:

```bash
build/linux-debug/bin/qmapshack
```

Each preset builds into `build/<preset-name>/`, so several presets can coexist without wiping each
other. `build/` is git-ignored.

## Available presets

| Preset | Build type | Notes |
|---|---|---|
| `linux-debug` | `Debug` | Everyday development build. |
| `linux-release` | `RelWithDebInfo` | Optimised, still debuggable — use it to reproduce speed problems. |
| `linux-asan` | `Debug` + AddressSanitizer | Sets `DEVELOPMENT_VERSION=ON`. Slower, catches memory errors. |
| `windows-msvc` | Visual Studio 17 2022, x64 | Dependency paths are placeholders — see below. |
| `macos` | `Release`, Unix Makefiles | Reads the environment `MacOSX/config.sh` exports; source it first. |

Presets are guarded by a host condition, so `--list-presets` only shows the ones that apply to your
operating system.

The `windows-msvc` and `macos` presets are **not routinely tested**; the established route on those
platforms is still `msvc_64/` and `MacOSX/build-all.sh`.

## Building a single target

The build presets compile everything. To build just one binary, pass the target through:

```bash
cmake --build --preset linux-debug --target qmapshack -j$(nproc)
```

Targets: `qmapshack`, `qmaptool`, `qmt_map2jnx`, `qmt_rgb2pct`.

## Adding options on top of a preset

Extra cache variables go on the configure line with `-D`, exactly as without presets:

```bash
cmake --preset linux-debug -DBUILD_QMAPTOOL=OFF
```

Useful ones:

| Option | Default | Meaning |
|---|---|---|
| `BUILD_QMAPSHACK` / `BUILD_QMAPTOOL` | `ON` | Skip the half of the tree you are not touching. |
| `USE_QT6DBus` | `ON` on Linux | Device detection via DBus. |
| `BUILD_FOR_LOCAL_SYSTEM` | `OFF` | `-march=native`; the binary is then **not** portable. |
| `UPDATE_TRANSLATIONS` | `OFF` | Only turn on when deliberately refreshing the `.ts` catalogs. |
| `CMAKE_INSTALL_PREFIX` | `/usr/local` | Where `cmake --install build/<preset>` puts things. |

Reviewing or tweaking options interactively still works — point the usual tools at the preset's
build directory:

```bash
ccmake build/linux-debug        # or: cmake-gui build/linux-debug
```

## Your own private preset

Never edit `CMakePresets.json` for machine-specific paths — it is tracked. Create
**`CMakeUserPresets.json`** next to it instead; it is git-ignored and CMake merges it automatically.
Inherit from a shipped preset and override only what differs:

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "my-debug",
      "inherits": "linux-debug",
      "cacheVariables": {
        "BUILD_QMAPTOOL": "OFF",
        "CMAKE_INSTALL_PREFIX": "/home/me/opt/qms"
      }
    }
  ],
  "buildPresets": [
    { "name": "my-debug", "configurePreset": "my-debug" }
  ]
}
```

Then use `cmake --preset my-debug` / `cmake --build --preset my-debug` as before. This is also the
right place for the Windows dependency paths (`QT_DEV_PATH`, `GDAL_DEV_PATH`, `PROJ_DEV_PATH`,
`ROUTINO_DEV_PATH`, `JPEG_DEV_PATH`) — inherit `windows-msvc` and
override them rather than editing the tracked defaults.

## IDE support

Qt Creator, CLion, VS Code (CMake Tools) and Visual Studio all read `CMakePresets.json` and offer
the presets in their configuration selector. Nothing extra to set up: open the project folder and
pick a preset.

## When things go wrong

- **`No such preset` / no presets listed** — CMake older than 3.21, or you are not in the project
  root.
- **Configuration is stale after editing `CMakeLists.txt`** — re-run `cmake --preset <name>`; it
  reuses the existing cache. To start clean, delete `build/<name>/` and configure again.
- **`Could not find ASAN`** (`linux-asan`) — the compiler has no `-fsanitize=address`; install the
  sanitizer runtime (`libasan` / `libclang-rt-*-dev`).
- **Ninja not found** — install it, or add a user preset inheriting `linux-debug` with
  `"generator": "Unix Makefiles"`.
