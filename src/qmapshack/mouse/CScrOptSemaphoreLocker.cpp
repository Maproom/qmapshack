/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "mouse/CScrOptSemaphoreLocker.h"
#include "mouse/IScrOpt.h"

#include <QDebug>

CScrOptSemaphoreLocker::CScrOptSemaphoreLocker(IScrOpt& scropt)
    : scropt(scropt)
{
    Q_ASSERT_X(scropt.semaphore.available(), "semaphore", "You try to acquire the on-screen option semaphore that is already acquired.");
    scropt.semaphore.acquire(1);
}


CScrOptSemaphoreLocker::~CScrOptSemaphoreLocker()
{
    scropt.semaphore.release(1);
}
