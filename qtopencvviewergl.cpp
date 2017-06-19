//==============================================================================
/*
    \author    Vinkle Srivastav
*/
//==============================================================================

#include "qtopencvviewergl.h"
#include <QOpenGLFunctions>

QtOpencvViewerGL::QtOpencvViewerGL(QWidget *parent) :
    QOpenGLWidget(parent)
{
    mSceneChanged = false;
    mBgColor = QColor::fromRgb(124, 12, 56);

    mOutH = 0;
    mOutW = 0;
    mImgRatio = 4.0f/3.0f;

    mPosX = 0;
    mPosY = 0;

//    mPausedVideo = false;
//    mDrawRectangle = false;
//    mDrawPolygon = false;
//    mMouseDrag  = false;
}
//void QtOpencvViewerGL::setDrawRect(bool)
//{
//    mDrawRectangle = true;
//}

//void QtOpencvViewerGL::setDrawPoly(bool)
//{
//    mDrawPolygon = true;
//}

void QtOpencvViewerGL::initializeGL()
{
    makeCurrent();
    initializeOpenGLFunctions();
    glClearColor(0.0,0.0,0.0,1.0f);
}

void QtOpencvViewerGL::resizeGL(int width, int height)
{
    makeCurrent();
    glViewport(0, 0, (GLint)width, (GLint)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, width, 0, height, 0, 1);	// To Draw image in the center of the area

    glMatrixMode(GL_MODELVIEW);

    // ---> Scaled Image Sizes
//    mOutH = width/mImgRatio;
    mOutH = height;
    mOutW = width;

//    if(mOutH>height)
//    {
//        mOutW = height*mImgRatio;
//        mOutH = height;
//    }

    emit imageSizeChanged( mOutW, mOutH );
    // <--- Scaled Image Sizes

    mPosX = (width-mOutW)/2;
    mPosY = (height-mOutH)/2;

    mSceneChanged = true;
    updateScene();
}

void QtOpencvViewerGL::updateScene()
{
    if( mSceneChanged && this->isVisible() )
        //updateGL(); obsolete
        update();
}

void QtOpencvViewerGL::paintGL()
{
    makeCurrent();

    //if( !mSceneChanged )
    //    return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderImage();

    mSceneChanged = false;

//    QPainter p(this);
//    if(mPausedVideo)
//    {
//        if(mDrawRectangle)
//        {
//            QPen pointPen(Qt::blue);
//            pointPen.setWidth(3);
//            p.setPen(pointPen);
//            QRect r1(mRectLeftTop, mRectDragPoint);
//            //qDebug() << "mRectLeftTop " << mRectLeftTop;
//            //qDebug() << "mRectRightBottom " << mRectDragPoint;
//            p.drawRect(r1);
//        }
//        else if(mDrawPolygon)
//        {
//            if(mPolyPoints.size() == 1)
//            {
//                QPen pointPen(Qt::blue);
//                pointPen.setWidth(3);
//                p.setPen(pointPen);
//                p.drawPoint(mPolyPoints[0]);
//            }
//            else
//            {
//                QPen pointPen(Qt::blue);
//                pointPen.setWidth(3);
//                p.setPen(pointPen);
//                p.drawPoint(mPolyPoints[0]);


//                for(int i = 0; i < mPolyPoints.size()-1; i++)
//                {
//                    QPen pointPen(Qt::blue);
//                    pointPen.setWidth(5);
//                    p.setPen(pointPen);
//                    p.drawPoint(mPolyPoints[i+1]);

//                    pointPen.setColor(Qt::red);
//                    pointPen.setWidth(2);
//                    p.setPen(pointPen);
//                    p.drawLine(mPolyPoints[i], mPolyPoints[i+1]);
//                }
//            }

//        }
//    }
    // Draw the rest of the objects
//    for(int k = 0; k < m_currentObjectDrawn.objects.size(); k++)
//    {
//        // Draw the polygon
//        if(m_currentObjectDrawn.objects[k].rectOrPoly)
//        {
//            std::vector<cv::Point> points = m_currentObjectDrawn.objects[k].pts;
//            QPen pointPen(Qt::blue);
//            pointPen.setWidth(3);
//            p.setPen(pointPen);
//            QPoint p1, p2;
//            p1.setX(points[0].x);
//            p1.setY(points[0].y);
//            p.drawPoint(p1);


//            for(uint i = 0; i < points.size()-1; i++)
//            {
//                p1.setX(points[i].x);
//                p1.setY(points[i].y);

//                p2.setX(points[i+1].x);
//                p2.setY(points[i+1].y);


//                QPen pointPen(Qt::blue);
//                pointPen.setWidth(5);
//                p.setPen(pointPen);
//                p.drawPoint(p2);

//                pointPen.setColor(Qt::red);
//                pointPen.setWidth(2);
//                p.setPen(pointPen);
//                p.drawLine(p1, p2);
//            }

//        }
//        else
//        {
//            // Draw the rectangle
//            QPen pointPen(Qt::blue);
//            pointPen.setWidth(3);
//            p.setPen(pointPen);
//            QRect r1(m_currentObjectDrawn.objects[k].rect.x,
//                     m_currentObjectDrawn.objects[k].rect.y,
//                     m_currentObjectDrawn.objects[k].rect.width,
//                     m_currentObjectDrawn.objects[k].rect.height);
//            p.drawRect(r1);
//        }
//    }
}
void QtOpencvViewerGL::renderImage()
{
    makeCurrent();

    glClear(GL_COLOR_BUFFER_BIT);

    if (!mRenderQtImg.isNull())
    {
        glLoadIdentity();

        QImage image; // the image rendered

        glPushMatrix();
        {
            int imW = mRenderQtImg.width();
            int imH = mRenderQtImg.height();

            // The image is to be resized to fit the widget?
            if( imW != this->size().width() &&
                    imH != this->size().height() )
            {

                image = mRenderQtImg.scaled( //this->size(),
                                             QSize(mOutW,mOutH),
                                             Qt::IgnoreAspectRatio,
                                             Qt::SmoothTransformation
                                             );

                //qDebug( QString( "Image size: (%1x%2)").arg(imW).arg(imH).toAscii() );
            }
            else
                image = mRenderQtImg;

            // ---> Centering image in draw area
            glRasterPos2i( mPosX, mPosY );
            // <--- Centering image in draw area

            imW = image.width();
            imH = image.height();
            //qDebug() << "imW " << imW << "imH " << imH;
            glDrawPixels( imW, imH, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
        }
        glPopMatrix();

        // end
        glFlush();
    }
}
//void QtOpencvViewerGL::setPause(bool p)
//{
//    mPausedVideo = p;
//}

//void QtOpencvViewerGL::drawCurrentFrameObjects(const ObjectsAnnotation &obj)
//{
//    m_currentObjectDrawn = obj;
//    update();
//}

bool QtOpencvViewerGL::showImage(const cv::Mat &image)
{
    //qDebug() << "mPausedVideo " << mPausedVideo;
    cv::flip(image, mOrigImage, 0);
    //image.copyTo(mOrigImage);

    mImgRatio = (float)image.cols/(float)image.rows;

    if( mOrigImage.channels() == 3)
        mRenderQtImg = QImage((const unsigned char*)(mOrigImage.data),
                              mOrigImage.cols, mOrigImage.rows,
                              mOrigImage.step, QImage::Format_RGB888)/*.rgbSwapped()*/;
    else if( mOrigImage.channels() == 1)
        mRenderQtImg = QImage((const unsigned char*)(mOrigImage.data),
                              mOrigImage.cols, mOrigImage.rows,
                              mOrigImage.step, QImage::Format_Indexed8);
    else
        return false;

    //mRenderQtImg = QGLWidget::convertToGLFormat(mRenderQtImg); obsolete

    mSceneChanged = true;

    updateScene();

    return true;
}

//void QtOpencvViewerGL::mousePressEvent(QMouseEvent* e)
//{
//    if(e->buttons() & Qt::LeftButton)
//    {
//        //qDebug()<<"left button clicked " ;
//        //v[0] = static_cast<int>(e->pos().x());
//        //v[1] = static_cast<int>(e->pos().y());
//        if(mDrawRectangle)
//        {
//            if(!mMouseDrag)
//            {
//                mMouseDrag = true;
//                mRectLeftTop = e->pos();
//            }
//        }

//        if(mDrawPolygon)
//        {
//            mPolyPoints.push_back(e->pos());
//            if(mPolyPoints.size() > 3)
//            {
//                if(((abs(mPolyPoints[mPolyPoints.size()-1].x() - mPolyPoints[0].x())) <=2) &&
//                   ((abs(mPolyPoints[mPolyPoints.size()-1].y() - mPolyPoints[0].y())) <=2))
//                {
//                    polygonDrawn(mPolyPoints);
//                    mDrawPolygon = false;
//                }
//                else
//                {
//                    update();
//                }
//            }
//            else
//            {
//                update();
//            }

//        }

//    }
//    if (e->buttons() & Qt::RightButton)
//    {
//        //qDebug()<<"right button clicked " ;
//        //v[0] = static_cast<int>(e->pos().x());
//        //v[1] = static_cast<int>(e->pos().y());

//    }
//}

//void QtOpencvViewerGL::mouseReleaseEvent(QMouseEvent * e)
//{
//    //vector<int> v(2);
//    //qDebug()<<"Mouse release event " ;
//    //v[0] = static_cast<int>(e->pos().x());
//    //v[1] = static_cast<int>(e->pos().y());
//    //qDebug()<< "x-> " << v[0] << "y-> " << v[1];
//    if(mDrawRectangle)
//    {
//        if(mMouseDrag)
//        {
//            mRectRightBottom = e->pos();
//            rectangleDrawn(QRect(mRectLeftTop, mRectRightBottom));
//            mMouseDrag = false;
//            mDrawRectangle = false;
//        }
//    }
//}


//void QtOpencvViewerGL::mouseMoveEvent(QMouseEvent* e)
//{
//    //vector<int> v(2);
//    //v[0] = static_cast<int>(e->pos().x());
//    //v[1] = static_cast<int>(e->pos().y());
//    //qDebug()<< "pos-> " << e->pos();
//    if(mDrawRectangle)
//    {
//        if(mMouseDrag)
//        {
//            mRectDragPoint = e->pos();
//            update();
//        }
//    }
//}




