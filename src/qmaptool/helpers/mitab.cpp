/**********************************************************************************************
    Copyright (C) 2008 Oliver Eichler <oliver.eichler@gmx.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/
#include "mitab.h"

/*
    Tables are copied over from the GDAL project. See https://gdal.org/
 */

/* EPSG code, MapInfo datum ID (or 9999), OGC Name, datum parameters... */
const QVector<MapInfoDatumInfo> asDatumInfoList = {
    {0, 0, "", 29, 0, 0, 0, 0, 0, 0, 0, 0},  // Datum ignore
    {6269, 74, "North_American_Datum_1983", 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {6201, 1, "Adindan", 6, -162, -12, 206, 0, 0, 0, 0, 0},
    {6205, 2, "Afgooye", 3, -43, -163, 45, 0, 0, 0, 0, 0},
    {6204, 3, "Ain_el_Abd_1970", 4, -150, -251, -2, 0, 0, 0, 0, 0},
    {0, 4, "Anna_1_Astro_1965", 2, -491, -22, 435, 0, 0, 0, 0, 0},
    {6209, 5, "Arc_1950", 15, -143, -90, -294, 0, 0, 0, 0, 0},
    {6210, 6, "Arc_1960", 6, -160, -8, -300, 0, 0, 0, 0, 0},
    {0, 7, "Ascension_Islands", 4, -207, 107, 52, 0, 0, 0, 0, 0},
    {0, 8, "Astro_Beacon_E", 4, 145, 75, -272, 0, 0, 0, 0, 0},
    {0, 9, "Astro_B4_Sorol_Atoll", 4, 114, -116, -333, 0, 0, 0, 0, 0},
    {0, 10, "Astro_Dos_71_4", 4, -320, 550, -494, 0, 0, 0, 0, 0},
    {0, 11, "Astronomic_Station_1952", 4, 124, -234, -25, 0, 0, 0, 0, 0},
    {6202, 12, "Australian_Geodetic_Datum_66", 2, -133, -48, 148, 0, 0, 0, 0, 0},
    {6203, 13, "Australian_Geodetic_Datum_84", 2, -134, -48, 149, 0, 0, 0, 0, 0},
    {0, 14, "Bellevue_Ign", 4, -127, -769, 472, 0, 0, 0, 0, 0},
    {6216, 15, "Bermuda_1957", 7, -73, 213, 296, 0, 0, 0, 0, 0},
    {6218, 16, "Bogota", 4, 307, 304, -318, 0, 0, 0, 0, 0},
    {6221, 17, "Campo_Inchauspe", 4, -148, 136, 90, 0, 0, 0, 0, 0},
    {0, 18, "Canton_Astro_1966", 4, 298, -304, -375, 0, 0, 0, 0, 0},
    {6222, 19, "Cape", 6, -136, -108, -292, 0, 0, 0, 0, 0},
    {6717, 20, "Cape_Canaveral", 7, -2, 150, 181, 0, 0, 0, 0, 0},
    {6223, 21, "Carthage", 6, -263, 6, 431, 0, 0, 0, 0, 0},
    {6672, 22, "Chatham_1971", 4, 175, -38, 113, 0, 0, 0, 0, 0},
    {6224, 23, "Chua", 4, -134, 229, -29, 0, 0, 0, 0, 0},
    {6225, 24, "Corrego_Alegre", 4, -206, 172, -6, 0, 0, 0, 0, 0},
    {6211, 25, "Batavia", 10, -377, 681, -50, 0, 0, 0, 0, 0},
    {0, 26, "Dos_1968", 4, 230, -199, -752, 0, 0, 0, 0, 0},
    {6719, 27, "Easter_Island_1967", 4, 211, 147, 111, 0, 0, 0, 0, 0},
    {6230, 28, "European_Datum_1950", 4, -87, -98, -121, 0, 0, 0, 0, 0},
    {6668, 29, "European_Datum_1979", 4, -86, -98, -119, 0, 0, 0, 0, 0},
    {6233, 30, "Gandajika_1970", 4, -133, -321, 50, 0, 0, 0, 0, 0},
    {6272, 31, "New_Zealand_GD49", 4, 84, -22, 209, 0, 0, 0, 0, 0},
    {6272, 31, "New_Zealand_Geodetic_Datum_1949", 4, 84, -22, 209, 0, 0, 0, 0, 0},
    {0, 32, "GRS_67", 21, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 33, "GRS_80", 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {6171, 33, "Reseau_Geodesique_Francais_1993", 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {6619, 33, "SWEREF99", 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {6675, 34, "Guam_1963", 7, -100, -248, 259, 0, 0, 0, 0, 0},
    {0, 35, "Gux_1_Astro", 4, 252, -209, -751, 0, 0, 0, 0, 0},
    {6254, 36, "Hito_XVIII_1963", 4, 16, 196, 93, 0, 0, 0, 0, 0},
    {6658, 37, "Hjorsey_1955", 4, -73, 46, -86, 0, 0, 0, 0, 0},
    {6738, 38, "Hong_Kong_1963", 4, -156, -271, -189, 0, 0, 0, 0, 0},
    {6236, 39, "Hu_Tzu_Shan", 4, -634, -549, -201, 0, 0, 0, 0, 0},
    {0, 40, "Indian_Thailand_Vietnam", 11, 214, 836, 303, 0, 0, 0, 0, 0},
    {0, 41, "Indian_Bangladesh", 11, 289, 734, 257, 0, 0, 0, 0, 0},
    {6299, 42, "Ireland_1965", 13, 506, -122, 611, 0, 0, 0, 0, 0},
    {0, 43, "ISTS_073_Astro_1969", 4, 208, -435, -229, 0, 0, 0, 0, 0},
    {6725, 44, "Johnston_Island_1961", 4, 191, -77, -204, 0, 0, 0, 0, 0},
    {6244, 45, "Kandawala", 11, -97, 787, 86, 0, 0, 0, 0, 0},
    {0, 46, "Kerguyelen_Island", 4, 145, -187, 103, 0, 0, 0, 0, 0},
    {6245, 47, "Kertau", 17, -11, 851, 5, 0, 0, 0, 0, 0},
    {0, 48, "L_C_5_Astro", 7, 42, 124, 147, 0, 0, 0, 0, 0},
    {6251, 49, "Liberia_1964", 6, -90, 40, 88, 0, 0, 0, 0, 0},
    {0, 50, "Luzon_Phillippines", 7, -133, -77, -51, 0, 0, 0, 0, 0},
    {0, 51, "Luzon_Mindanao_Island", 7, -133, -79, -72, 0, 0, 0, 0, 0},
    {6256, 52, "Mahe_1971", 6, 41, -220, -134, 0, 0, 0, 0, 0},
    {0, 53, "Marco_Astro", 4, -289, -124, 60, 0, 0, 0, 0, 0},
    {6262, 54, "Massawa", 10, 639, 405, 60, 0, 0, 0, 0, 0},
    {6261, 55, "Merchich", 16, 31, 146, 47, 0, 0, 0, 0, 0},
    {0, 56, "Midway_Astro_1961", 4, 912, -58, 1227, 0, 0, 0, 0, 0},
    {6263, 57, "Minna", 6, -92, -93, 122, 0, 0, 0, 0, 0},
    {0, 58, "Nahrwan_Masirah_Island", 6, -247, -148, 369, 0, 0, 0, 0, 0},
    {0, 59, "Nahrwan_Un_Arab_Emirates", 6, -249, -156, 381, 0, 0, 0, 0, 0},
    {0, 60, "Nahrwan_Saudi_Arabia", 6, -231, -196, 482, 0, 0, 0, 0, 0},
    {6271, 61, "Naparima_1972", 4, -2, 374, 172, 0, 0, 0, 0, 0},
    {6267, 62, "NAD_1927", 7, -8, 160, 176, 0, 0, 0, 0, 0},
    {6267, 62, "North_American_Datum_1927", 7, -8, 160, 176, 0, 0, 0, 0, 0},
    {0, 63, "NAD_27_Alaska", 7, -5, 135, 172, 0, 0, 0, 0, 0},
    {0, 64, "NAD_27_Bahamas", 7, -4, 154, 178, 0, 0, 0, 0, 0},
    {0, 65, "NAD_27_San_Salvador", 7, 1, 140, 165, 0, 0, 0, 0, 0},
    {0, 66, "NAD_27_Canada", 7, -10, 158, 187, 0, 0, 0, 0, 0},
    {0, 67, "NAD_27_Canal_Zone", 7, 0, 125, 201, 0, 0, 0, 0, 0},
    {0, 68, "NAD_27_Caribbean", 7, -7, 152, 178, 0, 0, 0, 0, 0},
    {0, 69, "NAD_27_Central_America", 7, 0, 125, 194, 0, 0, 0, 0, 0},
    {0, 70, "NAD_27_Cuba", 7, -9, 152, 178, 0, 0, 0, 0, 0},
    {0, 71, "NAD_27_Greenland", 7, 11, 114, 195, 0, 0, 0, 0, 0},
    {0, 72, "NAD_27_Mexico", 7, -12, 130, 190, 0, 0, 0, 0, 0},
    {0, 73, "NAD_27_Michigan", 8, -8, 160, 176, 0, 0, 0, 0, 0},
    {0, 75, "Observatorio_1966", 4, -425, -169, 81, 0, 0, 0, 0, 0},
    {0, 76, "Old_Egyptian", 22, -130, 110, -13, 0, 0, 0, 0, 0},
    {6135, 77, "Old_Hawaiian", 7, 61, -285, -181, 0, 0, 0, 0, 0},
    {0, 78, "Oman", 6, -346, -1, 224, 0, 0, 0, 0, 0},
    {6277, 79, "OSGB_1936", 9, 375, -111, 431, 0, 0, 0, 0, 0},
    {0, 80, "Pico_De_Las_Nieves", 4, -307, -92, 127, 0, 0, 0, 0, 0},
    {6729, 81, "Pitcairn_Astro_1967", 4, 185, 165, 42, 0, 0, 0, 0, 0},
    {6248, 82, "Provisional_South_American", 4, -288, 175, -376, 0, 0, 0, 0, 0},
    {6139, 83, "Puerto_Rico", 7, 11, 72, -101, 0, 0, 0, 0, 0},
    {6614, 84, "Qatar_National", 4, -128, -283, 22, 0, 0, 0, 0, 0},
    {6287, 85, "Qornoq", 4, 164, 138, -189, 0, 0, 0, 0, 0},
    {6627, 86, "Reunion", 4, 94, -948, -1262, 0, 0, 0, 0, 0},
    {6265, 87, "Monte_Mario", 4, -225, -65, 9, 0, 0, 0, 0, 0},
    {0, 88, "Santo_Dos", 4, 170, 42, 84, 0, 0, 0, 0, 0},
    {0, 89, "Sao_Braz", 4, -203, 141, 53, 0, 0, 0, 0, 0},
    {6292, 90, "Sapper_Hill_1943", 4, -355, 16, 74, 0, 0, 0, 0, 0},
    {6293, 91, "Schwarzeck", 14, 616, 97, -251, 0, 0, 0, 0, 0},
    {6618, 92, "South_American_Datum_1969", 24, -57, 1, -41, 0, 0, 0, 0, 0},
    {0, 93, "South_Asia", 19, 7, -10, -26, 0, 0, 0, 0, 0},
    {0, 94, "Southeast_Base", 4, -499, -249, 314, 0, 0, 0, 0, 0},
    {0, 95, "Southwest_Base", 4, -104, 167, -38, 0, 0, 0, 0, 0},
    {6298, 96, "Timbalai_1948", 11, -689, 691, -46, 0, 0, 0, 0, 0},
    {6301, 97, "Tokyo", 10, -128, 481, 664, 0, 0, 0, 0, 0},
    {0, 98, "Tristan_Astro_1968", 4, -632, 438, -609, 0, 0, 0, 0, 0},
    {6731, 99, "Viti_Levu_1916", 6, 51, 391, -36, 0, 0, 0, 0, 0},
    {0, 100, "Wake_Entiwetok_1960", 23, 101, 52, -39, 0, 0, 0, 0, 0},
    {0, 101, "WGS_60", 26, 0, 0, 0, 0, 0, 0, 0, 0},
    {6760, 102, "WGS_66", 27, 0, 0, 0, 0, 0, 0, 0, 0},
    {6322, 103, "WGS_1972", 1, 0, 8, 10, 0, 0, 0, 0, 0},
    {6322, 103, "World_Geodetic_System_1972", 1, 0, 8, 10, 0, 0, 0, 0, 0},
    {6326, 104, "WGS_1984", 28, 0, 0, 0, 0, 0, 0, 0, 0},
    {6309, 105, "Yacare", 4, -155, 171, 37, 0, 0, 0, 0, 0},
    {6311, 106, "Zanderij", 4, -265, 120, -358, 0, 0, 0, 0, 0},
    {0, 107, "NTF", 30, -168, -60, 320, 0, 0, 0, 0, 0},
    {6231, 108, "European_Datum_1987", 4, -83, -96, -113, 0, 0, 0, 0, 0},
    {0, 109, "Netherlands_Bessel", 10, 593, 26, 478, 0, 0, 0, 0, 0},
    {0, 110, "Belgium_Hayford", 4, 81, 120, 129, 0, 0, 0, 0, 0},
    {0, 111, "NWGL_10", 1, -1, 15, 1, 0, 0, 0, 0, 0},
    {6124, 112, "Rikets_koordinatsystem_1990", 10, 498, -36, 568, 0, 0, 0, 0, 0},
    {0, 113, "Lisboa_DLX", 4, -303, -62, 105, 0, 0, 0, 0, 0},
    {0, 114, "Melrica_1973_D73", 4, -223, 110, 37, 0, 0, 0, 0, 0},
    {6258, 115, "Euref_89", 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {6283, 116, "GDA94", 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {6283, 116, "Geocentric_Datum_of_Australia_1994", 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {6167, 117, "NZGD2000", 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {6167, 117, "New_Zealand_Geodetic_Datum_2000", 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {6169, 118, "America_Samoa", 7, -115, 118, 426, 0, 0, 0, 0, 0},
    {0, 119, "Antigua_Astro_1965", 6, -270, 13, 62, 0, 0, 0, 0, 0},
    {6713, 120, "Ayabelle_Lighthouse", 6, -79, -129, 145, 0, 0, 0, 0, 0},
    {6219, 121, "Bukit_Rimpah", 10, -384, 664, -48, 0, 0, 0, 0, 0},
    {0, 122, "Estonia_1937", 10, 374, 150, 588, 0, 0, 0, 0, 0},
    {6155, 123, "Dabola", 6, -83, 37, 124, 0, 0, 0, 0, 0},
    {6736, 124, "Deception_Island", 6, 260, 12, -147, 0, 0, 0, 0, 0},
    {0, 125, "Fort_Thomas_1955", 6, -7, 215, 225, 0, 0, 0, 0, 0},
    {0, 126, "Graciosa_base_1948", 4, -104, 167, -38, 0, 0, 0, 0, 0},
    {6255, 127, "Herat_North", 4, -333, -222, 114, 0, 0, 0, 0, 0},
    {0, 128, "Hermanns_Kogel", 10, 682, -203, 480, 0, 0, 0, 0, 0},
    {6240, 129, "Indian", 50, 283, 682, 231, 0, 0, 0, 0, 0},
    {6239, 130, "Indian_1954", 11, 217, 823, 299, 0, 0, 0, 0, 0},
    {6131, 131, "Indian_1960", 11, 198, 881, 317, 0, 0, 0, 0, 0},
    {6240, 132, "Indian_1975", 11, 210, 814, 289, 0, 0, 0, 0, 0},
    {6238, 133, "Indonesian_Datum_1974", 4, -24, -15, 5, 0, 0, 0, 0, 0},
    {0, 134, "ISTS061_Astro_1968", 4, -794, 119, -298, 0, 0, 0, 0, 0},
    {0, 135, "Kusaie_Astro_1951", 4, 647, 1777, -1124, 0, 0, 0, 0, 0},
    {6250, 136, "Leigon", 6, -130, 29, 364, 0, 0, 0, 0, 0},
    {0, 137, "Montserrat_Astro_1958", 6, 174, 359, 365, 0, 0, 0, 0, 0},
    {6266, 138, "Mporaloko", 6, -74, -130, 42, 0, 0, 0, 0, 0},
    {0, 139, "North_Sahara_1959", 6, -186, -93, 310, 0, 0, 0, 0, 0},
    {0, 140, "Observatorio_Met_1939", 4, -425, -169, 81, 0, 0, 0, 0, 0},
    {6620, 141, "Point_58", 6, -106, -129, 165, 0, 0, 0, 0, 0},
    {6282, 142, "Pointe_Noire", 6, -148, 51, -291, 0, 0, 0, 0, 0},
    {6615, 143, "Porto_Santo_1936", 4, -499, -249, 314, 0, 0, 0, 0, 0},
    {6616, 144, "Selvagem_Grande_1938", 4, -289, -124, 60, 0, 0, 0, 0, 0},
    {0, 145, "Sierra_Leone_1960", 6, -88, 4, 101, 0, 0, 0, 0, 0},
    {6156, 146, "S_JTSK_Ferro", 10, 589, 76, 480, 0, 0, 0, 0, 0},
    {6297, 147, "Tananarive_1925", 4, -189, -242, -91, 0, 0, 0, 0, 0},
    {6811, 148, "Voirol_1874", 6, -73, -247, 227, 0, 0, 0, 0, 0},
    {0, 149, "Virol_1960", 6, -123, -206, 219, 0, 0, 0, 0, 0},
    {6148, 150, "Hartebeesthoek94", 28, 0, 0, 0, 0, 0, 0, 0, 0},
    {6122, 151, "ATS77", 51, 0, 0, 0, 0, 0, 0, 0, 0},
    {6612, 152, "JGD2000", 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 153, "HGRS87", 0, -199.87, 74.79, 246.62, 0, 0, 0, 0, 0},
    {6214, 154, "Beijing 1954", 3, -31.4, 144.3, 81.2, 0, 0, 0, 0, 0},
    {6754, 155, "Libya (LGD 2006)", 4, 208.4058, 109.8777, 2.5764, 0, 0, 0, 0, 0},
    {6317, 156, "Dealul Piscului 1970", 3, 28, -121, -77, 0, 0, 0, 0, 0},
    {0, 157, "WGS_1984", 54, 0, 0, 0, 0, 0, 0, 0, 0},  // Google merc
    {6150, 158, "CH1903+ datum for Switzerland", 10, 674.374, 15.056, 405.346, 0, 0, 0, 0, 0},
    {0, 159, "Schwarzeck (updated) datum for Namibia", 14, 616.8, 103.3, -256.9, 0, 0, 0, 0, 0},
    {0, 161, "NOAA GCS_Sphere", 55, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1000, "DHDN_Potsdam_Rauenberg", 10, 582, 105, 414, -1.04, -0.35, 3.08, 8.3, 0},
    {6284, 1001, "Pulkovo_1942", 3, 24, -123, -94, -0.02, 0.25, 0.13, 1.1, 0},
    {6807, 1002, "NTF_Paris_Meridian", 30, -168, -60, 320, 0, 0, 0, 0, 2.337229166667},
    {6149, 1003, "Switzerland_CH_1903", 10, 660.077, 13.551, 369.344, 0.804816, 0.577692, 0.952236, 5.66, 0},
    {6237, 1004, "Hungarian_Datum_1972", 21, -56, 75.77, 15.31, -0.37, -0.2, -0.21, -1.01, 0},
    {0, 1005, "Cape_7_Parameter", 28, -134.73, -110.92, -292.66, 0, 0, 0, 1, 0},
    {6203, 1006, "AGD84_7_Param_Aust", 2, -117.763, -51.51, 139.061, -0.292, -0.443, -0.277, -0.191, 0},
    {0, 1007, "AGD66_7_Param_ACT", 2, -129.193, -41.212, 130.73, -0.246, -0.374, -0.329, -2.955, 0},
    {0, 1008, "AGD66_7_Param_TAS", 2, -120.271, -64.543, 161.632, -0.2175, 0.0672, 0.1291, 2.4985, 0},
    {0, 1009, "AGD66_7_Param_VIC_NSW", 2, -119.353, -48.301, 139.484, -0.415, -0.26, -0.437, -0.613, 0},
    {6272, 1010, "NZGD_7_Param_49", 4, 59.47, -5.04, 187.44, -0.47, 0.1, -1.024, -4.5993, 0},
    {0, 1011, "Rikets_Tri_7_Param_1990", 10, 419.3836, 99.3335, 591.3451, -0.850389, -1.817277, 7.862238, -0.99496, 0},
    {0, 1012, "Russia_PZ90", 52, -1.08, -0.27, -0.9, 0, 0, -0.16, -0.12, 0},
    {0, 1013, "Russia_SK42", 52, 23.92, -141.27, -80.9, 0, -0.35, -0.82, -0.12, 0},
    {0, 1014, "Russia_SK95", 52, 24.82, -131.21, -82.66, 0, 0, -0.16, -0.12, 0},
    {6301, 1015, "Tokyo", 10, -146.414, 507.337, 680.507, 0, 0, 0, 0, 0},
    {6123, 1016, "Kartastokoordinaattijarjestelma_1966", 4, -96.062, -82.428, -121.754, -4.801, -0.345, 1.376, 1.496,
     0},
    {6610, 1017, "Xian 1980", 53, 24, -123, -94, -0.02, -0.25, 0.13, 1.1, 0},
    {0, 1018, "Lithuanian Pulkovo 1942", 4, -40.59527, -18.54979, -69.33956, -2.508, -1.8319, 2.6114, -4.2991, 0},
    {6313, 1019, "Belgian 1972 7 Parameter", 4, -99.059, 53.322, -112.486, -0.419, 0.83, -1.885, 0.999999, 0},
    {6818, 1020, "S-JTSK with Ferro prime meridian", 10, 589, 76, 480, 0, 0, 0, 0, -17.666666666667},
    {1031, 1021, "Serbia datum MGI 1901", 10, 574.027, 170.175, 401.545, 4.88786, -0.66524, -13.24673, 6.88933, 0},
    {0, 1022, "North Sahara 7-parameter", 6, -38.7086, -128.8054, 118.8837, 0.83822, 7.38459, -1.57989, 3.9904, 0},
    {0, 1023, "Hungarian Projection System (EOV) - updated", 21, 52.684, -71.194, -13.975, 0.312, 0.1063, 0.3729,
     1.0191, 0},
    {1052, 1024, "S-JTSK (Krovak) Coordinate system - updated", 10, 570.6934, 85.6936, 462.8393, -4.99825, -1.58663,
     -5.26114, 3.5430155, 0},
    {0, 1025, "JTSK03 (Slovak Republic)", 10, 485.014055, 169.473618, 483.842943, -7.78625453, -4.39770887, -4.10248899,
     0, 0},
    {1168, 1028, "Geocentric Datum of Australia 2020", 0, -0.06155, 0.01087, 0.04019, 0.0394924, 0.0327221, 0.0328979,
     0.009994, 0},
    {0, 9999, "Bosnia-Herzegovina", 10, 472.8677, 187.8769, 544.7084, -5.76198422, -5.3222842, 12.80666941, 1.54517287,
     0},
    {6181, 9999, "Luxembourg 1930 / Gauss", 4, -192.986, 13.673, -39.309, 0.4099, 2.9332, -2.6881, 0.43, 0},
    {1168, 9999, "Geocentric Datum of Australia 2020", 0, -0.06155, 0.01087, 0.04019, 0.0394924, 0.0327221, 0.0328979,
     0.009994, 0},
};

/* -------------------------------------------------------------------- */
/*      This table was hand entered from Appendix I of the mapinfo 6    */
/*      manuals.                                                        */
/* -------------------------------------------------------------------- */

const QVector<MapInfoSpheroidInfo> asSpheroidInfoList = {
    {9, "Airy 1930", 6377563.396, 299.3249646},
    {13, "Airy 1930 (modified for Ireland 1965", 6377340.189, 299.3249646},
    {51, "ATS77 (Average Terrestrial System 1977)", 6378135, 298.257},
    {2, "Australian", 6378160.0, 298.25},
    {10, "Bessel 1841", 6377397.155, 299.1528128},
    {35, "Bessel 1841 (modified for NGO 1948)", 6377492.0176, 299.15281},
    {14, "Bessel 1841 (modified for Schwarzeck)", 6377483.865, 299.1528128},
    {36, "Clarke 1858", 6378293.639, 294.26068},
    {7, "Clarke 1866", 6378206.4, 294.9786982},
    {8, "Clarke 1866 (modified for Michigan)", 6378450.047484481, 294.9786982},
    {6, "Clarke 1880", 6378249.145, 293.465},
    {15, "Clarke 1880 (modified for Arc 1950)", 6378249.145326, 293.4663076},
    {30, "Clarke 1880 (modified for IGN)", 6378249.2, 293.4660213},
    {37, "Clarke 1880 (modified for Jamaica)", 6378249.136, 293.46631},
    {16, "Clarke 1880 (modified for Merchich)", 6378249.2, 293.46598},
    {38, "Clarke 1880 (modified for Palestine)", 6378300.79, 293.46623},
    {39, "Everest (Brunei and East Malaysia)", 6377298.556, 300.8017},
    {11, "Everest (India 1830)", 6377276.345, 300.8017},
    {40, "Everest (India 1956)", 6377301.243, 300.80174},
    {50, "Everest (Pakistan)", 6377309.613, 300.8017},
    {17, "Everest (W. Malaysia and Singapore 1948)", 6377304.063, 300.8017},
    {48, "Everest (West Malaysia 1969)", 6377304.063, 300.8017},
    {18, "Fischer 1960", 6378166.0, 298.3},
    {19, "Fischer 1960 (modified for South Asia)", 6378155.0, 298.3},
    {20, "Fischer 1968", 6378150.0, 298.3},
    {21, "GRS 67", 6378160.0, 298.247167427},
    {0, "GRS 80", 6378137.0, 298.257222101},
    {5, "Hayford", 6378388.0, 297.0},
    {22, "Helmert 1906", 6378200.0, 298.3},
    {23, "Hough", 6378270.0, 297.0},
    {31, "IAG 75", 6378140.0, 298.257222},
    {41, "Indonesian", 6378160.0, 298.247},
    {4, "International 1924", 6378388.0, 297.0},
    {49, "Irish (WOFO)", 6377542.178, 299.325},
    {3, "Krassovsky", 6378245.0, 298.3},
    {32, "MERIT 83", 6378137.0, 298.257},
    {33, "New International 1967", 6378157.5, 298.25},
    {42, "NWL 9D", 6378145.0, 298.25},
    {43, "NWL 10D", 6378135.0, 298.26},
    {44, "OSU86F", 6378136.2, 298.25722},
    {45, "OSU91A", 6378136.3, 298.25722},
    {46, "Plessis 1817", 6376523.0, 308.64},
    {52, "PZ90", 6378136.0, 298.257839303},
    {24, "South American", 6378160.0, 298.25},
    {12, "Sphere", 6370997.0, 0.0},
    {47, "Struve 1860", 6378297.0, 294.73},
    {34, "Walbeck", 6376896.0, 302.78},
    {25, "War Office", 6378300.583, 296.0},
    {26, "WGS 60", 6378165.0, 298.3},
    {27, "WGS 66", 6378145.0, 298.25},
    {1, "WGS 72", 6378135.0, 298.26},
    {28, "WGS 84", 6378137.0, 298.257223563},
    {29, "WGS 84 (MAPINFO Datum 0)", 6378137.01, 298.257223563},
    {54, "WGS 84 (MAPINFO Datum 157)", 6378137.01, 298.257223563},
};
