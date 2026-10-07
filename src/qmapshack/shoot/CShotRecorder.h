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

#ifndef CSHOTRECORDER_H
#define CSHOTRECORDER_H

#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QPoint>
#include <QSet>
#include <QString>
#include <functional>
#include <optional>

struct SShotFrame;
class CShotContext;
class QMenu;
class QWidget;

/**
   @brief What a writer does in the running application, as the steps of a scenario.

   A recording is the start state - window arrangement and map view - plus every change after it as steps. A handler
   turns what a control says into a step, kept only when the input being delivered went to that control; steps are
   ordered by that input. Never record during a replay: replayed input would be recorded.
 */
class CShotRecorder : public QObject {
  Q_OBJECT
 public:
  /** A press on a surface, held until its release decides which step it was. */
  struct gesture_t {
    quint64 frame = 0;    /**< the frame of the press, which is where the step is ordered */
    QJsonObject step;     /**< the step so far */
    QPoint pixel;         /**< where the press was, in the surface's coordinates */
    qint64 pressedAt = 0; /**< ms on now()'s monotonic clock, as CMouseAdapter times a press */
    QJsonObject moved;    /**< a `move` step to the pointer's offset from the press; empty until it moved */
    bool split = false;   /**< recorded as `press`, `move` and `release`, because a step came between */
  };

  explicit CShotRecorder(const CShotContext& ctx, QObject* parent = nullptr);
  virtual ~CShotRecorder();

  /** @brief Take the start state and watch the application; a previous recording is dropped. */
  void start();

  /**
     @brief Stop watching.

     @return the `layout` and `view` taken at start(), then the steps
   */
  QJsonArray stop();

  bool isRecording() const { return recording; }

  qsizetype steps() const { return entries.size(); }

  /**
     @brief Append @p step, which @p source made.

     Dropped unless the input being delivered went to @p source or, for an action, to a widget it sits in. The same verb
     from the same source in the same frame replaces the earlier step.
   */
  void record(QObject* source, const QJsonObject& step);

  /**
     @brief Update the step @p source is still making, or start it.

     Typing, a hover, a slider drag: one step holding where it ended, open until anything else is recorded, close() or
     stop().

     @param alsoThrough  a widget outside @p source whose input counts as @p source's, such as a completer popup
   */
  void amend(QObject* source, const QJsonObject& step, const QObject* alsoThrough = nullptr);

  /** @brief Nothing may be amended to @p source's step any more. */
  void close(QObject* source);

  /** @return the step @p source is still making with @p verb, nullptr when there is none */
  QJsonObject* openStep(QObject* source, const QString& verb);

  /**
     @brief Append @p step, which the input being delivered caused on @p source although it went elsewhere.

     For an effect a replay does not reproduce, such as an edit ended by a click elsewhere: a replayed click moves no
     focus.
   */
  void recordOutcome(QObject* source, const QJsonObject& step);

  /**
     @brief Take back the most recent step of @p source when @p matches says it is the one.

     For a step a later input turned out to be part of, such as the first click of a double click. Older steps are
     looked at only while @p passes lets the newer ones by.

     @param matches  gets the step, and whether it was made in the frame being delivered now
     @return true when a step was taken back
   */
  bool retractLast(QObject* source, const std::function<bool(const QJsonObject& step, bool sameFrame)>& matches,
                   const std::function<bool(const QJsonObject& step)>& passes = {});

  /** @brief A button went down on the surface @p source; the step it becomes is decided at its release. */
  void beginGesture(QObject* source, const QJsonObject& step, const QPoint& pixel);

  /** @return ms on the monotonic clock gestures are timed with */
  static qint64 now();

  /** @return the gesture held on @p source, nullptr when there is none */
  gesture_t* gesture(QObject* source);

  /**
     @brief The button went up: the gesture is a step, ordered where its press was.

     A gesture another step interrupted is split into `press` and `move` before that step and `release` now.
   */
  void endGesture(QObject* source);

  /** @return how a step names @p widget, nothing for a widget that cannot be named - reported once per class */
  std::optional<QString> address(const QWidget* widget) const;

  /**
     @brief The step that opens the context menu being asked for now, recording or not.

     @param owner  set to the widget the step is done to
     @return empty outside a context menu request
   */
  QJsonObject contextMenuStep(QWidget** owner = nullptr) const;

  const CShotContext& context() const { return ctx; }

 protected:
  /** @brief Watch what appears, hand a surface its input, and see a context menu answer a request */
  bool eventFilter(QObject* watched, QEvent* event) override;

 private:
  /** One step, and what decides where it ends up. */
  struct entry_t {
    quint64 frame = 0;         /**< the input it was made for; orders the recording */
    QObject* source = nullptr; /**< compared, never dereferenced; nullptr once gone */
    QString verb;
    QJsonObject step;
    bool open = false; /**< still being amended */
  };

  /** @return true when the input being delivered went to @p source, or to a widget the action @p source sits in */
  bool inputReached(const QObject* source) const;

  /** @brief Hand @p object to the handler of its class, once */
  void watch(QObject* object);

  /** @brief Hand every widget and action that exists to its handler */
  void watchAll();

  /** @brief Record that a context menu was asked for, now that one has answered */
  void recordContextMenu();

  /** @brief Record @p menu as opened by a tool button, a menu bar or another menu's entry */
  void recordMenuOpened(QMenu* menu);

  /** @brief Warn once per class that a press on @p widget records nothing */
  void reportUnhandled(const QWidget* widget);

  /** @brief Nothing may be amended to any step any more */
  void closeAll();

  /** @brief The input being delivered made a step; no key press of it is recorded besides */
  void touch();

  /** @brief A frame opened or closed; a key press no handler made a step of is recorded as itself */
  void onFrame(const SShotFrame& frame, bool opened);

  /** @brief Append the key press held back for a shortcut that did not claim it */
  void commitPendingKey();

  /** @brief A step interrupts a held gesture: record the gesture so far as steps */
  void splitGestures();

  /** @return the `release` step of @p gesture, at the offset from the press the pointer last was */
  static QJsonObject releaseOf(const gesture_t& gesture);

  const CShotContext& ctx;
  QList<entry_t> entries;
  QHash<QObject*, gesture_t> gestures;
  QJsonObject startLayout;
  QJsonObject startView;
  bool recording = false;
  /** Objects already handed to their handler; the connections outlive a recording, so start() must not repeat them. */
  QSet<QObject*> watched;
  /** The frame a `menu` step was recorded for; a submenu answers the same request again. */
  quint64 menuFrame = 0;
  /** Frames that made a step, so their key press is no step of its own. */
  QSet<quint64> touched;
  /** The key press step of each open key frame, recorded at its close unless the frame made a step. */
  QHash<quint64, QJsonObject> keyPresses;
  struct pendingKey_t {
    quint64 frame = 0;
    int key = 0;
    QJsonObject step;
  };
  /**
     A key press that made no step, held until another frame opens or its release closes: the Shortcut Qt delivers after
     it, or the release of Space on a button, may make the step instead.
   */
  std::optional<pendingKey_t> pendingKey;
  /** Classes already warned about. */
  mutable QSet<QString> reported;
};

#endif  // CSHOTRECORDER_H
