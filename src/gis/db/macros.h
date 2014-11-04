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

#ifndef MACROS_H
#define MACROS_H

#define DB_VERSION 1

#define QUERY_EXEC(cmd) \
if(!query.exec())\
{ \
    qDebug() << query.lastQuery();\
    qDebug() << query.lastError();\
    cmd;\
}\

#define PROGRESS_SETUP(lbl, max) \
QProgressDialog progress(lbl, "Abort", 0, max, 0);\
progress.setWindowModality(Qt::WindowModal);\

#define PROGRESS(x, cmd) \
progress.setValue(x); \
if (progress.wasCanceled()) cmd;\


#endif //MACROS_H

