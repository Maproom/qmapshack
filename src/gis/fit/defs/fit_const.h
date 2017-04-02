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

#include <QtCore>

/*
 * 0: does not print any decoded fit messages and definitions
 * 1: prints fit messages and definitions after finishing decoding
 * 2: 1 + prints fit messages and definitions during decoding (just after finishing one)
 */
#define FITDEBUGLVL 2

#define FITDEBUG(level, cmd) if(FITDEBUGLVL >= level) { cmd; }

static const quint8 fitLocalMesgNrInvalid = 255;
static const quint16 fitGlobalMesgNrInvalid = 0xffff;
static const quint8 fitFieldDefNrInvalid = 255;

typedef enum
{
    eFitArchEndianLittle = 0,
    eFitArchEndianBig = 1
}fit_arch_e;

#endif // FIT_CONST_H
