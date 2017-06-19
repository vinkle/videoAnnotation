#ifndef AUXCAMGRAPHICSVIEW_H
#define AUXCAMGRAPHICSVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QObject>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <string>
#include <QPainter>
//12-12-12
using namespace std;

class mainGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit mainGraphicsView(QWidget *parent = 0);
    //void setScene(QGraphicsScene * scene);
    //void show();
    ~mainGraphicsView();

protected:
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);



signals:
    void sendMouseClickEvent(const string &clk, int mouseX, int mouseY);
    void sendMouseMoveData(int mouseX, int mouseY);
    void sendMouseReleaseEvent(int mouseX, int mouseY);
protected:
     //virtual void drawBackground(QPainter *painter, const QRectF &rect);
};

#endif // AUXCAMGRAPHICSVIEW_H
