#include <QBrush>
#include <QLinearGradient>
#include <QDebug>
#include <QRect>
#include <QPolygon>
#include <QGraphicsPathItem>
#include "math.h"
#include "polygonDrawing.h"

polygonDrawing::polygonDrawing(const QPolygonF &p, const QString &n, QPointF position): QGraphicsPolygonItem(),
    _outterborderColor(Qt::yellow),
    _pen(),
    _location(0,0),
    _dragStart(0,0),
    _XcornerGrabBuffer(40),
    _YcornerGrabBuffer(40),
    _cornerGrabbed(false),
    _selectRegion()
{
    this->setPos(position);
    _pen.setWidth(2);
    _pen.setStyle(Qt::SolidLine);
    _pen.setColor(QColor(200, qrand()%255, qrand()%255));
    this->setAcceptHoverEvents(true);

    this->points = p;
    this->name = n;
    for(int i = 0; i < points.size(); i++)
    {
        QPointF p1  (points[i].x() - _XcornerGrabBuffer, points[i].y() - _YcornerGrabBuffer);
        QPointF p2  (points[i].x() + _XcornerGrabBuffer, points[i].y() - _YcornerGrabBuffer);
        QPointF p3  (points[i].x() + _XcornerGrabBuffer, points[i].y() + _YcornerGrabBuffer);
        QPointF p4  (points[i].x() - _XcornerGrabBuffer, points[i].y() + _YcornerGrabBuffer);
        _selectRegion.push_back(p1);
        _selectRegion.push_back(p2);
        _selectRegion.push_back(p3);
        _selectRegion.push_back(p4);
    }
    calculateConvexHull(_selectRegion);
    this->setPolygon(_selectRegion);

    _text.setPlainText(name);
    //_text.setScale(1.0);
    _text.setParentItem(this);
    _text.setDefaultTextColor(QColor(qrand()%255, qrand()%255, qrand()%255));
}

void polygonDrawing::updatePolygon(const QPolygonF &p)
{
    this->points = p;
    for(int i = 0; i < points.size(); i++)
    {
        QPointF p1  (points[i].x() - _XcornerGrabBuffer, points[i].y() - _YcornerGrabBuffer);
        QPointF p2  (points[i].x() + _XcornerGrabBuffer, points[i].y() - _YcornerGrabBuffer);
        QPointF p3  (points[i].x() + _XcornerGrabBuffer, points[i].y() + _YcornerGrabBuffer);
        QPointF p4  (points[i].x() - _XcornerGrabBuffer, points[i].y() + _YcornerGrabBuffer);
        _selectRegion.push_back(p1);
        _selectRegion.push_back(p2);
        _selectRegion.push_back(p3);
        _selectRegion.push_back(p4);
    }
    calculateConvexHull(_selectRegion);
    this->setPolygon(_selectRegion);
}

QPolygonF polygonDrawing::getPolygon()
{
    return mapToScene(points);
}

void polygonDrawing::calculateConvexHull(QPolygonF &points)
{
    std::vector<cv::Point2f> hull;
    std::vector<cv::Point2f> ptCV(points.size());
    for(int i = 0; i < points.size(); i++)
    {
        ptCV[i] = cv::Point2f((float)points[i].x(), (float)points[i].y());
    }
    cv::convexHull( cv::Mat(ptCV), hull, false );
    points.clear();
    points.resize(hull.size());
    for(int i = 0; i < points.size(); i++)
    {
        points[i] = QPointF(hull[i].x, hull[i].y);
    }
}

bool polygonDrawing::sceneEventFilter ( QGraphicsItem * watched, QEvent * event )
{
    //qDebug() << " QEvent == " + QString::number(event->type());
    this->prepareGeometryChange();
    CornerGrabber * corner = dynamic_cast<CornerGrabber *>(watched);
    if ( corner == NULL) return false; // not expected to get here

    QGraphicsSceneMouseEvent * mevent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    if ( mevent == NULL)
    {
        // this is not one of the mouse events we are interrested in
        return false;
    }


    switch (event->type() )
    {
    // if the mouse went down, record the x,y coords of the press, record it inside the corner object
    case QEvent::GraphicsSceneMousePress:
    {
        corner->setMouseState(CornerGrabber::kMouseDown);

        QPointF scenePosition =  corner->mapToScene(mevent->pos());
        corner->mouseDownY = scenePosition.y();
        corner->mouseDownX= scenePosition.x();
        //qDebug() << "grabbed points " << points;
        _cornerGrabbed = true;

    }
        break;

    case QEvent::GraphicsSceneMouseRelease:
    {
        corner->setMouseState(CornerGrabber::kMouseReleased);
        // here send the updated points
        // qDebug() << "release points " << mapToScene(points);
        QPolygonF scenePoints = mapToScene(points);
        emit sendModifiedPolygon(name, scenePoints); // send the points corresponding to point editing event
        _cornerGrabbed = false;
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
        createCustomPath(mevent->pos(), corner);
        this->update();
    }
    return true;// true => do not send event to watched - we are finished with this event
}



void polygonDrawing::createCustomPath(QPointF mouseLocation, CornerGrabber* corner)
{
    qreal mouseX =0, mouseY = 0;
    QPointF scenePosition = corner->mapToScene(mouseLocation);
    mouseY = scenePosition.y();
    mouseX = scenePosition.x();

    _selectRegion.clear();
    for(int i = 0; i < points.size(); i++)
    {
        if(corner == _corners[i])
        {
            points[i] = QPointF(mapFromScene(mouseX, mouseY));
            //points[i] = mouseLocation;
        }
        QPointF p1  (points[i].x() - _XcornerGrabBuffer, points[i].y() - _YcornerGrabBuffer);
        QPointF p2  (points[i].x() + _XcornerGrabBuffer, points[i].y() - _YcornerGrabBuffer);
        QPointF p3  (points[i].x() + _XcornerGrabBuffer, points[i].y() + _YcornerGrabBuffer);
        QPointF p4  (points[i].x() - _XcornerGrabBuffer, points[i].y() + _YcornerGrabBuffer);
        _selectRegion.push_back(p1);
        _selectRegion.push_back(p2);
        _selectRegion.push_back(p3);
        _selectRegion.push_back(p4);
    }
    calculateConvexHull(_selectRegion);
    this->setPolygon(_selectRegion);
    setCornerPositions();
    this->update();
}


// for supporting moving the box across the scene
void polygonDrawing::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    this->prepareGeometryChange();
    event->setAccepted(true);
    _location.setX( static_cast<int>(_location.x())) ;
    _location.setY( static_cast<int>(_location.y()));
    this->setPos(_location);
    QPolygonF scenePoints = mapToScene(points);
    emit sendModifiedPolygon(name, scenePoints);// send the points corresponding to dragging event
    //qDebug() << "Release points " << mapToScene(points);
}


// for supporting moving the box across the scene
void polygonDrawing::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
    _dragStart = event->pos();
}

// for supporting moving the box across the scene
void polygonDrawing::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    this->prepareGeometryChange();
    QPointF newPos = event->pos() ;
    _location += (newPos - _dragStart);
    this->setPos(_location);
}

// remove the corner grabbers
void polygonDrawing::hoverLeaveEvent ( QGraphicsSceneHoverEvent * )
{
    _outterborderColor = Qt::yellow;
    for(uint i = 0; i < _corners.size(); i++)
    {
        _corners[i]->setParentItem(NULL);
    }
    for(uint i = 0; i < _corners.size(); i++)
    {
        delete _corners[i];
    }
}


// create the corner grabbers
void polygonDrawing::hoverEnterEvent ( QGraphicsSceneHoverEvent * )
{
    _outterborderColor = Qt::cyan;
    _corners.clear();
    _corners.resize(points.size());
    for(int i = 0; i < points.size(); i++)
    {
        _corners[i] = new CornerGrabber(this,0);
    }
    for(int i = 0; i < points.size(); i++)
    {
        _corners[i]->installSceneEventFilter(this);
    }
    setCornerPositions();
}

void polygonDrawing::setCornerPositions()
{
    int cornerWidth  = (_corners[0]->boundingRect().width())/2;
    int cornerHeight = (_corners[0]->boundingRect().height())/2;
    for(int i = 0; i < points.size(); i++)
    {
        _corners[i]->setPos(points[i].x() - cornerWidth, points[i].y() - cornerHeight);
    }
    //qDebug() << "corner[0].pos : " + QString::number(_corners[0]->pos().x()) + ", " + QString::number(_corners[0]->pos().y())
    //<< "; corner[1].pos : " + QString::number(_corners[1]->pos().x()) + ", " + QString::number(_corners[1]->pos().y());
}

void polygonDrawing::paint (QPainter *painter, const QStyleOptionGraphicsItem *i, QWidget *w)
{
    _text.setPos(points[0]-QPointF(2,2));
    painter->setPen(_pen);
    painter->drawPolygon(points);
    //_pen.setColor(Qt::blue);
    //painter->setPen(_pen);
    //painter->drawPolyline(_selectRegion);
}

void polygonDrawing::mouseMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

void polygonDrawing::mousePressEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}
