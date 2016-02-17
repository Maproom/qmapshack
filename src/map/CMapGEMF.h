#ifndef CMAPGEMF_H
#define CMAPGEMF_H
#define    NAMEBUFLEN 1024

#include "IMap.h"


class CMapGEMF : public IMap
{
    Q_OBJECT
public:
    CMapGEMF(const QString& filename, CMapDraw *parent);
    void draw(IDrawContext::buffer_t& buf) override;

private:
    QImage getTile(quint32 col, quint32 row,quint32 z);
    quint64 getFilenameFromAddress(quint64 offset,QString &filename);

     struct source_t {
      quint32 index;
      QString name;
     };

    struct gemffile_t {
        QString filename;
        quint64 size;

    };
    struct range_t {
        quint32 zoomlevel;
        quint32 minX;
        quint32 maxX;
        quint32 minY;
        quint32 maxY;
        quint32 sourceIdx;
        quint64 offset;
    };

    QString filename;
    quint32 version;
    quint32 tileSize;
    quint32 sourceNr;
    quint32 rangeNum;
    quint32 minZoom = 20;
    quint32 maxZoom = 0;
    QList< source_t> sources;
    QList<gemffile_t> files;
    QHash<quint32, QList<range_t>> rangesByZoom;

signals:

public slots:
};

#endif // CMAPGEMF_H
