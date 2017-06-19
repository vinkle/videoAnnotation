/*
  * Copyright 2010, David W. Drell
  *
  * This is free sample code. Use as you wish, no warranties.
  */

#ifndef rectangleDrawing_H
#define rectangleDrawing_H

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QtCore>
#include <QPointF>
#include "cornergrabber.h"


class rectangleDrawing : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    rectangleDrawing(const QRectF &p, const QString &name);
    QGraphicsTextItem _text;    ///< sample text to go in the title area.
    QString name; // name of the object

    void setGridSpace(int space);
    void updateRectangle(const QRectF &p);
    QRectF getRectangle();

signals:
    void sendModifiedRectangle(const QString &, const QRectF &);

private:
    virtual QRectF boundingRect() const; ///< must be re-implemented in this class to provide the diminsions of the box to the QGraphicsView
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


    QColor _outterborderColor; ///< the hover event handlers will toggle this between red and black
    QPen _outterborderPen; ///< the pen is used to paint the red/black border
    QPointF _location;
    QPointF _dragStart;


    CornerGrabber*  _corners[4];// 0,1,2,3  - starting at x=0,y=0 and moving clockwise around the box


};

#endif // rectangleDrawing_H
