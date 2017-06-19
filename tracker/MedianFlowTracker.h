

#ifndef MEDIANFLOWTRACKER_H_
#define MEDIANFLOWTRACKER_H_

#include <opencv/cv.h>

#include <cmath>

#include "tracker/FBTrack.h"

#include <cstdio>
#include "tracker/BB.h"
#include "tracker/BBPredict.h"
#include "tracker/Median.h"
#include "tracker/Lk.h"

using namespace std;
using namespace  cv;

class MedianFlowTracker
{
public:
    MedianFlowTracker();
    ~MedianFlowTracker();
    cv::Rect track(const cv::Mat &prevImg, const cv::Mat &currImg, const cv::Rect &prevBB);

    std::vector<cv::Point2f> track(const cv::Mat &prevImg, const cv::Mat &currImg, const cv::vector<cv::Point2f> &poly);

    vector<Point2f> samplePointsInsidePolygon(const vector<Point2f> &inputPolygon);

    int predictTrackedPolygon(const vector<Point2f> &inputPolygon, CvPoint2D32f *pt0, CvPoint2D32f *pt1, int nPts,
                              vector<Point2f> &outPolygon, float *shift);
};
#endif
