/**********************************************************************************************
 Copyright (C) 2015 Ivo Kronenberg

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

#ifndef FIT_CONST_H
#define FIT_CONST_H

#include<QtCore>

static const uint8_t LocalMesgNrInvalid = 255;
static const uint16_t GlobalMesgNrInvalid = 0xffff;
static const uint8_t FieldDefNrInvalid = 255;

typedef enum {
    ArchEndianLittle = 0,
    ArchEndianBig = 1
}Arch;

#endif // FIT_CONST_H
