/**********************************************************************************************
    Copyright (C) 2026 Gert Pellin <gert@pellin.be>

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

#include "gis/trk/surface/CSurfaceWidget.h"

#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>
#include <algorithm>

#include "gis/trk/surface/CSurfaceBar.h"
#include "helpers/CSettings.h"
#include "units/IUnit.h"

CSurfaceWidget::CSurfaceWidget(QWidget* parent) : QWidget(parent) {
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(3, 3, 3, 3);
  layout->setSpacing(3);

  labelHint = new QLabel(this);
  labelHint->setWordWrap(true);
  labelHint->setTextFormat(Qt::RichText);
  connect(labelHint, &QLabel::linkActivated, this, &CSurfaceWidget::sigSetup);
  layout->addWidget(labelHint);

  auto addBar = [&](QLabel*& title, CSurfaceBar*& bar, QLabel*& legend, const QString& text) {
    title = new QLabel("<b>" + text + "</b>", this);
    bar = new CSurfaceBar(this);
    legend = new QLabel(this);
    legend->setTextFormat(Qt::RichText);
    layout->addWidget(title);
    layout->addWidget(bar);
    layout->addWidget(legend);
  };
  addBar(labelClasses, barClasses, legendClasses, tr("Surfaces"));
  addBar(labelWays, barWays, legendWays, tr("Way types"));

  toolDetails = new QToolButton(this);
  toolDetails->setText(tr("Surfaces in detail"));
  toolDetails->setCheckable(true);
  toolDetails->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  toolDetails->setArrowType(Qt::RightArrow);
  toolDetails->setAutoRaise(true);
  layout->addWidget(toolDetails);

  labelDetails = new QLabel(this);
  labelDetails->setTextFormat(Qt::RichText);
  layout->addWidget(labelDetails);

  layout->addStretch();

  SETTINGS;
  toolDetails->setChecked(cfg.value("TrackDetails/surfaceDetails", false).toBool());
  slotDetails(toolDetails->isChecked());
  connect(toolDetails, &QToolButton::toggled, this, &CSurfaceWidget::slotDetails);
}

void CSurfaceWidget::slotDetails(bool show) {
  toolDetails->setArrowType(show ? Qt::DownArrow : Qt::RightArrow);
  labelDetails->setVisible(show && toolDetails->isVisible());
  SETTINGS;
  cfg.setValue("TrackDetails/surfaceDetails", show);
}

static QString km(qreal meter) {
  QString val, unit;
  IUnit::self().meter2distance(meter, val, unit);
  return val + "&nbsp;" + unit;
}

// one table row: color, name, length, share
static QString legendItem(const QColor& color, const QString& name, qreal meter, qreal total) {
  return QString(
             "<tr><td><span style='color:%1'>&#9632;</span>&nbsp;</td><td nowrap>%2</td>"
             "<td align='right' nowrap>&nbsp;&nbsp;%3</td><td align='right' nowrap>&nbsp;&nbsp;%4%</td></tr>")
      .arg(color.name(), name.toHtmlEscaped(), km(meter))
      .arg(qRound(100 * meter / total));
}

void CSurfaceWidget::setData(const CSurfaceTrk& surface) {
  const CSurfaceTrk::state_e state = surface.getState();
  const bool ready = state == CSurfaceTrk::eStateReady && surface.getResult().total > 0;

  const QList<QWidget*> widgetsReady = {labelClasses, barClasses, legendClasses, labelWays,
                                        barWays,      legendWays, toolDetails};
  for (QWidget* w : widgetsReady) {
    w->setVisible(ready);
  }
  labelDetails->setVisible(ready && toolDetails->isChecked());

  switch (state) {
    case CSurfaceTrk::eStateNoDatabase:
      labelHint->setText(
          "<b>" + tr("Surfaces") + "</b><br/>" +
          tr("No surface database covers this track. Build one from an OpenStreetMap extract "
             "(.osm.pbf, e.g. from download.geofabrik.de) with %1.")
              .arg("<a href='setup'>" + tr("Tool") + " &rarr; " + tr("Create Surface Database") + "</a>"));
      labelHint->show();
      return;
    case CSurfaceTrk::eStateAnalyzing:
      labelHint->setText("<b>" + tr("Surfaces") + "</b><br/>" + tr("Analyzing the track..."));
      labelHint->show();
      return;
    case CSurfaceTrk::eStateNoTrack:
      labelHint->hide();
      return;
    default:
      labelHint->setVisible(!ready);
      labelHint->setText("<b>" + tr("Surfaces") + "</b><br/>" + tr("The track is too short."));
      break;
  }
  if (!ready) {
    return;
  }

  const CSurfaceAnalyzer::result_t& res = surface.getResult();

  QList<CSurfaceBar::part_t> parts;
  QStringList legend;
  for (qint32 i = 0; i < CSurface::eClassCount; i++) {
    const CSurface::class_e cls = CSurface::class_e(i);
    const qreal m = res.byClass[i];
    if (m <= 0) {
      continue;
    }
    const QString name = CSurface::className(cls);
    parts << CSurfaceBar::part_t{CSurface::classColor(cls), m,
                                 QString("%1: %2 %").arg(name).arg(qRound(res.percent(cls)))};
    legend << legendItem(CSurface::classColor(cls), name, m, res.total);
  }
  barClasses->setParts(parts);
  legendClasses->setText("<table cellspacing='0'>" + legend.join("") + "</table>");

  // way types sorted by length, like the classes they are few
  QVector<qint32> ways;
  for (qint32 i = 0; i < CSurface::eWayCount; i++) {
    if (res.byWay[i] > 0) {
      ways << i;
    }
  }
  std::stable_sort(ways.begin(), ways.end(), [&](qint32 a, qint32 b) { return res.byWay[a] > res.byWay[b]; });
  parts.clear();
  legend.clear();
  for (qint32 i : std::as_const(ways)) {
    const CSurface::waytype_e way = CSurface::waytype_e(i);
    const QString name = CSurface::wayTypeName(way);
    const qreal m = res.byWay[i];
    parts << CSurfaceBar::part_t{CSurface::wayTypeColor(way), m,
                                 QString("%1: %2 %").arg(name).arg(qRound(100 * m / res.total))};
    legend << legendItem(CSurface::wayTypeColor(way), name, m, res.total);
  }
  barWays->setParts(parts);
  legendWays->setText("<table cellspacing='0'>" + legend.join("") + "</table>");

  QVector<qint32> labels;
  for (qint32 i = 0; i < CSurface::eLabelCount; i++) {
    if (res.byLabel[i] > 0) {
      labels << i;
    }
  }
  std::stable_sort(labels.begin(), labels.end(), [&](qint32 a, qint32 b) { return res.byLabel[a] > res.byLabel[b]; });
  QString details = "<table>";
  for (qint32 i : std::as_const(labels)) {
    details += QString(
                   "<tr><td nowrap>%1</td><td align='right' nowrap>&nbsp;&nbsp;%2</td><td align='right' "
                   "nowrap>&nbsp;&nbsp;%3%</td></tr>")
                   .arg(CSurface::labelName(CSurface::label_e(i)).toHtmlEscaped(), km(res.byLabel[i]))
                   .arg(qRound(100 * res.byLabel[i] / res.total));
  }
  details += "</table>";
  labelDetails->setText(details);
}
