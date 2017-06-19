//==============================================================================
/*
    \author    Vinkle Srivastav
*/
//==============================================================================

#ifndef INTERFACE_H
#define INTERFACE_H

#include <QWidget>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QtCore>
#include <QShortcut>
#include <QString>
#include <QFileDialog>
#include <QtGui>
#include <QStringList>
#include <QGraphicsItem>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QStackedLayout>
#include "graphicsscene.h"
#include "objectsAnnotation.h"
#include "objectdescription.h"
#include "polygonDrawing.h"
#include "tracker/MedianFlowTracker.h"

using namespace cv;
using namespace std;

namespace Ui {
class interface;
}

class interface : public QWidget
{
    Q_OBJECT

public:
    explicit interface(QWidget *parent = 0);
    ~interface();

    enum {kDrawfromPolgon=0, kDrawfromRectangle};

    void updatePolygonAndRectangles();

protected:
    void keyPressEvent(QKeyEvent *e);

private slots:
    void on_pbQuit_clicked();

    void on_pbLoad_clicked();

    void Read();

    void SliderFramePress();

    void SliderFrameRelease();

    void SliderFrameMove(int);

    void on_pbPlay_clicked();

    void leftKeyPressed();

    void rightKeyPressed();

    void on_sliderFrame_valueChanged(int value);

    void on_pbNewObject_clicked();

    void polygonDrawn(const QString &, const QPolygonF &);

    void rectangleDrawn(const QString &, const QRectF &);

    void on_pushButton_clicked();

    void on_btnTrackObjects_clicked();

private:
    vector<Point2f> qtPolygon2cvContour(const QPolygonF &);
    QPolygonF cvContour2qtPolygon(const vector<Point2f> &);

private:
    Ui::interface *ui;
    // capture to handle video file
    cv::VideoCapture m_videoHandle;

    // current and previous frame
    cv::Mat m_currentFrame, m_previousFrame, m_currentFrameGray, m_previousFrameGray;

    // video parameters
    int m_fpsVideo, m_numFrames, m_frameW, m_frameH;

    // Viewer Height and width
    int m_viewerHeight, m_viewerWidth;

    // Timer to extract frames
    QTimer *m_timerVideo;
    //QTimer *m_timerDrawing;

    // if pause is pressed
    bool m_pausePressed;

    // counter on the frames
    int m_readCount;

    // shortcuts on the left and right arrow
    QShortcut *m_leftArrow, *m_rightArrow;

    // Drawing method
    int m_drawingMethod;

    // Graphic scene
    GraphicsScene *m_scene;

    // datastructure to handle the annotation
    objectsAnnotations* m_objectsAnnotation;


    // name of the objects on the screen; suppose to be read from the file
    int m_toolCount;
    int m_pegCount;
    int m_targetPegCount;
    int m_ringCount;
    QString m_currentObjectName;

    QStringList m_rectangularObjects;
    QStringList m_polygonalObjects;

    // tracker parameters
    MedianFlowTracker *medianFlowTracker;


private:
    void filldummy();
};

#endif // INTERFACE_H
