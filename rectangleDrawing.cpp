#include "rectangleDrawing.h"

#include <QBrush>
#include <QLinearGradient>
#include <QDebug>

#include "math.h"

/**
  *  This box can be re-sized and it can be moved. For moving, we capture
  *  the mouse move events and move the box location accordingly.
  *
  *  To resize the box, we place small indicator boxes on the four corners that give the user
  *  a visual cue to grab with the mouse. The user then drags the corner to stretch
  *  or shrink the box.  The corner grabbers are implemented with the CornerGrabber class.
  *  The CornerGrabber class captures the mouse when the mouse is over the corner's area,
  *  but the StateBox object (which owns the corners) captures and processes the mouse
  *  events on behalf of the CornerGrabbers (because the owner wants to be
  *  resized, not the CornerGrabbers themselves). This is accomplished by installed a scene event filter
  *  on the CornerGrabber objects:
          _corners[0]->installSceneEventFilter(this);
  *
  *
  *
  */

rectangleDrawing::rectangleDrawing(const QRectF &p, const QString &name):
        _text(),
        _outterborderColor(Qt::black),
        _outterborderPen(),
        _location(0,0),
        _dragStart(0,0)
{
    _outterborderPen.setWidth(2);
    _outterborderPen.setColor(QColor(qrand()%255, qrand()%255, qrand()%255));
    _text.setPlainText(name);
    _text.setParentItem(this);
    _text.setDefaultTextColor(QColor(qrand()%255, qrand()%255, qrand()%255));
    this->name = name;
    this->setAcceptHoverEvents(true);
    this->setRect(p);
    this->setPos(0,0);

}

void rectangleDrawing::updateRectangle(const QRectF &p)
{
    this->setRect(p);
}

QRectF rectangleDrawing::getRectangle()
{
    QPointF topLeft =  mapToScene(QPointF(this->rect().x(), this->rect().y()));
    return QRectF(topLeft, QSize(this->rect().width(), this->rect().height()));
}


bool rectangleDrawing::sceneEventFilter ( QGraphicsItem * watched, QEvent * event )
{
    //qDebug() << " QEvent == " + QString::number(event->type());

    CornerGrabber * corner = dynamic_cast<CornerGrabber *>(watched);
    if ( corner == NULL) return false; // not expected to get here

    QGraphicsSceneMouseEvent * mevent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    if ( mevent == NULL)
    {
        // this is not  one of the mouse events we are interrested in
        return false;
    }


    switch (event->type() )
    {
            // if the mouse went down, record the x,y coords of the press, record it inside the corner object
        case QEvent::GraphicsSceneMousePress:
            {
                corner->setMouseState(CornerGrabber::kMouseDown);
                corner->mouseDownX = mevent->pos().x();
                corner->mouseDownY = mevent->pos().y();
            }
            break;

        case QEvent::GraphicsSceneMouseRelease:
            {
                corner->setMouseState(CornerGrabber::kMouseReleased);

            }
            break;

        case QEvent::GraphicsSceneMouseMove:
            {
                corner->setMouseState(CornerGrabber::kMouseMoving );
            }
            break;

        default:
            // we dont care about the rest of the events
            return false;
            break;
    }


    if ( corner->getMouseState() == CornerGrabber::kMouseMoving )
    {

        qreal x = mevent->pos().x(), y = mevent->pos().y();

        // depending on which corner has been grabbed, we want to move the position
        // of the item as it grows/shrinks accordingly. so we need to eitehr add
        // or subtract the offsets based on which corner this is.

        int XaxisSign = 0;
        int YaxisSign = 0;
        switch( corner->getCorner() )
        {
        case 0:
            {
                XaxisSign = +1;
                YaxisSign = +1;
            }
            break;

        case 1:
            {
                XaxisSign = -1;
                YaxisSign = +1;
            }
            break;

        case 2:
            {
                XaxisSign = -1;
                YaxisSign = -1;
            }
            break;

        case 3:
            {
                XaxisSign = +1;
                YaxisSign = -1;
            }
            break;

        }

        // if the mouse is being dragged, calculate a new size and also re-position
        // the box to give the appearance of dragging the corner out/in to resize the box

        int xMoved = corner->mouseDownX - x;
        int yMoved = corner->mouseDownY - y;

        int newWidth = this->rect().width() + ( XaxisSign * xMoved);
        if ( newWidth < 40 ) newWidth  = 40;

        int newHeight = this->rect().height() + (YaxisSign * yMoved) ;
        if ( newHeight < 40 ) newHeight = 40;

        int deltaWidth  =   newWidth - this->rect().width() ;
        int deltaHeight =   newHeight - this->rect().height() ;

        this->setRect(this->rect().x(), this->rect().y(), this->rect().width()+deltaWidth, this->rect().height()+deltaHeight);


        deltaWidth *= (-1);
        deltaHeight *= (-1);

        if ( corner->getCorner() == 0 )
        {
            //int newXpos = this->pos().x() + deltaWidth;
            //int newYpos = this->pos().y() + deltaHeight;

            this->setRect(this->rect().x() + deltaWidth, this->rect().y() + deltaHeight, this->rect().width(), this->rect().height());


            //this->setPos(newXpos, newYpos);
        }
        else   if ( corner->getCorner() == 1 )
        {
            //int newYpos = this->pos().y() + deltaHeight;
            //this->setPos(this->pos().x(), newYpos);
            this->setRect(this->rect().x(), this->rect().y() + deltaHeight, this->rect().width(), this->rect().height());

        }
        else   if ( corner->getCorner() == 3 )
        {
            //int newXpos = this->pos().x() + deltaWidth;
            //this->setPos(newXpos,this->pos().y());
            this->setRect(this->rect().x() + deltaWidth, this->rect().y(), this->rect().width(), this->rect().height());
        }
        QPointF topLeft =  mapToScene(QPointF(this->rect().x(), this->rect().y()));
        //qDebug() << "Rect out " << QRectF(topLeft, QSize(this->rect().width(), this->rect().height()));
        emit sendModifiedRectangle(name, QRectF(topLeft, QSize(this->rect().width(), this->rect().height())));

        setCornerPositions();
        this->update();
    }

    return true;// true => do not send event to watched - we are finished with this event
}



// for supporting moving the box across the scene
void rectangleDrawing::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    this->prepareGeometryChange();
    event->setAccepted(true);
    _location.setX( ( static_cast<int>(_location.x())));
    _location.setY( ( static_cast<int>(_location.y())));
    this->setPos(_location);

    QPointF topLeft =  mapToScene(QPointF(this->rect().x(), this->rect().y()));
    //qDebug() << "Rect out " << QRectF(topLeft, QSize(this->rect().width(), this->rect().height()));
    emit sendModifiedRectangle(name, QRectF(topLeft, QSize(this->rect().width(), this->rect().height())));
}

void rectangleDrawing::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    this->prepareGeometryChange();
    event->setAccepted(true);
    _dragStart = event->pos();
}
void rectangleDrawing::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    this->prepareGeometryChange();
    QPointF newPos = event->pos() ;
    _location += (newPos - _dragStart);
    //qDebug() << "_location " << _location;
    this->setPos(_location);
}

// remove the corner grabbers

void rectangleDrawing::hoverLeaveEvent ( QGraphicsSceneHoverEvent * )
{
    _outterborderColor = Qt::black;

    _corners[0]->setParentItem(NULL);
    _corners[1]->setParentItem(NULL);
    _corners[2]->setParentItem(NULL);
    _corners[3]->setParentItem(NULL);

    delete _corners[0];
    delete _corners[1];
    delete _corners[2];
    delete _corners[3];
}

void rectangleDrawing::hoverEnterEvent ( QGraphicsSceneHoverEvent * )
{
    _outterborderColor = Qt::red;
    _corners[0] = new CornerGrabber(this,0);
    _corners[1] = new CornerGrabber(this,1);
    _corners[2] = new CornerGrabber(this,2);
    _corners[3] = new CornerGrabber(this,3);
    _corners[0]->installSceneEventFilter(this);
    _corners[1]->installSceneEventFilter(this);
    _corners[2]->installSceneEventFilter(this);
    _corners[3]->installSceneEventFilter(this);
    setCornerPositions();
}

void rectangleDrawing::setCornerPositions()
{
    _corners[0]->setPos(this->rect().x(), this->rect().y());
    _corners[1]->setPos(this->rect().width() + this->rect().x()-5, this->rect().y());
    _corners[2]->setPos(this->rect().width() + this->rect().x()-5, this->rect().height() + this->rect().y()-5);
    _corners[3]->setPos(this->rect().x(), this->rect().height() + this->rect().y()-5);
}

QRectF rectangleDrawing::boundingRect() const
{
    return this->rect();
}


// example of a drop shadow effect on a box, using QLinearGradient and two boxes

void rectangleDrawing::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // for the desired effect, no border will be drawn, and because a brush was set, the drawRoundRect will fill the box with the gradient brush.
    _text.setPos(this->rect().x(), this->rect().y() + (this->rect().height() / 2));
    painter->setPen(_outterborderPen);
    painter->drawRect(this->rect()); // corner radius of 25 pixels
}


void rectangleDrawing::mouseMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

void rectangleDrawing::mousePressEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}
