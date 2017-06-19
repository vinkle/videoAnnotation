#include "graphicsscene.h"
#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include "qmath.h"

GraphicsScene::GraphicsScene(QObject *parent) :
    QGraphicsScene(parent)
{
    //this->setBackgroundBrush(Qt::gray);
    msgBoxAcceptAnnotation.setWindowTitle("Accept");
    msgBoxAcceptAnnotation.setText("Accept annotation to the database ?");
    msgBoxAcceptAnnotation.setStandardButtons(QMessageBox::Yes);
    msgBoxAcceptAnnotation.addButton(QMessageBox::No);
    msgBoxAcceptAnnotation.setDefaultButton(QMessageBox::Yes);
    m_drawingMethod = -1;
    m_currentObjectName = "";
    m_tempRect = new QGraphicsRectItem();

    m_RectPen.setWidth(2);
    m_RectPen.setColor(Qt::green);

    m_tempRect = this->addRect(QRect(0,0,0,0), m_RectPen);
    m_MouseDrag = false;
}

void GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    //qDebug() << Q_FUNC_INFO << mouseEvent->scenePos();
    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void GraphicsScene::setDrawingParams(int m, const QString &name)
{
    m_drawingMethod = m;
    m_currentObjectName = name;
}

//int GraphicsScene::getDrawingMethod()
//{
//    return m_drawingMethod;
//}


void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    if(m_MouseDrag)
    {
        m_RectRightBottom = mouseEvent->scenePos();
        m_tempRect->setRect(QRectF(m_RectLeftTop, m_RectRightBottom));
    }
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}
void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    //qDebug() << mouseEvent->scenePos();
    if(mouseEvent->button() == Qt::LeftButton)
    {
        if(m_drawingMethod == GraphicsScene::kDrawfromPolgon)
        {
            int radius = 4;
            QGraphicsEllipseItem* ellipse = this->addEllipse(mouseEvent->scenePos().x()-radius, mouseEvent->scenePos().y()-radius, radius*2, radius*2);
            groupEllipsesPolygon.append(ellipse);
            ellipse->setBrush(Qt::white);
            m_points.append(mouseEvent->scenePos());
            if(m_points.size() > 1)
            {
                uint sz = m_points.size();
                QGraphicsLineItem *line = this->addLine(m_points[sz-1].x(), m_points[sz-1].y(), m_points[sz-2].x(), m_points[sz-2].y());
                groupLinesPolygon.append(line);
                line->setPen(QPen(Qt::red));
                if(m_points.size() > 3)
                {
                    if(((abs(m_points[m_points.size()-1].x() - m_points[0].x())) <=5) &&
                            ((abs(m_points[m_points.size()-1].y() - m_points[0].y())) <=5))
                    {
                        if(msgBoxAcceptAnnotation.exec() == QMessageBox::Yes)
                        {
                            // clear the drawing stuff
                            for(int i = 0; i < groupEllipsesPolygon.size(); i++)
                            {
                                delete groupEllipsesPolygon[i];
                            }
                            groupEllipsesPolygon.clear();
                            for(int i = 0; i < groupLinesPolygon.size(); i++)
                            {
                                delete groupLinesPolygon[i];
                            }
                            groupLinesPolygon.clear();

                            // create a new polygon object
                            m_points.removeLast();
                            poly = new polygonDrawing(m_points, m_currentObjectName, QPointF(0,0));

                            // connect the moving and editing signals to this class signal;
                            connect(poly, SIGNAL(sendModifiedPolygon(const QString &, const QPolygonF &)), this, SIGNAL(polygonDrawn(const QString &, const QPolygonF &)));
                            m_items_polygon[m_currentObjectName] = poly;

                            // add it to the scene
                            this->addItem(poly);

                            // send a signal that object is been added
                            emit polygonDrawn(m_currentObjectName, m_points);

                            // clear the name
                            m_points.clear();
                            m_drawingMethod = -1;
                        }
                        else
                        {
                            for(int i = 0; i < groupEllipsesPolygon.size(); i++)
                            {
                                delete groupEllipsesPolygon[i];
                            }
                            groupEllipsesPolygon.clear();
                            for(int i = 0; i < groupLinesPolygon.size(); i++)
                            {
                                delete groupLinesPolygon[i];
                            }
                            groupLinesPolygon.clear();
                            m_drawingMethod = GraphicsScene::kDrawfromPolgon;
                            m_points.clear();
                            emit polygonDrawn("", m_points);
                            m_drawingMethod = -1;
                        }
                    }
                }
            }
        }
        else if(m_drawingMethod == GraphicsScene::kDrawfromRectangle)
        {
            if(!m_MouseDrag)
            {
                m_MouseDrag = true;
                m_RectLeftTop = mouseEvent->scenePos();
            }
        }
    }
    else if(mouseEvent->button() == Qt::RightButton)
    {
    }
    //qDebug() << "button clicked\n";
    QGraphicsScene::mousePressEvent(mouseEvent);
}

GraphicsScene::~GraphicsScene()
{
    delete m_tempRect;
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    //qDebug() << me->scenePos();
    if(m_MouseDrag)
    {
        if(msgBoxAcceptAnnotation.exec() == QMessageBox::Yes)
        {
            m_RectRightBottom = mouseEvent->scenePos();
            m_tempRect->setRect(QRectF(m_RectLeftTop, m_RectRightBottom));


            rect = new rectangleDrawing(QRectF(m_RectLeftTop, m_RectRightBottom), m_currentObjectName);

            // connect the moving and editing signals to this class signal;
            connect(rect, SIGNAL(sendModifiedRectangle(const QString &, const QRectF &)), this, SIGNAL(rectangleDrawn(const QString &, const QRectF &)));
            m_items_rectangle[m_currentObjectName] = rect;

            // add it to the scene
            this->addItem(rect);

            // send a signal that object is been added
            emit rectangleDrawn(m_currentObjectName, QRectF(m_RectLeftTop, m_RectRightBottom));
            m_tempRect->setRect(QRectF(0,0,0,0));
            m_drawingMethod = -1;
            m_MouseDrag = false;
        }
        else
        {
            // send a signal that object is been added
            emit rectangleDrawn(m_currentObjectName, QRectF(-1.0f, -1.0f, -1.0f, -1.0f));
            m_tempRect->setRect(QRectF(0,0,0,0));
            m_drawingMethod = -1;
            m_MouseDrag = false;

        }
    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
