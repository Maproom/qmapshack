# Fixture DEM

## tirol_dgm5m.tif, tirol_dgm5m.vrt

Datenquelle: Land Tirol - data.tirol.gv.at,
[Digitales Geländemodell Tirol](https://www.data.gv.at/katalog/dataset/0454f5f3-1d8c-464e-847d-541901eb021a),
5 m, retrieved 2026-09-15, [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/). Terms:
<https://www.tirol.gv.at/data/nutzungsbedingungen/>.

Changes: cut to MGI / Austria GK West (EPSG:31254) x 9500–22800, y 259300–269600 (2660 × 2060 px,
5 m), which covers both example projects; 0 declared as nodata - Land Tirol has no data beyond its
border, which is the German part of the extent; LERC-compressed with a maximum error of 0.05 m;
overviews added.

```bash
curl -o coverage.bin 'https://gis.tirol.gv.at/arcgis/services/Service_Public/terrain/MapServer/WCSServer?service=WCS&version=2.0.1&request=GetCoverage&coverageId=Gelaendemodell_5m_M28&format=image/tiff&subset=y(259300,269600)&subset=x(9500,22800)'
# the reply is multipart/related; dgm5m.tif is its image/tiff part
gdal_translate -a_nodata 0 -co COMPRESS=LERC_DEFLATE -co MAX_Z_ERROR=0.05 -co TILED=YES dgm5m.tif tirol_dgm5m.tif
gdaladdo -r average --config COMPRESS_OVERVIEW LERC_DEFLATE --config MAX_Z_ERROR_OVERVIEW 0.05 tirol_dgm5m.tif 2 4 8
gdalbuildvrt tirol_dgm5m.vrt tirol_dgm5m.tif
```

Made with GDAL 3.12.0.
