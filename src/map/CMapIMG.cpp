/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "map/CMapIMG.h"
#include "CFileExt.h"
#include "Platform.h"

#include <QtWidgets>

#define DEBUG_SHOW_SECT_DESC

CMapIMG::CMapIMG(const QString &filename, CMap *parent)
    : IMap(parent)
    , filename(filename)
{
    qDebug() << "------------------------------";
    qDebug() << "IMG: try to open" << filename;

    try
    {
        readBasics();
    }
    catch(const exce_t& e)
    {
        QMessageBox::critical(0, tr("Failed ..."), e.msg, QMessageBox::Abort);
    }


    isActivated = true;
}

CMapIMG::~CMapIMG()
{

}

void CMapIMG::readFile(CFileExt& file, quint32 offset, quint32 size, QByteArray& data)
{
    if(offset + size > file.size())
    {
        throw exce_t(eErrOpen, tr("Failed to read: ") + filename);
    }

    data = QByteArray::fromRawData(file.data(offset), size);
    // wenn mask == 0 ist kein xor noetig
    if(mask == 0)
    {
        return;
    }

#ifdef HOST_IS_64_BIT
    quint64 * p64 = (quint64*)data.data();
    for(quint32 i = 0; i < size/8; ++i)
    {
        *p64++ ^= mask64;
    }
    quint32 rest = size % 8;
    quint8 * p = (quint8*)p64;
#else
    quint32 * p32 = (quint32*)data.data();
    for(quint32 i = 0; i < size/4; ++i)
    {
        *p32++ ^= mask32;
    }
    quint32 rest = size % 4;
    quint8 * p = (quint8*)p32;
#endif

    for(quint32 i = 0; i < rest; ++i)
    {
        *p++ ^= mask;
    }

}


bool CMapIMG::readBasics()
{
    char tmpstr[64];
    qint64 fsize    = QFileInfo(filename).size();

    CFileExt file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        throw exce_t(eErrOpen, tr("Failed to open: ") + filename);
    }

    mask = (quint8)*file.data(0);

    mask32   = mask;
    mask32 <<= 8;
    mask32  |= mask;
    mask32 <<= 8;
    mask32  |= mask;
    mask32 <<= 8;
    mask32  |= mask;

    mask64   = mask32;
    mask64 <<= 32;
    mask64  |= mask32;

    // read hdr_img_t
    QByteArray imghdr;
    readFile(file, 0, sizeof(hdr_img_t), imghdr);
    hdr_img_t * pImgHdr = (hdr_img_t*)imghdr.data();

    if(strncmp(pImgHdr->signature,"DSKIMG",7) != 0)
    {
        throw exce_t(errFormat,tr("Bad file format: ") + filename);
    }
    if(strncmp(pImgHdr->identifier,"GARMIN",7) != 0)
    {
        throw exce_t(errFormat,tr("Bad file format: ") + filename);
    }

    mapdesc  = QByteArray((const char*)pImgHdr->desc1,20);
    mapdesc += pImgHdr->desc2;
    qDebug() << mapdesc;

    size_t blocksize = pImgHdr->blocksize();

    // 1st read FAT
    QByteArray FATblock;
    readFile(file, sizeof(hdr_img_t), sizeof(FATblock_t), FATblock);
    const FATblock_t * pFATBlock = (const FATblock_t * )FATblock.data();

    size_t dataoffset = sizeof(hdr_img_t);

    // skip dummy blocks at the beginning
    while(dataoffset < (size_t)fsize)
    {
        if(pFATBlock->flag != 0x00)
        {
            break;
        }
        dataoffset += sizeof(FATblock_t);
        readFile(file, dataoffset, sizeof(FATblock_t), FATblock);
        pFATBlock = (const FATblock_t * )FATblock.data();
    }

    // start of new subfile part
    /*
        It is taken for granted that the single subfile parts are not
        fragmented within the file. Thus it is not really neccessary to
        store and handle all block sequence numbers. Just the first one
        will give us the offset. This also implies that it is not necessary
        to care about FAT blocks with a non-zero part number.

        2007-03-31: Garmin's world base map seems to be coded different.
                    The part field seems to be rather a bit field than
                    a part number. As the total subfile size is given
                    for the first part only (for all otheres it's zero)
                    I use it to identify the 1st part of a subfile

        2007-05-26: Gmapsupp images by Sendmap code quite some bull shit,
                    too. The size is stored in every part and they do have
                    a part number. I introduced a set of subfile names
                    storing the subfile's name and type. The first part
                    with a size info and it's name / type not stored in the
                    set is used to get the location information.
    */
    QSet<QString> subfileNames;
    while(dataoffset < (size_t)fsize)
    {
        if(pFATBlock->flag != 0x01)
        {
            break;
        }

        memcpy(tmpstr,pFATBlock->name,sizeof(pFATBlock->name) + sizeof(pFATBlock->type));
        tmpstr[sizeof(pFATBlock->name) + sizeof(pFATBlock->type)] = 0;

        if(gar_load(uint32_t, pFATBlock->size) != 0 && !subfileNames.contains(tmpstr) && tmpstr[0] != 0x20)
        {
            subfileNames << tmpstr;

            memcpy(tmpstr,pFATBlock->name,sizeof(pFATBlock->name));
            tmpstr[sizeof(pFATBlock->name)] = 0;

            // skip MAPSORC.MPS section
            if(strcmp(tmpstr,"MAPSOURC") && strcmp(tmpstr,"SENDMAP2"))
            {

                subfile_desc_t& subfile = subfiles[tmpstr];
                subfile.name = tmpstr;

                memcpy(tmpstr,pFATBlock->type,sizeof(pFATBlock->type));
                tmpstr[sizeof(pFATBlock->type)] = 0;

                subfile_part_t& part = subfile.parts[tmpstr];
                part.size   = gar_load(uint32_t, pFATBlock->size);
                part.offset = gar_load(uint16_t, pFATBlock->blocks[0]) * blocksize;
            }

        }

        dataoffset += sizeof(FATblock_t);
        readFile(file, dataoffset, sizeof(FATblock_t), FATblock);
        pFATBlock = (const FATblock_t * )FATblock.data();
    }

    if((dataoffset == sizeof(hdr_img_t)) || (dataoffset >= (size_t)fsize))
    {
        throw exce_t(errFormat,tr("Failed to read file structure: ") + filename);
    }

    // gmapsupp.img files do not have a data offset field
    if(gar_load(uint32_t, pImgHdr->dataoffset) == 0)
    {
        pImgHdr->dataoffset = gar_load(uint32_t, dataoffset);
    }

    // sometimes there are dummy blocks at the end of the FAT
    if(gar_load(uint32_t, pImgHdr->dataoffset) != dataoffset)
    {
        dataoffset = gar_load(uint32_t, pImgHdr->dataoffset);
    }

#ifdef DEBUG_SHOW_SECT_DESC
    {
        QMap<QString,subfile_desc_t>::const_iterator subfile = subfiles.begin();
        while(subfile != subfiles.end())
        {
            qDebug() << "--- subfile" << subfile->name << "---";
            QMap<QString,subfile_part_t>::const_iterator part = subfile->parts.begin();
            while(part != subfile->parts.end())
            {
                qDebug() << part.key() << hex << part->offset << part->size;
                ++part;
            }
            ++subfile;
        }
    }
#endif                       //DEBUG_SHOW_SECT_DESC

}

void CMapIMG::draw(buffer_t& buf)
{

}
