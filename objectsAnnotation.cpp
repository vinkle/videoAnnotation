#include "objectsAnnotation.h"

objectsAnnotations::objectsAnnotations(const QSize &p)
{
    this->frameSize = p;
}
objectsAnnotations::~objectsAnnotations()
{
}

void objectsAnnotations::print()
{
    qDebug() << "Frame Size -> (" << frameSize.width() << "," << frameSize.height() << ")\n";
    qDebug() << " Rectangular objects";

    for(auto e : rectObjects.keys())
    {
        qDebug() << "Name - " << e;
        QVector<QRectF> ff = rectObjects.value(e);
        for(uint i = 0; i < ff.size(); i++)
        {
            if(!ff[i].isEmpty())
            {
                qDebug() << "Frame - " << i+1 << " Data: " << ff[i];
            }
        }
    }
    qDebug() << "\nPolygon objects";
    for(auto e : polyObjects.keys())
    {
        qDebug() << "Name - " << e;
        QVector<QPolygonF> ff = polyObjects.value(e);
        qDebug() << "Size of the vector - " << ff.size();
        for(uint i = 0; i < ff.size(); i++)
        {
            if(!ff[i].isEmpty())
            {
                qDebug() << "Frame - " << i+1 << " Data: " << ff[i];
            }
        }
    }
}
