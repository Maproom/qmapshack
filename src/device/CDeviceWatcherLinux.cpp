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

#include "CDeviceWatcherLinux.h"

#include <asm/types.h>
#include <errno.h>
#include <linux/netlink.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <QtCore>
#include <QtNetwork>

#define UDEV_MONITOR_KERNEL     1
#define UEVENT_BUFFER_SIZE      4096

/*
    Most of the code is copied from QDeviceWatcher written by Wang Bin <wbsecg1@gmail.com>
 */

CDeviceWatcherLinux::CDeviceWatcherLinux(QObject *parent)
    : IDeviceWatcher(parent)
    , netlinkSocket(-1)
{
    int retval;
    struct sockaddr_nl sockAddr;
    const int buffersize = 16 * 1024 * 1024;

    memset(&sockAddr, 0x00, sizeof(struct sockaddr_nl));
    sockAddr.nl_family = AF_NETLINK;
    sockAddr.nl_pid = getpid();
    sockAddr.nl_groups = UDEV_MONITOR_KERNEL;

    netlinkSocket = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (netlinkSocket == -1)
    {
        qWarning("error getting socket: %s", strerror(errno));
        return;
    }

    setsockopt(netlinkSocket, SOL_SOCKET, SO_RCVBUFFORCE, &buffersize, sizeof(buffersize));
    retval = bind(netlinkSocket, (struct sockaddr*) &sockAddr, sizeof(struct sockaddr_nl));
    if (retval < 0)
    {
        qWarning("bind failed: %s", strerror(errno));
        close(netlinkSocket);
        netlinkSocket = -1;
        return;
    }
    else if (retval == 0)
    {
        //from libudev-monitor.c
        struct sockaddr_nl sockAddr1;
        socklen_t addrlen;

        /*
         * get the address the kernel has assigned us
         * it is usually, but not necessarily the pid
         */
        addrlen = sizeof(struct sockaddr_nl);
        getsockname(netlinkSocket, (struct sockaddr *)&sockAddr1, &addrlen);

        sockAddr.nl_pid = sockAddr1.nl_pid;
    }

    notifier = new QSocketNotifier(netlinkSocket, QSocketNotifier::Read, this);
    connect(notifier, SIGNAL(activated(int)), SLOT(slotParseDeviceInfo()));     //will always active
    notifier->setEnabled(true);
}

CDeviceWatcherLinux::~CDeviceWatcherLinux()
{
}

void CDeviceWatcherLinux::slotParseDeviceInfo()
{
    QByteArray data(UEVENT_BUFFER_SIZE, 0);

    size_t len = read(notifier->socket(), data.data(), UEVENT_BUFFER_SIZE);
    data.resize(len);
    data = data.replace(0, '\n').trimmed();

    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    while(!buffer.atEnd())
    {
        parseLine(buffer.readLine().trimmed());
    }
    buffer.close();
}

void CDeviceWatcherLinux::parseLine(const QByteArray& line)
{
    // only watching for block devices.
    if (!line.contains("/block/"))
    {
        return;
    }

    qDebug() << line;
}
