#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <QDomNode>

template<typename T>
void readXml(const QDomNode& xml, const QString& tag, T& value);

void readXml(const QDomNode& xml, const QString& tag, QString& value);

void readXml(const QDomNode& xml, const QString& tag, QString& value, bool& isHtml);
#endif // SERIALIZATION_H
