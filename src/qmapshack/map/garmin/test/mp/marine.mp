; mkgmap: --code-page=1252 --lower-case
; Marine extra data of every type range mkgmap encodes (ExtTypeAttributes): lights, buoys, depths, obstructions,
; label colour, facilities, depth lines and areas, line styles.
[IMG ID]
CodePage=1252
LblCoding=9
ID=63570010
Name=QMS marine
Levels=2
Level0=24
Level1=18
Zoom0=0
Zoom1=1
[END-IMG ID]

[POI]
Type=0x10100
Data0=(47.5000000,10.5000000)
Label=Light Single
Light=(red,10,0)
Period=4
[END]

[POI]
Type=0x10101
Data0=(47.5010000,10.5000000)
Label=Light Sectors
Light=(red,10,0),(green,8,120),(white,12,240)
Period=5
HeightAboveDatum=12
HeightAboveFoundation=30
HeightAboveFoundationUnit=f
LightType=flashing
Racon=Y
[END]

[POI]
Type=0x10102
Data0=(47.5020000,10.5000000)
Label=Light Morse
Light=(white,6,0)
LightType=U
[END]

[POI]
Type=0x10103
Data0=(47.5030000,10.5000000)
Label=Light Note
Light=(green,3,0)
Note=Harbour entry
LeadingAngle=123.4
[END]

[POI]
Type=0x10206
Data0=(47.5040000,10.5000000)
Label=Buoy Red
FoundationColor=red
Light=(green,0,0)
Period=3
[END]

[POI]
Type=0x10301
Data0=(47.5000000,10.5100000)
Label=Sounding Metre
Depth=12
[END]

[POI]
Type=0x10301
Data0=(47.5010000,10.5100000)
Label=Sounding Tenth
Depth=5.5
[END]

[POI]
Type=0x10301
Data0=(47.5020000,10.5100000)
Label=Sounding Deep
Depth=300
[END]

[POI]
Type=0x10302
Data0=(47.5030000,10.5100000)
Label=Sounding Feet
Depth=40
DepthUnit=f
[END]

[POI]
Type=0x10400
Data0=(47.5040000,10.5100000)
Label=Obstruction
Depth=3
Position=approximate
[END]

[POI]
Type=0x10500
Data0=(47.5050000,10.5100000)
Label=Label Yellow
Color=3
[END]

[POI]
Type=0x10903
Data0=(47.5060000,10.5100000)
Label=Facility
FacilityPoint=0x10021
[END]

[POLYLINE]
Type=0x10301
Data0=(47.4900000,10.4900000), (47.4950000,10.5050000)
Label=Depth Contour
Depth=10
[END]

[POLYLINE]
Type=0x10105
Data0=(47.4910000,10.4900000), (47.4960000,10.5050000)
Label=Contour Height
Height=25
[END]

[POLYLINE]
Type=0x10400
Data0=(47.4920000,10.4900000), (47.4970000,10.5050000)
Label=Line Colour
Color=1
[END]

[POLYLINE]
Type=0x10500
Data0=(47.4930000,10.4900000), (47.4980000,10.5050000)
Label=Line Style
Color=2
Style=1
[END]

[POLYLINE]
Type=0x10600
Data0=(47.4940000,10.4900000), (47.4990000,10.5050000)
Label=Line Style High
Color=4
Style=0x13
[END]

[POLYGON]
Type=0x10301
Data0=(47.5200000,10.4800000), (47.5200000,10.5000000), (47.5300000,10.5000000), (47.5200000,10.4800000)
Label=Depth Area
Depth=7.5
[END]
