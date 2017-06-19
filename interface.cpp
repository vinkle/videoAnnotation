//==============================================================================
/*
    \author    Vinkle Srivastav
*/
//==============================================================================

#include "interface.h"
#include "ui_interface.h"

interface::interface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::interface)
{
    ui->setupUi(this);

    // member variable initialization
    m_pausePressed = false;
    m_readCount = 0;
    m_viewerHeight =  0;
    m_viewerWidth  =  0;

    // member object initialization
    m_timerVideo = new QTimer(this);
    m_scene = new GraphicsScene(this);
    m_leftArrow = new QShortcut(Qt::Key_Left, this, SLOT(leftKeyPressed()));
    m_rightArrow = new QShortcut(Qt::Key_Right, this, SLOT(rightKeyPressed()));

    // connect methods
    connect(m_timerVideo,SIGNAL(timeout()),this,SLOT(Read()));
    connect(ui->sliderFrame,SIGNAL(sliderPressed()),this,SLOT(SliderFramePress()));
    connect(ui->sliderFrame,SIGNAL(sliderReleased()),this,SLOT(SliderFrameRelease()));
    connect(ui->sliderFrame,SIGNAL(sliderMoved(int)),this,SLOT(SliderFrameMove(int)));

    connect(m_scene, SIGNAL(polygonDrawn(const QString &, const QPolygonF &)), this, SLOT(polygonDrawn(const QString &, const QPolygonF &)));
    connect(m_scene, SIGNAL(rectangleDrawn(const QString &, const QRectF &)), this, SLOT(rectangleDrawn(const QString &, const QRectF &)));

    // Drawing methods
    ui->drawing->setStyleSheet("background: transparent");
    ui->drawing->setScene(m_scene);
    ui->drawing->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->drawing->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Default image to be shown on the viewer
    cv::Mat im = cv::imread("/home/terminalx/Projects/video-ann/surgicalAnnotation/surgicalAnnotation/img/screenshow.png");
    ui->viewer->showImage(im);

    // Deafult ui parameters
    ui->radioDrawPolygon->setChecked(true);
    ui->radioObjectRing->setChecked(true);


    // No of objects count initialization
    m_toolCount = 0;
    m_pegCount = 0;
    m_targetPegCount = 0;
    m_ringCount = 0;

    // initialize the tracker

    medianFlowTracker = new MedianFlowTracker();

}



interface::~interface()
{
    delete ui;
    delete m_timerVideo ;
    delete m_scene;
    delete m_leftArrow ;
    delete m_rightArrow;
    delete medianFlowTracker;
}

void interface::updatePolygonAndRectangles()
{
    // get all the
    //qDebug() << "Current Frame " << m_readCount;
    qDebug() << "m_readCount - " << m_readCount;
    if(m_readCount > 0  && m_readCount < m_numFrames-1)
    {
        if(m_currentFrame.channels() == 3)
        {
            cvtColor(m_currentFrame, m_currentFrameGray, CV_BGR2GRAY);
        }
        if(m_previousFrame.channels() == 3)
        {
            cvtColor(m_previousFrame, m_previousFrameGray, CV_BGR2GRAY);
        }
        for(int p = 0; p < m_rectangularObjects.size(); p++)
        {
            QString name = m_rectangularObjects[p];
            QRectF prvRect = m_objectsAnnotation->rectObjects[name][m_readCount - 1];
            //qDebug() << "prvRect " << prvRect;
            cv::Rect nextRect = medianFlowTracker->track(m_previousFrameGray, m_currentFrameGray, cv::Rect(prvRect.x(), prvRect.y(), prvRect.width(), prvRect.height()));
            //qDebug() << "nextRect " << QRectF(nextRect.x, nextRect.y, nextRect.width, nextRect.height);
            m_objectsAnnotation->rectObjects[name][m_readCount] = QRectF(nextRect.x, nextRect.y, nextRect.width, nextRect.height);
        }

        for(int p = 0; p < m_polygonalObjects.size(); p++)
        {
            QString name = m_polygonalObjects[p];
            QPolygonF prvPoly = m_objectsAnnotation->polyObjects[name][m_readCount - 1];
            qDebug() << "prvPoly " << prvPoly;
            vector<Point2f> nextPoly = medianFlowTracker->track(m_previousFrameGray, m_currentFrameGray, qtPolygon2cvContour(prvPoly));
            qDebug() << "nextPoly " << cvContour2qtPolygon(nextPoly);
            m_objectsAnnotation->polyObjects[name][m_readCount] = cvContour2qtPolygon(nextPoly);
        }

        for(int i = 0; i < m_rectangularObjects.size(); i++)
        {
            QString name = m_rectangularObjects[i];
            QRectF r = m_objectsAnnotation->rectObjects[name][m_readCount];
            if(!r.isEmpty())
            {
                m_scene->m_items_rectangle[name]->updateRectangle(r);
            }
        }
        for(int i = 0; i < m_polygonalObjects.size(); i++)
        {
            QString name = m_polygonalObjects[i];
            QPolygonF p = m_objectsAnnotation->polyObjects[name][m_readCount];
            if(!p.isEmpty())
            {
                m_scene->m_items_polygon[name]->updatePolygon(p);
            }
        }
    }
}

void interface::rectangleDrawn(const QString &name, const QRectF &pp)
{
    ui->pbNewObject->setEnabled(true);
    ui->groupBox_2->setEnabled(true);
    ui->groupBox_3->setEnabled(true);
    ui->groupBox_4->setEnabled(true);
    ui->groupBox_5->setEnabled(true);
    if(!(pp.width() < 0.0f && pp.height() < 0.0f))
    {
        if(m_objectsAnnotation->rectObjects[name].isEmpty())
        {
            m_objectsAnnotation->rectObjects[name].resize(m_numFrames);
            m_rectangularObjects.push_back(name);
        }
        m_objectsAnnotation->rectObjects[name][m_readCount] = pp;
    }
    else
    {
        if(m_currentObjectName.contains("peg"))
        {
            --m_pegCount;
        }
        else if(m_currentObjectName.contains("ring"))
        {
            --m_ringCount;
        }
        else if(m_currentObjectName.contains("targetpeg"))
        {
            --m_targetPegCount;
        }
        else if(m_currentObjectName.contains("tool"))
        {
            --m_toolCount;
        }
    }


}

void interface::polygonDrawn(const QString & name, const QPolygonF & pp)
{
    ui->pbNewObject->setEnabled(true);
    ui->groupBox_2->setEnabled(true);
    ui->groupBox_3->setEnabled(true);
    ui->groupBox_4->setEnabled(true);
    ui->groupBox_5->setEnabled(true);
    if(!pp.isEmpty())
    {
        if(m_objectsAnnotation->polyObjects[name].isEmpty())
        {
            m_objectsAnnotation->polyObjects[name].resize(m_numFrames);
            m_polygonalObjects.push_back(name);
        }
        m_objectsAnnotation->polyObjects[name][m_readCount] = pp;
    }
    else
    {
        if(m_currentObjectName.contains("peg"))
        {
            --m_pegCount;
        }
        else if(m_currentObjectName.contains("ring"))
        {
            --m_ringCount;
        }
        else if(m_currentObjectName.contains("targetpeg"))
        {
            --m_targetPegCount;
        }
        else if(m_currentObjectName.contains("tool"))
        {
            --m_toolCount;
        }
    }
}

void interface::leftKeyPressed()
{
    if(m_pausePressed)
    {
        m_readCount--;
        m_readCount = m_readCount < 0 ? 0 : m_readCount;
        m_videoHandle.set(CV_CAP_PROP_POS_FRAMES, m_readCount);

        m_currentFrame.copyTo(m_previousFrame);
        m_videoHandle >> m_currentFrame;

        ui->viewer->showImage(m_currentFrame);
        ui->sliderFrame->setValue(m_readCount);
        updatePolygonAndRectangles();
    }
}
void interface::rightKeyPressed()
{
    if(m_pausePressed)
    {
        m_readCount++;
        m_readCount = m_readCount >= m_numFrames-1 ? m_numFrames-1 : m_readCount;
        m_videoHandle.set(CV_CAP_PROP_POS_FRAMES, m_readCount);

        m_currentFrame.copyTo(m_previousFrame);
        m_videoHandle >> m_currentFrame;

        ui->viewer->showImage(m_currentFrame);
        ui->sliderFrame->setValue(m_readCount);
        updatePolygonAndRectangles();
    }
}

void interface::Read()
{
    m_readCount++;
    ui->sliderFrame->setValue(m_readCount);

    m_currentFrame.copyTo(m_previousFrame);
    m_videoHandle >> m_currentFrame;

    ui->viewer->showImage(m_currentFrame);
    updatePolygonAndRectangles();
    if(m_readCount >= m_numFrames-1)
    {
        m_timerVideo->stop();
        m_readCount = 0;
    }
    //m_currentFrameAction = ui->lineEditAction->text().split(",");
    //addActionToAnnotationStructure(m_currentFrameAction);
}


void interface::keyPressEvent(QKeyEvent *e)
{
    //qDebug() << e->text() ;
    if(e->text() == "p")
    {
        if(m_pausePressed)
        {
            m_timerVideo->start((int) (1000/m_fpsVideo));
            m_pausePressed = false;
        }
        else
        {
            m_timerVideo->stop();
            m_pausePressed = true;
            //ui->viewer->setPause(m_pausePressed);
        }
    }
    else if(e->text() == "q")
    {
        QApplication::instance()->exit();
    }
}

void interface::SliderFramePress()
{
    m_timerVideo->stop();
}

void interface::SliderFrameRelease()
{
    m_timerVideo->start((int) (1000/m_fpsVideo));
}

void interface::SliderFrameMove(int k)
{
    m_videoHandle.set(CV_CAP_PROP_POS_FRAMES, k);

    m_currentFrame.copyTo(m_previousFrame);
    m_videoHandle >> m_currentFrame;

    ui->viewer->showImage(m_currentFrame);
    m_readCount = k;
    updatePolygonAndRectangles();
}

void interface::on_pbQuit_clicked()
{
    QApplication::instance()->exit();
}

void interface::on_pbLoad_clicked()
{
    ui->drawing->setSceneRect(0,0, ui->viewer->geometry().width(), ui->viewer->geometry().height());
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Video file"), "", tr("Video Files (*.avi)"));


    QFileInfo checkFile(fileName);
    if(!checkFile.isFile())
    {
        QMessageBox::warning(this, tr("Warning"), "File doesn't exist..");
    }
    else
    {
        //qDebug() << "Size of the screen " << QSize(ui->viewer->width(), ui->viewer->height());
        m_objectsAnnotation = new objectsAnnotations(QSize(ui->viewer->width(), ui->viewer->height()));
        ui->pbNewObject->setEnabled(true);

        m_videoHandle.open(fileName.toStdString());
        m_frameH = m_videoHandle.get(CV_CAP_PROP_FRAME_HEIGHT);
        m_frameW = m_videoHandle.get(CV_CAP_PROP_FRAME_WIDTH);
        m_fpsVideo =  m_videoHandle.get(CV_CAP_PROP_FPS);
        m_numFrames = m_videoHandle.get(CV_CAP_PROP_FRAME_COUNT);

        ui->sliderFrame->setEnabled(true);
        ui->sliderFrame->setMaximum(m_numFrames-1);

        m_pausePressed = true;
        m_readCount = 0;
        ui->sliderFrame->setValue(m_readCount);
        m_videoHandle >> m_currentFrame;
        m_previousFrame = m_currentFrame.clone();
        ui->viewer->showImage(m_currentFrame);
        ui->pbPlay->setEnabled(true);
        ui->pbLoad->setEnabled(false);
        ui->btnTrackObjects->setEnabled(true);
        ui->pbPlay->setText("Play");


        //m_currentFrameAction = ui->lineEditAction->text().split(",");
        //addActionToAnnotationStructure(m_currentFrameAction);
        //ui->viewer->setFixedSize(QSize(m_currentFrame.cols, m_currentFrame.rows));
        //ui->viewer->setGeometry(9, 55, m_currentFrame.cols, m_currentFrame.rows);
        //ui->viewer->resize(m_currentFrame.cols, m_currentFrame.rows);
    }
}

void interface::filldummy()
{
    //    for(uint i = 0; i < m_objectsAnnotation.size(); i++)
    //    {
    //        m_objectsAnnotation[i].frameNo = i+1;
    //        Object p;
    //        p.rectOrPoly = false;
    //        p.rect = cv::Rect(i % m_objectsAnnotation.size(), i % m_objectsAnnotation.size(), 20, 20);
    //        m_objectsAnnotation[i].objects.push_back(p);
    //    }
}

void interface::on_pbPlay_clicked()
{
    if(m_pausePressed)
    {
        m_timerVideo->start((int) (1000/m_fpsVideo));
        m_pausePressed = false;
        ui->pbPlay->setText("Pause");
    }
    else
    {
        m_timerVideo->stop();
        m_pausePressed = true;
        ui->pbPlay->setText("Play");
    }
}


void interface::on_sliderFrame_valueChanged(int value)
{
    ui->labelSliderCount->setText(QString::number(value));
}



void interface::on_pbNewObject_clicked()
{
    ui->groupBox_2->setEnabled(false);
    ui->groupBox_3->setEnabled(false);
    ui->groupBox_4->setEnabled(false);
    ui->groupBox_5->setEnabled(false);

    if(ui->radioDrawPolygon->isChecked())
    {
        m_drawingMethod = interface::kDrawfromPolgon;
    }
    else if(ui->radioDrawRectangle->isChecked())
    {
        m_drawingMethod = interface::kDrawfromRectangle;
    }

    if(ui->radioObjectPeg->isChecked())
    {
        m_currentObjectName = "peg" + QString::number(++m_pegCount);
    }
    else if(ui->radioObjectRing->isChecked())
    {
        m_currentObjectName = "ring" + QString::number(++m_ringCount);
    }
    else if(ui->radioObjectTargetPeg->isChecked())
    {
        m_currentObjectName = "targetpeg" + QString::number(++m_targetPegCount);
    }
    else if(ui->radioObjectTool->isChecked())
    {
        m_currentObjectName = "tool" + QString::number(++m_toolCount);
    }

    if(m_drawingMethod == interface::kDrawfromPolgon)
    {
        m_scene->setDrawingParams(GraphicsScene::kDrawfromPolgon, m_currentObjectName);
    }
    else if(m_drawingMethod == interface::kDrawfromRectangle)
    {
        m_scene->setDrawingParams(GraphicsScene::kDrawfromRectangle, m_currentObjectName);
    }
}

void interface::on_pushButton_clicked()
{
    m_objectsAnnotation->print();
    //move all the rectangular items;
    //    for(auto e : rectObjects.keys())
    //    {
    //        qDebug() << "Name - " << e;
    //        QVector<QRectF> ff = rectObjects.value(e);
    //        for(uint i = 0; i < ff.size(); i++)
    //        {
    //            if(!ff[i].isEmpty())
    //            {
    //                qDebug() << "Frame - " << i+1 << " Data: " << ff[i];
    //            }
    //        }
    //    }
    //    for(auto e : m_scene->m_items_rectangle.keys())
    //    {
    //        QRectF ff = m_scene->m_items_rectangle.value(e)->getRectangle();
    //        ff.setX(ff.x() + 15);
    //        ff.setY(ff.y() + 15);
    //        ff.setWidth(20);
    //        ff.setHeight(20);
    //        m_scene->m_items_rectangle[e]->updateRectangle(ff);
    //    }
    //    for(auto e : m_scene->m_items_polygon.keys())
    //    {
    //        QPolygonF ff = m_scene->m_items_polygon.value(e)->getPolygon();
    //        for(int i = 0; i < ff.size(); i++)
    //        {
    //            ff[i] = ff[i] + QPointF(5,5);
    //        }
    //        m_scene->m_items_polygon[e]->updatePolygon(ff);
    //    }
    //    cv::Mat dest;
    //    cv::resize(m_currentFrame, dest, cv::Size(1719, 948));
    //    cv::imshow("hy", dest);
    //qDebug() << "m_rectangularObjects " << m_rectangularObjects;
    //qDebug() << "m_polygonalObjects " << m_polygonalObjects;


}

void interface::on_btnTrackObjects_clicked()
{
    int lastTrackedRectFrameNo = -1;
    int current_frameNo = m_readCount;
    cv::Mat prFrame, crFrame;
    // Track all rectangular objects
    if(m_readCount > 0)
    {
        // Get the last tracked frame
        if(!m_rectangularObjects.isEmpty())
        {
            QString name = m_rectangularObjects[0];
            QVector<QRectF> r = m_objectsAnnotation->rectObjects[name];
            for(int i = 0; i < r.size(); i++)
            {
                if(r[i].isEmpty())
                {
                    lastTrackedRectFrameNo = i-1;
                    break;
                }
            }
        }
        if(!m_polygonalObjects.isEmpty())
        {
            QString name = m_polygonalObjects[0];
            QVector<QPolygonF> r = m_objectsAnnotation->polyObjects[name];
            for(int i = 0; i < r.size(); i++)
            {
                if(r[i].isEmpty())
                {
                    lastTrackedRectFrameNo = i-1;
                    break;
                }
            }
        }
        //
        if(current_frameNo > lastTrackedRectFrameNo)
        {
            m_videoHandle.set(CV_CAP_PROP_POS_FRAMES, lastTrackedRectFrameNo);
            m_videoHandle >> prFrame;
            cv::resize(prFrame, prFrame, cv::Size(ui->viewer->width(), ui->viewer->height()));
            for(int i = lastTrackedRectFrameNo; i < current_frameNo; i++)
            {
                m_videoHandle >> crFrame;
                cv::resize(crFrame, crFrame, cv::Size(ui->viewer->width(), ui->viewer->height()));
                if(crFrame.channels() == 3)
                {
                    cvtColor(crFrame, crFrame, CV_BGR2GRAY);
                }
                if(prFrame.channels() == 3)
                {
                    cvtColor(prFrame, prFrame, CV_BGR2GRAY);
                }
                for(int p = 0; p < m_rectangularObjects.size(); p++)
                {
                    QString name = m_rectangularObjects[p];
                    QRectF prvRect = m_objectsAnnotation->rectObjects[name][i];
                    cv::Rect nextRect = medianFlowTracker->track(prFrame, crFrame, cv::Rect(prvRect.x(), prvRect.y(), prvRect.width(), prvRect.height()));
                    m_objectsAnnotation->rectObjects[name][i+1] = QRectF(nextRect.x, nextRect.y, nextRect.width, nextRect.height);
                }
                for(int p = 0; p < m_polygonalObjects.size(); p++)
                {
                    QString name = m_polygonalObjects[p];
                    QPolygonF prvPoly = m_objectsAnnotation->polyObjects[name][i];
                    vector<Point2f> nextPoly = medianFlowTracker->track(prFrame, crFrame, qtPolygon2cvContour(prvPoly));
                    m_objectsAnnotation->polyObjects[name][i+1] = cvContour2qtPolygon(nextPoly);
                }
                crFrame.copyTo(prFrame);
            }
        }
    }
}

vector<Point2f> interface::qtPolygon2cvContour(const QPolygonF &inp)
{
    vector<Point2f> out(inp.size());
    QPointF p;
    for(int i = 0; i < inp.size(); i++)
    {
        out[i] = Point2f(inp[i].x(), inp[i].y());
    }
    return out;
}

QPolygonF interface::cvContour2qtPolygon(const vector<Point2f> & inp)
{
    QPolygonF out(inp.size());
    for(int i = 0; i < inp.size(); i++)
    {
        out[i] = QPointF(inp[i].x, inp[i].y);
    }
    return out;
}

