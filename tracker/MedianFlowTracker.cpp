

#include "tracker/MedianFlowTracker.h"

#include <cmath>

#include "tracker/FBTrack.h"

using namespace cv;


MedianFlowTracker::MedianFlowTracker()
{
}

MedianFlowTracker::~MedianFlowTracker()
{
}

cv::Rect MedianFlowTracker::track(const Mat &prevMat, const Mat &currMat, const Rect &prevBB)
{
    Rect outRect = Rect(-1,-1,-1,-1);

    if(prevBB.width <= 0 || prevBB.height <= 0)
    {
        return outRect;
    }

    float bb_tracker[] = {prevBB.x, prevBB.y, prevBB.width + prevBB.x - 1, prevBB.height + prevBB.y - 1};
    float scale;

    IplImage prevImg = prevMat;
    IplImage currImg = currMat;

    int success = fbtrack(&prevImg, &currImg, bb_tracker, bb_tracker, &scale);

    //Extract subimage
    float x, y, w, h;
    x = floor(bb_tracker[0] + 0.5);
    y = floor(bb_tracker[1] + 0.5);
    w = floor(bb_tracker[2] - bb_tracker[0] + 1 + 0.5);
    h = floor(bb_tracker[3] - bb_tracker[1] + 1 + 0.5);

    //TODO: Introduce a check for a minimum size
    if(!success || x < 0 || y < 0 || w <= 0 || h <= 0 || x + w > currMat.cols || y + h > currMat.rows || x != x || y != y || w != w || h != h) //x!=x is check for nan
    {
        //Leave it empty
    }
    else
    {
        outRect = Rect(x,y,w,h);
    }

    return outRect;
}

vector<Point2f> MedianFlowTracker::samplePointsInsidePolygon(const vector<Point2f> &inputPolygon)
{
    vector<Point2f> pts;
    vector<Point2f> returnPts;

    Rect boundRect = boundingRect(Mat(inputPolygon));

    int numM = boundRect.width / 2;
    int numN = boundRect.height /2;
    int margin = 5;
    int i;
    int j;
    /**
      * gap between points in width direction
      */
    float divN;
    /**
      * gap between points in height direction
      */
    float divM;
    float bb_local[4];
    float spaceN;
    float spaceM;


    bb_local[0] = boundRect.tl().x + margin;
    bb_local[1] = boundRect.tl().y + margin;
    bb_local[2] = boundRect.br().x - margin;
    bb_local[3] = boundRect.br().y - margin;


    divM = numM - 1;
    divN = numN - 1;


    //calculate step width
    spaceN = (bb_local[2] - bb_local[0]) / divN;
    spaceM = (bb_local[3] - bb_local[1]) / divM;

    //calculate points and save them to the array
    for(i = 0; i < numN; i++)
    {
        for(j = 0; j < numM; j++)
        {
            pts.push_back(Point2f(bb_local[0] + i * spaceN, bb_local[1] + j * spaceM));
        }
    }

    for(int i = 0 ; i < pts.size(); i++)
    {
        if(pointPolygonTest( inputPolygon, pts[i], false ) >= 0)
        {
            returnPts.push_back(pts[i]);
        }
    }
    return returnPts;
}

int MedianFlowTracker::predictTrackedPolygon(const vector<Point2f> &inputPolygon, CvPoint2D32f *pt0, CvPoint2D32f *pt1, int nPts,
                          vector<Point2f> &outPolygon, float *shift)
{
    outPolygon.clear();
    outPolygon.resize(inputPolygon.size());
    Moments mu = cv::moments(inputPolygon, false);
    float cx = mu.m10/mu.m00;
    float cy = mu.m01/mu.m00;

    float *ofx = (float *) malloc(sizeof(float) * nPts);
    float *ofy = (float *) malloc(sizeof(float) * nPts);
    int i;
    int j;
    int d = 0;
    float dx, dy;
    int lenPdist;
    float *dist0;
    float *dist1;
    float s0, s1;

    for(i = 0; i < nPts; i++)
    {
        ofx[i] = pt1[i].x - pt0[i].x;
        ofy[i] = pt1[i].y - pt0[i].y;
    }

    dx = getMedianUnmanaged(ofx, nPts);
    dy = getMedianUnmanaged(ofy, nPts);
    free(ofx);
    ofx = 0;
    free(ofy);
    ofy = 0;
    //m(m-1)/2
    lenPdist = nPts * (nPts - 1) / 2;
    dist0 = (float *) malloc(sizeof(float) * lenPdist);
    dist1 = (float *) malloc(sizeof(float) * lenPdist);

    for(i = 0; i < nPts; i++)
    {
        for(j = i + 1; j < nPts; j++, d++)
        {
            dist0[d]
            = sqrt(pow(pt0[i].x - pt0[j].x, 2) + pow(pt0[i].y - pt0[j].y, 2));
            dist1[d]
            = sqrt(pow(pt1[i].x - pt1[j].x, 2) + pow(pt1[i].y - pt1[j].y, 2));
            dist0[d] = dist1[d] / dist0[d];
        }
    }

    //The scale change is the median of all changes of distance.
    //same as s = median(d2./d1) with above
    *shift = getMedianUnmanaged(dist0, lenPdist);
    free(dist0);
    dist0 = 0;
    free(dist1);
    dist1 = 0;

    // translate the points
    for(int i = 0; i < inputPolygon.size(); i++)
    {
        outPolygon[i].x = inputPolygon[i].x + dx;
        outPolygon[i].y = inputPolygon[i].y + dy;
    }

    // scale points by shift
    for(int i = 0; i < outPolygon.size(); i++)
    {
        Point2f pt = outPolygon[i];
        // move to centre
        pt.x -= cx;
        pt.y -= cy;
        // scale by shift
        pt.x *= *shift;
        pt.y *= *shift;
        // move to original place
        pt.x += cx;
        pt.y += cy;
        outPolygon[i] = pt;
    }

    return 1;

}

std::vector<cv::Point2f> MedianFlowTracker::track(const cv::Mat &prevMat, const cv::Mat &currMat, const cv::vector<cv::Point2f> &_polygon)
{
    cv::vector<cv::Point2f> poly = samplePointsInsidePolygon(_polygon);

    std::vector<cv::Point2f> outPoly;

    IplImage prevImg = prevMat;
    IplImage currImg = currMat;

    // get the previous and current
    IplImage *imgI = &prevImg;
    IplImage *imgJ = &currImg;

    // variables to be used
    int nlkPoints;
    int i, M;
    int nRealPoints;
    float medFb;
    float medNcc;
    int nAfterFbUsage;
    int level = 5;
    float scale;
    // ptrs
    CvPoint2D32f *startPoints;
    CvPoint2D32f *targetPoints;
    float *fbLkCleaned;
    float *nccLkCleaned;


    int nPoints = poly.size();

    float *fb  = (float*)malloc( sizeof(float) * nPoints);
    float *ncc = (float*)malloc( sizeof(float) * nPoints);
    char *status = (char*)malloc( sizeof(char) * nPoints);

    float *pt = (float*)malloc( sizeof(float) * nPoints* 2);
    float *ptTracked = (float*)malloc( sizeof(float) * nPoints* 2);

    //getFilledBBPoints(bb, numM, numN, 5, pt);
    // copy polygon points to point to be tracked
    for(int p = 0; p < poly.size(); p++)
    {
        pt[2 * p] = poly[p].x;
        pt[2 * p + 1] = poly[p].y;
    }
    memcpy(ptTracked, pt, sizeof(float)*nPoints*2);


    initImgs();
    trackLK(imgI, imgJ, pt, nPoints, ptTracked, nPoints, level, fb, ncc, status);
    initImgs();
    //  char* status = *statusP;
    nlkPoints = 0;
    for(i = 0; i < nPoints; i++)
    {
        nlkPoints += status[i];
    }

    startPoints = (CvPoint2D32f *) malloc(nlkPoints * sizeof(CvPoint2D32f));
    targetPoints = (CvPoint2D32f *) malloc(nlkPoints * sizeof(CvPoint2D32f));
    fbLkCleaned = (float *) malloc(nlkPoints * sizeof(float));
    nccLkCleaned = (float *) malloc(nlkPoints * sizeof(float));

    M = 2;
    nRealPoints = 0;
    for(i = 0; i < nPoints; i++)
    {
        //TODO:handle Missing Points
        //or status[i]==0
        if(ptTracked[M * i] == -1)
        {
        }
        else
        {
            startPoints[nRealPoints].x = pt[2 * i];
            startPoints[nRealPoints].y = pt[2 * i + 1];
            targetPoints[nRealPoints].x = ptTracked[2 * i];
            targetPoints[nRealPoints].y = ptTracked[2 * i + 1];
            fbLkCleaned[nRealPoints] = fb[i];
            nccLkCleaned[nRealPoints] = ncc[i];
            nRealPoints++;
        }
    }

    //    //assert nRealPoints==nlkPoints
    medFb = getMedian(fbLkCleaned, nlkPoints);
    medNcc = getMedian(nccLkCleaned, nlkPoints);
    /*  printf("medianfb: %f\nmedianncc: %f\n", medFb, medNcc);
     printf("Number of points after lk: %d\n", nlkPoints);*/
    nAfterFbUsage = 0;
    for(i = 0; i < nlkPoints; i++)
    {
        if((fbLkCleaned[i] <= medFb) & (nccLkCleaned[i] >= medNcc))
        {
            startPoints[nAfterFbUsage] = startPoints[i];
            targetPoints[nAfterFbUsage] = targetPoints[i];
            nAfterFbUsage++;
        }
    }
    predictTrackedPolygon(_polygon, startPoints, targetPoints, nAfterFbUsage, outPoly, &scale);

    free(startPoints);
    free(targetPoints);
    free(fbLkCleaned);
    free(nccLkCleaned);
    free(pt);
    free(ptTracked);
    free(fb);
    free(ncc);
    free(status);

    return outPoly;
}






