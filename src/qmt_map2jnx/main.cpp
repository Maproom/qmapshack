/**********************************************************************************************
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

#include "config.h"

#ifdef _MSC_VER
#define fseeko _fseeki64
#define ftello _ftelli64
#else
#define _FILE_OFFSET_BITS 64
#endif //

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <wctype.h>


#include <list>
#include <string>
#include <vector>

#include <gdal_priv.h>
#include <proj_api.h>
#include <ogr_spatialref.h>

extern "C"
{
#include <jpeglib.h>
}

#include "argv.h"


#ifndef _MKSTR_1
#define _MKSTR_1(x)         #x
#define _MKSTR(x)           _MKSTR_1(x)
#endif

#define VER_STR             _MKSTR(VER_MAJOR) "." _MKSTR(VER_MINOR) "." _MKSTR(VER_STEP)
#define WHAT_STR            "qmt_map2jnx, Version " VER_STR

#define JNX_MAX_TILES       50000 //6250
#define JNX_MAX_TILE_SIZE   1024

#define JPG_BLOCK_SIZE      (JNX_MAX_TILE_SIZE * JNX_MAX_TILE_SIZE)

#define HEADER_BLOCK_SIZE   1024

#pragma pack(1)

struct jnx_hdr_t
{
    jnx_hdr_t(): version(0x00000004), devid(0), expire(0), productId(0), crc(0), signature(0), signature_offset(0), zorder(25){}
    uint32_t version;           // byte 00000000..00000003
    uint32_t devid;             // byte 00000004..00000007
    int32_t  top;               // byte 00000014..00000017
    int32_t  right;             // byte 00000010..00000013
    int32_t  bottom;            // byte 0000000C..0000000F
    int32_t  left;              // byte 00000008..0000000B
    uint32_t details;           // byte 00000018..0000001B
    uint32_t expire;            // byte 0000001C..0000001F
    uint32_t productId;         // byte 00000020..00000023
    uint32_t crc;               // byte 00000024..00000027
    uint32_t signature;         // byte 00000028..0000002B
    uint32_t signature_offset;  // byte 0000002C..0000002F
    uint32_t zorder;            // byte 00000030..00000033
};


struct jnx_level_t
{
    jnx_level_t(): nTiles(0), offset(0), scale(0), dummy(2){}

    uint32_t nTiles;
    uint32_t offset;
    uint32_t scale;
    uint32_t dummy;
};

struct jnx_tile_t
{
    jnx_tile_t() : top(0), right(0), bottom(0), left(0), width(0), height(0), size(0), offset(0){}
    int32_t  top;
    int32_t  right;
    int32_t  bottom;
    int32_t  left;
    uint16_t width;
    uint16_t height;
    uint32_t size;
    uint32_t offset;
};


#ifdef WIN32
#pragma pack()
#else
#pragma pack(0)
#endif

struct file_t
{
    file_t(): dataset(0), pj(0){memset(colortable,0, sizeof(colortable));}
    ~file_t()
    {
        //if(dataset) delete dataset;
        if(pj) pj_free(pj);
    }

    bool operator<(const file_t& other)  const
    {
        return (xscale > other.xscale);
    }

    std::string     filename;
    std::string     projection;
    GDALDataset *   dataset;
    projPJ          pj;
    uint32_t        width;
    uint32_t        height;
    double          xscale;
    double          yscale;
    double          scale;
    double          xref1;
    double          yref1;
    double          xref2;
    double          yref2;

    double          lon1;
    double          lat1;
    double          lon2;
    double          lat2;

    uint32_t        colortable[256];

};

struct level_t : public jnx_level_t
{
    std::list<file_t *> files;
    uint32_t tileSize;
};

struct scale_t
{
    double scale;
    uint32_t jnxScale;
};

/// number of used levels
static int32_t nLevels;
/// up to five levels. nLevels gives the actual count
static level_t levels[5];
/// information about all files
static std::list<file_t> files;
/// the target lon/lat WGS84 projection
static projPJ wgs84;
/// the JNX file header to be copied to the outfile
static jnx_hdr_t jnx_hdr;
/// the tile information table for all 5 levels
static jnx_tile_t tileTable[JNX_MAX_TILES * 5];
/// tile buffer for 8 bit palette tiles, private to readTile
static uint8_t  tileBuf8Bit[JNX_MAX_TILE_SIZE * JNX_MAX_TILE_SIZE] = {0};
/// tile buffer for 24 bit raw RGB tiles, private to writeTile
static uint8_t tileBuf24Bit[JNX_MAX_TILE_SIZE * JNX_MAX_TILE_SIZE * 3] = {0};
/// tile buffer for 32 bit raw RGBA tiles
static uint32_t tileBuf32Bit[JNX_MAX_TILE_SIZE * JNX_MAX_TILE_SIZE] = {0};
/// internal jpeg buffer used by write tile.
static std::vector<JOCTET> jpgbuf;

static void prinfFileinfo(const file_t& file)
{
    printf("\n\n----------------------");
    printf("\n%s:", file.filename.c_str());
    printf("\nprojection: %s", file.projection.c_str());
    printf("\nwidth: %i pixel height: %i pixel", file.width, file.height);

    if(pj_is_latlong(file.pj))
    {
        printf("\narea (top/left, bottom/right): %f %f, %f %f", file.lat1, file.lon1, file.lat2, file.lon2);
        printf("\nxscale: %f °/px, yscale: %f °/px", file.xscale, file.yscale);
    }
    else
    {
        printf("\narea (top/left, bottom/right): %f %f, %f %f", file.lat1, file.lon1, file.lat2, file.lon2);
        printf("\nxscale: %f m/px, yscale: %f m/px", file.xscale, file.yscale);
    }
    printf("\nreal scale: %f m/px", file.scale);
}

bool readTile(uint32_t xoff, uint32_t yoff, uint32_t xsize, uint32_t ysize, file_t& file, uint32_t * output)
{
    GDALDataset * dataset = file.dataset;
    int32_t rasterBandCount = dataset->GetRasterCount();

    memset(output,-1, sizeof(uint32_t) * xsize * ysize);

    if(rasterBandCount == 1)
    {
        GDALRasterBand * pBand;
        pBand = dataset->GetRasterBand(1);
        if(pBand->RasterIO(GF_Read,(int)xoff,(int)yoff, xsize, ysize, tileBuf8Bit,xsize,ysize,GDT_Byte,0,0) == CE_Failure)
        {
            return false;
        }

        for(unsigned int i = 0; i < (xsize * ysize); i++)
        {
            output[i] = file.colortable[tileBuf8Bit[i]];
        }
    }
    else
    {
        for(int b = 1; b <= rasterBandCount; ++b)
        {
            GDALRasterBand * pBand;
            pBand = dataset->GetRasterBand(b);

            uint32_t mask = ~(0x000000FF << (8*(b-1)));

            if(pBand->RasterIO(GF_Read,(int)xoff,(int)yoff, xsize, ysize, tileBuf8Bit,xsize,ysize,GDT_Byte,0,0) == CE_Failure)
            {
                return false;
            }

            for(unsigned int i = 0; i < (xsize * ysize); i++)
            {
                uint32_t pixel = output[i];

                pixel &= mask;
                pixel |= tileBuf8Bit[i] << (8*(b-1));
                output[i] = pixel;
            }
        }
    }

    return true;
}



static void init_destination (j_compress_ptr cinfo)
{
    jpgbuf.resize(JPG_BLOCK_SIZE);
    cinfo->dest->next_output_byte   = &jpgbuf[0];
    cinfo->dest->free_in_buffer     = jpgbuf.size();
}

static boolean empty_output_buffer (j_compress_ptr cinfo)
{
    size_t oldsize = jpgbuf.size();
    jpgbuf.resize(oldsize + JPG_BLOCK_SIZE);
    cinfo->dest->next_output_byte   = &jpgbuf[oldsize];
    cinfo->dest->free_in_buffer     = jpgbuf.size() - oldsize;
    return true;
}

static void term_destination (j_compress_ptr cinfo)
{
    jpgbuf.resize(jpgbuf.size() - cinfo->dest->free_in_buffer);
}


static uint32_t writeTile(uint32_t xsize, uint32_t ysize, uint32_t * raw_image, FILE * fid, int quality, int subsampling)
{
    uint32_t size = 0;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];

    jpeg_destination_mgr destmgr    = {0};
    destmgr.init_destination        = init_destination;
    destmgr.empty_output_buffer     = empty_output_buffer;
    destmgr.term_destination        = term_destination;

    // convert from RGBA to RGB
    for(uint32_t r = 0; r < ysize; r++)
    {
        for(uint32_t c = 0; c < xsize; c++)
        {
            uint32_t pixel = raw_image[r * xsize + c];
            tileBuf24Bit[r * xsize * 3 + c * 3]     =  pixel        & 0x0FF;
            tileBuf24Bit[r * xsize * 3 + c * 3 + 1] = (pixel >>  8) & 0x0FF;
            tileBuf24Bit[r * xsize * 3 + c * 3 + 2] = (pixel >> 16) & 0x0FF;
        }
    }

    cinfo.err = jpeg_std_error( &jerr );
    jpeg_create_compress(&cinfo);

    cinfo.dest              = &destmgr;
    cinfo.image_width       = xsize;
    cinfo.image_height      = ysize;
    cinfo.input_components  = 3;
    cinfo.in_color_space    = JCS_RGB;

    jpeg_set_defaults( &cinfo );

    if (subsampling != -1)
    {
        switch (subsampling)
        {
        case 422:  // 2x1, 1x1, 1x1 (4:2:2) : Medium
            {
                cinfo.comp_info[0].h_samp_factor = 2;
                cinfo.comp_info[0].v_samp_factor = 1;
                cinfo.comp_info[1].h_samp_factor = 1;
                cinfo.comp_info[1].v_samp_factor = 1;
                cinfo.comp_info[2].h_samp_factor = 1;
                cinfo.comp_info[2].v_samp_factor = 1;
                break;
            }
        case 411:  // 2x2, 1x1, 1x1 (4:1:1) : High
            {
                cinfo.comp_info[0].h_samp_factor = 2;
                cinfo.comp_info[0].v_samp_factor = 2;
                cinfo.comp_info[1].h_samp_factor = 1;
                cinfo.comp_info[1].v_samp_factor = 1;
                cinfo.comp_info[2].h_samp_factor = 1;
                cinfo.comp_info[2].v_samp_factor = 1;
                break;
            }
        case 444:  // 1x1 1x1 1x1 (4:4:4) : None
            {
                cinfo.comp_info[0].h_samp_factor = 1;
                cinfo.comp_info[0].v_samp_factor = 1;
                cinfo.comp_info[1].h_samp_factor = 1;
                cinfo.comp_info[1].v_samp_factor = 1;
                cinfo.comp_info[2].h_samp_factor = 1;
                cinfo.comp_info[2].v_samp_factor = 1;
                break;
            }
        }
    }

    if (quality != -1)
    {
        jpeg_set_quality( &cinfo, quality, TRUE );
    }

    jpeg_start_compress( &cinfo, TRUE );

    while( cinfo.next_scanline < cinfo.image_height )
    {
        row_pointer[0] = (JSAMPLE*)&tileBuf24Bit[ cinfo.next_scanline * cinfo.image_width *  cinfo.input_components];
        jpeg_write_scanlines( &cinfo, row_pointer, 1 );
    }
    /* similar to read file, clean up after we're done compressing */
    jpeg_finish_compress( &cinfo );
    jpeg_destroy_compress( &cinfo );

    // write data to output file
    size = jpgbuf.size() - 2;
    fwrite(&jpgbuf[2], size, 1, fid);

    return size;
}

static double distance(const double u1, const double v1, const double u2, const double v2)
{
    double dU = u2 - u1; // lambda
    double dV = v2 - v1; // roh

    double d = 2*asin(sqrt(sin(dV/2) * sin(dV/2) + cos(v1) * cos(v2) * sin(dU/2) * sin(dU/2)));

    return 6371010 * d;
}

static uint32_t scale2jnx(double scale)
{
    /*
    Ok, I've made some calculations, and got the following formula to
    calculate the JNX scale (S) depending on the map's meters/pixel
    ratio (R):

      S(R) =
        qRound(
          76437 *
          exp(
            ln(2.000032708011) *
            qRound(
              ln(R * 130.2084 / 76437) /
              ln(2.000032708011)
            )
          )
        )


    where
      qRound - is a function which returns the closest integer from
        floating point value, [unfortunately its defined in C99 but not standard C++]
      exp - exponent,
      ln - natural logarithm.

    Magic number 130.2084 - is an average value for
      (JNX scale) / (maps meters per pixel)
    ratio among all zoom levels in metric system.

    Magic number 2.000032708011 is a ratio on which our standard scale
    table is built. It is (76437 / 4777) ^ (1/4).
    */

    return (uint32_t)floor(0.5 + 76437 * exp(log(2.000032708011) * floor(0.5 + log(scale * 10 * 130.2084 / 76437) / log(2.000032708011) ) ) );
}

static char randChar()
{
    char buf[2];
#if defined(HAVE_ARC4RANDOM)
    int r = (int)((arc4random() * 16.0) / UINT_MAX);
#else
    int r = (int)((rand() * 16.0) / RAND_MAX);
#endif
    sprintf(buf,"%X", r & 0x0F);
    return buf[0];
}

static void createGUID(char * guid)
{
#if !defined(HAVE_ARC4RANDOM)
    srand((unsigned int)time(0));
#endif

    guid[0]     = randChar();
    guid[1]     = randChar();
    guid[2]     = randChar();
    guid[3]     = randChar();
    guid[4]     = randChar();
    guid[5]     = randChar();
    guid[6]     = randChar();
    guid[7]     = randChar();
    guid[8]     = '-';
    guid[9]     = randChar();
    guid[10]    = randChar();
    guid[11]    = randChar();
    guid[12]    = randChar();
    guid[13]    = '-';
    guid[14]    = randChar();
    guid[15]    = randChar();
    guid[16]    = randChar();
    guid[17]    = randChar();
    guid[18]    = '-';
    guid[19]    = randChar();
    guid[20]    = randChar();
    guid[21]    = randChar();
    guid[22]    = randChar();
    guid[23]    = '-';
    guid[24]    = randChar();
    guid[25]    = randChar();
    guid[26]    = randChar();
    guid[27]    = randChar();
    guid[28]    = randChar();
    guid[29]    = randChar();
    guid[30]    = randChar();
    guid[31]    = randChar();
    guid[32]    = randChar();
    guid[33]    = randChar();
    guid[34]    = randChar();
    guid[35]    = randChar();
    guid[36]    = 0;

}

/// this code is from the GDAL project
static void printProgress(int current, int total)
{
    double dfComplete = double(current)/double(total);

    static int nLastTick = -1;
    int nThisTick = (int) (dfComplete * 40.0);

    nThisTick = MIN(40,MAX(0,nThisTick));

    // Have we started a new progress run?
    if( nThisTick < nLastTick && nLastTick >= 39 )
    {
        nLastTick = -1;
    }

    if( nThisTick <= nLastTick )
    {
        return;
    }

    while( nThisTick > nLastTick )
    {
        nLastTick++;
        if( nLastTick % 4 == 0 )
        {
            fprintf( stdout, "%d", (nLastTick / 4) * 10 );
        }
        else
        {
            fprintf( stdout, "." );
        }
    }

    if( nThisTick == 40 )
    {
        fprintf( stdout, " - done.\n" );
    }
    else
    {
        fflush( stdout );
    }

}


int main(int argc, char ** argv)
{
    uint16_t tmp16;
    const uint8_t dummy = 0;
    uint32_t tileTableStart = 0;
    uint32_t tileCnt    = 0;
    uint32_t tilesTotal = 0;
    char projstr[1024];
    OGRSpatialReference oSRS;
    int quality         = -1;
    int subsampling     = -1;

    const char *copyright = "Unknown";
    const char *subscname = "BirdsEye";
    const char *mapname   = "Unknown";

    char *copyright_buf = NULL;
    char *subscname_buf = NULL;
    char *mapname_buf   = NULL;

    std::vector<int> forced_scale_values;

    printf("\n****** %s ******\n", WHAT_STR);

    if(argc < 2)
    {
        fprintf(stderr,"\nusage: qmt_map2jnx -q <1..100> -s <411|422|444> -p <0..> -c \"copyright notice\" -m \"BirdsEye\" -n \"Unknown\" -x file1_scale,file2_scale,...,fileN_scale <file1> <file2> ... <fileN> <outputfile>\n");
        fprintf(stderr,"\n");
        fprintf(stderr,"  -q The JPEG quality from 1 to 100. Default is 75 \n");
        fprintf(stderr,"  -s The chroma subsampling. Default is 411  \n");
        fprintf(stderr,"  -p The product ID. Default is 0  \n");
        fprintf(stderr,"  -c The copyright notice. Default is \"Unknown\"  \n");
        fprintf(stderr,"  -m The subscription product name. Default is \"BirdsEye\"  \n");
        fprintf(stderr,"  -n The map name. Default is \"Unknown\"  \n");
        fprintf(stderr,"  -z The z order (drawing order). Default is 25\n");
        fprintf(stderr,"  -x Override levels scale. Default: autodetect\n");
        fprintf(stderr,"\n");
        fprintf(stderr,"\nThe projection of the input files must have the same latitude along");
        fprintf(stderr,"\na pixel row. Mecator and Longitude/Latitude projections match this");
        fprintf(stderr,"\nthis property. Transversal Merkator or Lambert projections do not.");
        fprintf(stderr,"\n");
        fprintf(stderr,"\nTo rectify a geotiff map, you can use the gdalwarp command, e.g.");
        fprintf(stderr,"\ngdalwarp -t_srs \"EPSG:4326\" <inputfile> <outputfile>");
        fprintf(stderr,"\n");
        fprintf(stderr,"Scale levels must be pass in same order as level files pointed.\n");
        fprintf(stderr,"Empty and zero values equal to autodetect. We can point only needed\n");
        fprintf(stderr,"levels, like:\n");
        fprintf(stderr,"  -x 45356,,,75; -x ,,,,75\n");
        fprintf(stderr,"Calculated levels table can be found:\n");
        fprintf(stderr,"  English: http://whiter.brinkster.net/en/JNX.shtml\n");
        fprintf(stderr,"  Russian: http://whiter.brinkster.net/JNX.shtml\n");
        fprintf(stderr,"Most common values for different map scales:\n");
        fprintf(stderr,"  JNX scale              Map scale\n");
        fprintf(stderr,"  -------------          ---------\n");
        fprintf(stderr,"  78125-31250            1:1 000 000\n");
        fprintf(stderr,"  20834-7813             1:500 000\n");
        fprintf(stderr,"  7813-3125              1:200 000\n");
        fprintf(stderr,"  3125-2084              1:100 000\n");
        fprintf(stderr,"  2084-782               1:50 000\n");
        fprintf(stderr,"  782-32                 1:25 000\n");
        fprintf(stderr,"  32-21                  1:10 000\n");
        fprintf(stderr,"  21-14                  1:5000, 1:2000\n");
        fprintf(stderr,"\n");
        fprintf(stderr,"\n");
        exit(-1);
    }

    GDALAllRegister();
    wgs84 = pj_init_plus("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs");

    // read geo information from input files
    //files.resize(argc - 2);
    int skip_next_arg = 0;
    int files_count = 0;

    for(int i = 1; i < (argc - 1); i++)
    {
        if (skip_next_arg)
        {
            skip_next_arg = 0;
            continue;
        }

        if (argv[i][0] == '-')
        {
            if (towupper(argv[i][1]) == 'Q')
            {
                quality = atol(argv[i+1]);
                skip_next_arg = 1;
                continue;
            }
            else if (towupper(argv[i][1]) == 'S')
            {
                subsampling = atol(argv[i+1]);
                skip_next_arg = 1;
                continue;
            }
            else if (towupper(argv[i][1]) == 'P')
            {
                jnx_hdr.productId = atol(argv[i+1]);
                skip_next_arg = 1;
                continue;
            }
            else if (towupper(argv[i][1]) == 'C')
            {
                copyright = copyright_buf = get_argv(i + 1, argv);
                skip_next_arg = 1;
                continue;
            }
            else if (towupper(argv[i][1]) == 'M')
            {
                subscname = subscname_buf = get_argv(i + 1, argv);
                skip_next_arg = 1;
                continue;
            }
            else if (towupper(argv[i][1]) == 'N')
            {
                mapname = mapname_buf = get_argv(i + 1, argv);
                skip_next_arg = 1;
                continue;
            }
            else if (towupper(argv[i][1]) == 'Z')
            {
                jnx_hdr.zorder = atol(argv[i+1]);
                skip_next_arg = 1;
                continue;
            }
            else if (towupper(argv[i][1]) == 'X')
            {
                skip_next_arg = 1;

                std::string scales_buf(get_argv(i + 1, argv));
                size_t pos = 0;
                size_t last_pos = 0;

                pos = scales_buf.find_first_of(',');
                std::string val;
                while (pos != std::string::npos)
                {
                    val = scales_buf.substr(last_pos, pos - last_pos);
                    last_pos = pos + 1;
                    pos = scales_buf.find_first_of(',', pos + 1);

                    //printf("val: %s : %d\n", val.c_str(), pos);
                    forced_scale_values.push_back(atol(val.c_str()));
                }
                val = scales_buf.substr(last_pos, pos);
                //printf("val: %s : %d\n", val.c_str(), pos);
                forced_scale_values.push_back(atol(val.c_str()));

                continue;
            }

        }

        files_count++;
        files.resize(files_count);

        double dist;

        GDALDataset * dataset = (GDALDataset*)GDALOpen(argv[i],GA_ReadOnly);
        if(dataset == 0)
        {
            fprintf(stderr,"\nFailed to open %s\n", argv[i]);
            exit(-1);
        }

        projPJ   pj;
        const char * wkt = projstr;

        if(dataset->GetProjectionRef())
        {
            strncpy(projstr,dataset->GetProjectionRef(),sizeof(projstr));
        }
        oSRS.importFromWkt(&wkt);

        char *proj4 = nullptr;
        oSRS.exportToProj4(&proj4);

        pj = pj_init_plus(proj4);
        if(pj == 0)
        {
            fprintf(stderr,"\nUnknown projection in file %s\n", argv[i]);
            exit(-1);
        }

        double adfGeoTransform[6];
        dataset->GetGeoTransform( adfGeoTransform );

        std::list<file_t>::iterator f = files.begin();
        std::advance(f, files_count - 1);

        file_t& file    = *f;
        file.filename   = argv[i];
        file.projection = proj4;
        file.dataset    = dataset;
        file.pj         = pj;
        file.width      = dataset->GetRasterXSize();
        file.height     = dataset->GetRasterYSize();
        file.xscale     = adfGeoTransform[1];
        file.yscale     = adfGeoTransform[5];
        file.xref1      = adfGeoTransform[0];
        file.yref1      = adfGeoTransform[3];
        file.xref2      = file.xref1 + file.width  * file.xscale;
        file.yref2      = file.yref1 + file.height * file.yscale;

        if(pj_is_latlong(file.pj))
        {
            file.lon1 = file.xref1;
            file.lat1 = file.yref1;
            file.lon2 = file.xref2;
            file.lat2 = file.yref2;
        }
        else
        {
            file.lon1 = file.xref1;
            file.lat1 = file.yref1;
            file.lon2 = file.xref2;
            file.lat2 = file.yref2;

            pj_transform(pj,wgs84,1,0,&file.lon1,&file.lat1,0);
            pj_transform(pj,wgs84,1,0,&file.lon2,&file.lat2,0);

            file.lon1 *= RAD_TO_DEG;
            file.lat1 *= RAD_TO_DEG;
            file.lon2 *= RAD_TO_DEG;
            file.lat2 *= RAD_TO_DEG;
        }

        dist = distance(file.lon1 * DEG_TO_RAD, file.lat1 * DEG_TO_RAD, file.lon2 * DEG_TO_RAD, file.lat1 * DEG_TO_RAD);
        file.scale = dist/file.width;

        // fill color table if necessary
        GDALRasterBand * pBand;
        pBand = dataset->GetRasterBand(1);

        if(pBand->GetColorInterpretation() == GCI_PaletteIndex)
        {
            GDALColorTable * pct = pBand->GetColorTable();
            for(int c=0; c < pct->GetColorEntryCount(); ++c)
            {
                const GDALColorEntry& e = *pct->GetColorEntry(c);
                file.colortable[c] = e.c1 | (e.c2 << 8) | (e.c3 << 16) | (e.c4 << 24);
            }
        }
        else if(pBand->GetColorInterpretation() ==  GCI_GrayIndex )
        {
            for(int c=0; c < 256; ++c)
            {
                file.colortable[c] = c | (c << 8) | (c << 16) | 0xFF000000;
            }
        }

        int success = 0;
        int idx = (int)pBand->GetNoDataValue(&success);

        if(success)
        {
            file.colortable[idx] &= 0x00FFFFFF;
        }
    }

    // apply sorted files to levels and extract file header data
    double right    = -180.0;
    double top      =  -90.0;
    double left     =  180.0;
    double bottom   =   90.0;

    double scale = 0.0;
    files.sort();
    std::list<file_t>::iterator f;
    for(f = files.begin(); f != files.end(); f++)
    {
        file_t& file = *f;
        prinfFileinfo(file);

        if(file.lon1 < left)    left   = file.lon1;
        if(file.lat1 > top)     top    = file.lat1;
        if(file.lat2 < bottom)  bottom = file.lat2;
        if(file.lon2 > right)   right  = file.lon2;

        if(scale != 0.0 && ((fabs(scale - file.xscale)) / scale) > 0.02)
        {
            nLevels++;
            if(nLevels > 4)
            {
                fprintf(stderr,"\nToo many different detail levels.\n");
                exit(-1);
            }
        }
        scale = file.xscale;

        levels[nLevels].files.push_back(&file);
    }
    nLevels++;

    FILE * fid = fopen(argv[argc-1],"wb");
    if(fid == 0)
    {
        fprintf(stderr,"\nFailed to create file %s\n", argv[argc-1]);
        exit(-1);
    }

    jnx_hdr.left    = (int32_t)((left   * 0x7FFFFFFF) / 180);
    jnx_hdr.top     = (int32_t)((top    * 0x7FFFFFFF) / 180);
    jnx_hdr.right   = (int32_t)((right  * 0x7FFFFFFF) / 180);
    jnx_hdr.bottom  = (int32_t)((bottom * 0x7FFFFFFF) / 180);

    jnx_hdr.details = nLevels;

    printf("\n\n======== map header ========");
    printf("\nmap area (top/left, bottom/right): %f %f, %f %f", left, top, right, bottom);
    printf("\n                                   %08X %08X, %08X %08X", jnx_hdr.left, jnx_hdr.top, jnx_hdr.right, jnx_hdr.bottom);
    printf("\nnumber of detail levels:           %i", jnx_hdr.details);
    printf("\nz-order:                           %i\n", jnx_hdr.zorder);


    for(int i=0; i<HEADER_BLOCK_SIZE; i++)
    {
        fwrite(&dummy, sizeof(dummy), 1, fid);
    }
    fseeko(fid,0,SEEK_SET);
    fwrite(&jnx_hdr, sizeof(jnx_hdr), 1, fid);

    // --------------------------------------------------------------
    // get all information to write the table of detail levels and the dummy tile table
    for(int i = 0; i < nLevels; i++)
    {
        uint32_t size   = 256;
        level_t& level  = levels[i];
        std::list<file_t *>::iterator f;
        double scale    = 0.0;

        while(size <= JNX_MAX_TILE_SIZE)
        {
            level.nTiles    = 0;
            level.tileSize  = size;
            for(f = level.files.begin(); f != level.files.end(); f++)
            {
                file_t& file  = *(*f);
                double xTiles = file.width  / double(size);
                double yTiles = file.height / double(size);
                level.nTiles += int(ceil(xTiles)) * int(ceil(yTiles));

                scale         = file.scale;
            }

            if(level.nTiles < JNX_MAX_TILES)
            {
                break;
            }
            size <<= 1;
        }


        level.offset    = tilesTotal * sizeof(jnx_tile_t) + HEADER_BLOCK_SIZE; // still has to be offset by complete header
        if (forced_scale_values.size() == 0 || (unsigned)i >= forced_scale_values.size() ||  forced_scale_values[i] == 0)
        {
            level.scale     = scale2jnx(scale);
        }
        else
        {
            level.scale     = forced_scale_values[i];
        }
        tilesTotal     += level.nTiles;

        fwrite(&level.nTiles, sizeof(level.nTiles), 1, fid);
        fwrite(&level.offset, sizeof(level.offset), 1, fid);
        fwrite(&level.scale, sizeof(level.scale), 1, fid);
        fwrite(&level.dummy, sizeof(level.dummy), 1, fid);
        fwrite(copyright, strlen(copyright) + 1, 1, fid);


        printf("\n    Level %i: % 5i tiles, offset %08X, scale: %i, %ix%i", i, level.nTiles, level.offset, level.scale, level.tileSize, level.tileSize);

    }

    // --------------------------------------------------------------
    // write map loader info block
    uint32_t blockVersion = 0x00000009;
    char GUID[40];
    createGUID(GUID);

    tmp16 = jnx_hdr.productId;

    fwrite(&blockVersion, sizeof(blockVersion), 1, fid);
    fwrite(GUID, 37, 1, fid);
    fwrite(subscname, strlen(subscname) + 1, 1, fid);
    fwrite(&dummy, sizeof(dummy), 1, fid);
    fwrite(&tmp16, sizeof(tmp16), 1, fid);
    fwrite(mapname, strlen(mapname) + 1, 1, fid);
    fwrite(&nLevels , sizeof(nLevels), 1, fid);
    for(int i = 1; i <= nLevels; i++)
    {
        char str[40];
        sprintf(str,"Level %i", i);
        fwrite(str, strlen(str) + 1, 1, fid);
        fwrite(str, strlen(str) + 1, 1, fid);
        fwrite(copyright, strlen(copyright) + 1, 1, fid);
        fwrite(&i,sizeof(i), 1, fid);
    }

    // --------------------------------------------------------------
    // write dummy tile table
    tileTableStart = HEADER_BLOCK_SIZE;
    fseeko(fid, tileTableStart, SEEK_SET);
    fwrite(tileTable, sizeof(jnx_tile_t), tilesTotal, fid);

    // --------------------------------------------------------------
    // read tiles from input files and write jpeg coded tiles to output file
    printf("\n\nStart conversion:\n");
    for(int l = 0; l < nLevels; l++)
    {
        level_t& level = levels[l];

        std::list<file_t *>::iterator f;
        for(f = level.files.begin(); f != level.files.end(); f++)
        {
            file_t& file  = *(*f);

            uint32_t xoff = 0;
            uint32_t yoff = 0;

            uint32_t xsize = level.tileSize;
            uint32_t ysize = level.tileSize;

            while(yoff < file.height)
            {
                if(ysize > (file.height - yoff))
                {
                    ysize = file.height - yoff;
                }

                xsize = level.tileSize;
                xoff  = 0;

                while(xoff < file.width)
                {
                    if(xsize > (file.width - xoff))
                    {
                        xsize = (file.width - xoff);
                    }

                    // //
                    if(!readTile(xoff, yoff, xsize, ysize, file, tileBuf32Bit))
                    {
                        fprintf(stderr,"\nError reading tiles from map file\n");
                        exit(-1);
                    }

                    jnx_tile_t& tile = tileTable[tileCnt++];
                    if(pj_is_latlong(file.pj))
                    {

                        double u1 = file.lon1 + xoff * file.xscale;
                        double v1 = file.lat1 + yoff * file.yscale;
                        double u2 = file.lon1 + (xoff + xsize) * file.xscale;
                        double v2 = file.lat1 + (yoff + ysize) * file.yscale;


                        tile.left   = (int32_t)(u1 * 0x7FFFFFFF / 180);
                        tile.top    = (int32_t)(v1 * 0x7FFFFFFF / 180);
                        tile.right  = (int32_t)(u2 * 0x7FFFFFFF / 180);
                        tile.bottom = (int32_t)(v2 * 0x7FFFFFFF / 180);

                    }
                    else
                    {
                        double u1 = file.xref1 + xoff * file.xscale;
                        double v1 = file.yref1 + yoff * file.yscale;
                        double u2 = file.xref1 + (xoff + xsize) * file.xscale;
                        double v2 = file.yref1 + (yoff + ysize) * file.yscale;

                        pj_transform(file.pj,wgs84,1,0,&u1,&v1,0);
                        pj_transform(file.pj,wgs84,1,0,&u2,&v2,0);

                        tile.left    = (int32_t)((u1 * RAD_TO_DEG) * 0x7FFFFFFF / 180);
                        tile.top     = (int32_t)((v1 * RAD_TO_DEG) * 0x7FFFFFFF / 180);
                        tile.right   = (int32_t)((u2 * RAD_TO_DEG) * 0x7FFFFFFF / 180);
                        tile.bottom  = (int32_t)((v2 * RAD_TO_DEG) * 0x7FFFFFFF / 180);
                    }

                    tile.width  = xsize;
                    tile.height = ysize;
                    tile.offset = (uint32_t)(ftello(fid) & 0x0FFFFFFFF);
                    tile.size   = writeTile(xsize, ysize, tileBuf32Bit, fid, quality, subsampling);

                    printProgress(tileCnt, tilesTotal);
                    // //
                    xoff += xsize;
                }

                yoff += ysize;
            }
        }
    }

    // terminate output file
    fwrite("BirdsEye", 8, 1, fid);

    // write final tile table
    fseeko(fid, tileTableStart, SEEK_SET);
    fwrite(tileTable, sizeof(jnx_tile_t), tilesTotal, fid);
    // done
    fclose(fid);

    // clean up
    pj_free(wgs84);
    GDALDestroyDriverManager();
    if (copyright_buf)
        free(copyright_buf);
    if (subscname_buf)
        free(subscname_buf);
    if (mapname_buf)
        free(mapname_buf);
    printf("\n\n");
    return 0;
}
