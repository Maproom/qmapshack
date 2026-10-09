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

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFileInfo>
#include <QTextStream>

#include "gis/trk/surface/CSurfaceDbBuilder.h"

// Build a QMapShack surface database without the GUI, e.g. next to a Routino database in a script.
int main(int argc, char** argv) {
  QCoreApplication app(argc, argv);
  QCoreApplication::setApplicationName("qms_surfacedb");
  QCoreApplication::setApplicationVersion(VER_STRING);

  QCommandLineParser parser;
  parser.setApplicationDescription("Build a QMapShack surface database (<name>." + CSurfaceDbBuilder::suffix() +
                                   ") from an OpenStreetMap extract. Put it in the directory set up in QMapShack's "
                                   "\"Create Surface Database\" tool.");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("source", "OSM extract (.osm.pbf)");
  parser.addPositionalArgument("target", "database file to write, e.g. Belgium." + CSurfaceDbBuilder::suffix());
  QCommandLineOption optName({"n", "name"}, "name of the region (default: from the target file)", "name");
  QCommandLineOption optQuiet({"q", "quiet"}, "no progress output");
  parser.addOption(optName);
  parser.addOption(optQuiet);
  parser.process(app);

  const QStringList args = parser.positionalArguments();
  if (args.size() != 2) {
    parser.showHelp(1);
  }

  CSurfaceDbBuilder::options_t opts;
  opts.pbf = args[0];
  opts.target = args[1];
  opts.name = parser.isSet(optName) ? parser.value(optName) : QFileInfo(opts.target).fileName().section('.', 0, 0);

  QTextStream err(stderr);
  const bool quiet = parser.isSet(optQuiet);
  qint32 last = -1;
  CSurfaceDbBuilder builder(opts);
  const bool ok = builder.build([&](qreal fraction, const QString& step) {
    const qint32 percent = qRound(fraction * 100);
    if (!quiet && percent != last) {
      last = percent;
      err << "\r" << step << " " << percent << "%   " << Qt::flush;
    }
  });
  if (!quiet) {
    err << "\n";
  }
  if (!ok) {
    err << "error: " << builder.errorString() << "\n";
    return 1;
  }
  const CSurfaceDbBuilder::stats_t& stats = builder.statistics();
  if (!quiet) {
    err << opts.target << ": " << stats.ways << " ways, " << stats.nodes << " nodes, " << stats.chunks << " chunks\n";
  }
  return 0;
}
