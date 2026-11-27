; mkgmap: --code-page=1252 --lower-case --route --transparent
; A routable road, labelled through NET, then labelled extended lines and polygons; no standard polygon.
; The labelled road takes NET1 offset 0, an unlabelled road follows.
[IMG ID]
CodePage=1252
LblCoding=9
ID=63570003
Name=QMS routable
Routing=Y
Levels=2
Level0=24
Level1=18
Zoom0=0
Zoom1=1
[END-IMG ID]

[POLYLINE]
Type=0x06
Data0=(47.4900000,10.4900000), (47.4950000,10.5050000), (47.5050000,10.5150000)
Label=Main Street
RoadID=2
RouteParam=3,0,0,0,0,0,0,0,0,0,0,0
Nod1=0,3,0
Nod2=2,4,0
[END]

[POLYLINE]
Type=0x06
Data0=(47.4800000,10.4900000), (47.4850000,10.5050000)
RoadID=1
RouteParam=3,0,0,0,0,0,0,0,0,0,0,0
Nod1=0,1,0
Nod2=1,2,0
[END]

[POLYLINE]
Type=0x10e00
Data0=(47.5100000,10.4900000), (47.5150000,10.5050000)
Label=Extended Line
[END]

[POLYGON]
Type=0x10f00
Data0=(47.5200000,10.4800000), (47.5200000,10.5000000), (47.5300000,10.5000000), (47.5200000,10.4800000)
Label=Extended Area
[END]
