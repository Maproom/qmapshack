/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#define DB_VERSION 6

#define NO_CMD ((void)0)

#define QUERY_EXEC(cmd) \
    if(!query.exec()) \
    { \
        qWarning() << "Execution of SQL-Statement `" << query.lastQuery() << "` failed:"; \
        qWarning() << query.lastError(); \
        cmd; \
    }

#define QUERY_RUN(stmt, cmd) \
    if(!query.exec(stmt)) \
    { \
        qWarning() << "Execution of SQL-Statement `" << query.lastQuery() << "` failed:"; \
        qWarning() << query.lastError(); \
        cmd; \
    }

#endif //MACROS_H

