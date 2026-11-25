# Garmin BaseCamp under Wine — reference renderer for Garmin maps

BaseCamp shows how Garmin draws an IMG map; QMapShack's and GPXSee's rendering are compared against it.

## Ready setup

`~/wine/basecamp/` (4.5 GB):

- `prefix/` — Wine prefix (win64) with .NET 4.8 and BaseCamp 4.7.5. Maps in `prefix/drive_c/ProgramData/Garmin/Maps/`:
  `OMTB_luxembourg.gmap`, `OMTB_liechtenstein.gmap`, `Openfietsmap (BNL).gmap`, DEM files removed.
- `installers/` — `BaseCamp_475.exe`, `wine-mono-8.1.0-x86.msi`, `winetricks`, `winetricks-cache/` (.NET installers).
- `places.gpx` — the test places as waypoints (Tegelen, Berdorf, Luxembourg city, Vaduz).

Run it on a nested display, never on the desktop:

```sh
Xephyr :7 -screen 1400x950 -ac -br -noreset &
setxkbmap -display :7 us            # xdotool types ':' and '\' wrong otherwise
cd ~/wine/basecamp/prefix/drive_c/Program\ Files\ \(x86\)/Garmin/BaseCamp
WINEPREFIX=~/wine/basecamp/prefix DISPLAY=:7 WINEDEBUG=-all wine BaseCamp.exe &
```

Stop BaseCamp and Xephyr by PID, then `WINEPREFIX=~/wine/basecamp/prefix wineserver -k`.

## Building it again (Wine 9.0)

1. `wineboot -i` with `WINEPREFIX` set, `DISPLAY=:7`.
2. `BaseCamp_475.exe` from Garmin (`https://download.garmin.com/software/BaseCamp_475.exe`, linked from
   `www8.garmin.com/support/download_details.jsp?id=4435`). The bundle fails on its USB driver MSI ("operating system
   not supported", 0x80070643), but caches `BaseCamp.msi` in `drive_c/ProgramData/Package Cache/{AC8BE850-…}v4.7.5.0/`;
   install that alone: `wine msiexec /i BaseCamp.msi /qn`.
3. wine-mono is not enough: BaseCamp aborts with "Operation is not supported on this platform"
   (`mainCRTStartupStrArray`). Install real .NET: `winetricks -q --unattended dotnet48` (winetricks from its GitHub
   repository, `src/winetricks`; it removes wine-mono).
4. Maps go in as `.gmap` folders into `drive_c/ProgramData/Garmin/Maps/`; BaseCamp lists them under Karten.
   - From tiles plus TYP: `java -jar mkgmap.jar --gmapi --family-id=<TYP's FID> --product-id=1 6*.img 7*.img <typ>.TYP`.
     The FID is the u16 at offset 0x2f of the TYP; it matches the tile names (`6423xxxx.img` → 6423).
   - mkgmap cannot turn a `gmapsupp.img` into a gmap ("Error building gmapi data"); take the map's own gmap download.
5. Delete every `*.DEM` in the `.gmap` folders: BaseCamp shades the relief from them and has no switch for it.

## Driving it

- Menus open with a press/release pair (`xdotool mousedown 1; sleep 0.3; mouseup 1`), not `click`.
- Import waypoints: Datei → "In 'Meine Sammlung' importieren…", type the Windows path (`C:\places.gpx`).
- Go to a place: right-click the waypoint in the list → "Auf Karte anzeigen".
- Zoom with the mouse wheel (`click 4`/`click 5`); `+`/`-` keys do nothing. Steps by scale bar (~100 px):
  … 1 km, 700 m, 500 m, 300 m, 200 m.
- Screenshot: `import -display :7 -window root -crop 1100x850+298+55 out.png` (map area of a 1400x950 window at 0,0).
