# Fixture maps

## bev_km50.tif, bev_km50.vrt

Datenquelle: [BEV – Bundesamt für Eich- und Vermessungswesen](https://www.bev.gv.at), Kartographisches
Modell 1:50 000 – Raster (KM50-R), tile 0630-0, Stichtag 01.07.2021,
[CC BY 4.0](https://creativecommons.org/licenses/by/4.0/),
DOI [10.48677/2398d7a3-1e4f-4870-9abd-4a558cc16fd8](https://doi.org/10.48677/2398d7a3-1e4f-4870-9abd-4a558cc16fd8).

Changes: cropped to ETRS89 / UTM 32N (EPSG:25832) E 613000–623000, N 5259000–5269000
(4000 × 4000 px, 2.5 m), recompressed as JPEG, overviews added.

```bash
SRC=/vsicurl/https://data.bev.gv.at/download/KM_R/KM50/20210701/KM50_UTM32N_200L_Farbtiff_mit_Relief/km50_mit_Relief_0630_0_202107.tif
gdal_translate -projwin 613000 5269000 623000 5259000 -co COMPRESS=DEFLATE -co PREDICTOR=2 -co TILED=YES "$SRC" crop.tif
gdal_translate -co COMPRESS=JPEG -co JPEG_QUALITY=85 -co PHOTOMETRIC=YCBCR -co TILED=YES crop.tif bev_km50.tif
gdaladdo -r average --config COMPRESS_OVERVIEW JPEG --config PHOTOMETRIC_OVERVIEW YCBCR \
  --config JPEG_QUALITY_OVERVIEW 85 bev_km50.tif 2 4 8 16
gdalbuildvrt bev_km50.vrt bev_km50.tif
```

Made with GDAL 3.12.0.

## osm.tms

Online tiles from tile.openstreetmap.org, © OpenStreetMap contributors,
[ODbL](https://www.openstreetmap.org/copyright). Only the tile server definition is committed.
