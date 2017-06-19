#ifndef objectsAnnotation_Included
#define objectsAnnotation_Included

#include <QtCore>
#include <QPolygonF>
#include <QMap>
#include <QString>
#include <QWidget>
#include <iostream>
class objectsAnnotations
{
public:
    void print();
    objectsAnnotations(const QSize &);
    ~objectsAnnotations();
    QSize frameSize;
    QMap<QString, QVector<QPolygonF> > polyObjects;
    QMap<QString, QVector<QRectF> >rectObjects;
};


#endif
