#ifndef POLYGONDRAWING_H
#define POLYGONDRAWING_H


#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QTransform>
#include <QtCore>
#include <QGraphicsPolygonItem>
#include <QPainterPath>
#include <string>
#include <QGraphicsTextItem>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "cornergrabber.h"
class polygonDrawing : public QObject,  public QGraphicsPolygonItem
{
    Q_OBJECT
public:
    polygonDrawing(const QPolygonF &p, const QString &name, QPointF position);
    QGraphicsTextItem _text;
    void setGridSpace(int space);
    void setLength(int len);
    void updatePolygon(const QPolygonF &p);
    QPolygonF getPolygon();
    QString name;

private:
   // virtual QRectF boundingRect() const; ///< must be re-implemented in this class to provide the diminsions of the box to the QGraphicsView
    virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget); ///< must be re-implemented here to pain the box on the paint-event
    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event ); ///< must be re-implemented to handle mouse hover enter events
    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ); ///< must be re-implemented to handle mouse hover leave events

    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );///< allows the main object to be moved in the scene by capturing the mouse move events
    virtual void mousePressEvent (QGraphicsSceneMouseEvent * event );
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent * event );

    virtual void mouseMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mousePressEvent(QGraphicsSceneDragDropEvent *event);
    virtual bool sceneEventFilter ( QGraphicsItem * watched, QEvent * event ) ;

    void setCornerPositions();
    void createCustomPath(QPointF mouseLocation, CornerGrabber*);

signals:
    void sendModifiedPolygon(const QString &, const QPolygonF &);

private:
    QColor  _outterborderColor; ///< the hover event handlers will toggle this between red and black
    QPen    _pen; ///< the pen is used to paint the red/black border
    QPointF _location;
    QPointF _dragStart;
    int _XcornerGrabBuffer;
    int _YcornerGrabBuffer;
    bool _cornerGrabbed;
    std::vector<CornerGrabber*>  _corners;
    QPolygonF _selectRegion;
    QPolygonF points;

private:
    void calculateConvexHull(QPolygonF &);
};
#endif
