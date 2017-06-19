#include "mainGraphicsView.h"

mainGraphicsView::mainGraphicsView(QWidget *parent): QGraphicsView(parent)
{
}
mainGraphicsView::~mainGraphicsView()
{
}

void mainGraphicsView::mousePressEvent(QMouseEvent* e)
{
    string click;
    if(e->buttons() & Qt::LeftButton)
    {
        click = "left";
        emit sendMouseClickEvent(click, e->pos().x(), e->pos().y());
    }
    if (e->buttons() & Qt::RightButton)
    {
        click = "right";
        emit sendMouseClickEvent(click, e->pos().x(), e->pos().y());
    }
    QGraphicsView::mousePressEvent(e);
}

void mainGraphicsView::mouseMoveEvent(QMouseEvent* e)
{
    emit sendMouseMoveData(e->pos().x(), e->pos().y());
    QGraphicsView::mouseMoveEvent(e);
}

void mainGraphicsView::mouseReleaseEvent(QMouseEvent * e)
{
    emit sendMouseReleaseEvent(e->pos().x(), e->pos().y());
    QGraphicsView::mouseReleaseEvent(e);
}







