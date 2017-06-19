#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QList>
#include <QGraphicsItem>
#include <QMessageBox>
#include <QGraphicsRectItem>
#include "polygonDrawing.h"
#include "rectangleDrawing.h"

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    enum {kDrawfromPolgon=0, kDrawfromRectangle};

    explicit GraphicsScene(QObject *parent = 0);
    ~GraphicsScene();
    void setDrawingParams(int m, const QString &name);
    //int getDrawingMethod();
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);

public:
    // container for the polygon and rectangular items
    QMap<QString, polygonDrawing *> m_items_polygon;
    QMap<QString, rectangleDrawing *> m_items_rectangle;
signals:
    void polygonDrawn(const QString &, const QPolygonF &);
    void rectangleDrawn(const QString &, const QRectF &);
private:
    // message box for accepting the annotation
    QMessageBox msgBoxAcceptAnnotation;


    QPolygonF m_points;


    polygonDrawing *poly;
    rectangleDrawing *rect;

    // temporary drawing objects
    QList<QGraphicsEllipseItem*> groupEllipsesPolygon;
    QList<QGraphicsLineItem*> groupLinesPolygon;
    QGraphicsRectItem* m_tempRect;

    // Drawing method
    int m_drawingMethod;
    QString m_currentObjectName;

    bool m_MouseDrag;
    QPointF m_RectLeftTop, m_RectRightBottom;
    QPen m_RectPen;

};

#endif // GRAPHICSSCENE_H
