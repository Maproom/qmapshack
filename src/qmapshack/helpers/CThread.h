/**********************************************************************************************
    Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CTHREAD_H
#define CTHREAD_H

#include <QDeadlineTimer>
#include <QThread>

class CThread : public QThread {
 public:
  using fCallback = std::function<void(void)>;

  CThread(fCallback worker);
  virtual ~CThread() = default;

  void run() override;

  void cancel();

  /**
     @brief Stop a thread making blocking queued calls into an object, and wait for it

     Calls not delivered yet are dropped, as that releases the thread. Call on the receiver's thread. Without a
     deadline, never call it from inside such a call or while holding a lock the thread needs: the wait would not end.

     @param receiver    the object the thread calls into
     @param deadline    when to give up waiting
     @return True if the thread has finished, false if the deadline expired first
   */
  bool cancel(QObject* receiver, QDeadlineTimer deadline = QDeadlineTimer::Forever);

 private:
  fCallback worker;
};

#endif  // CTHREAD_H
