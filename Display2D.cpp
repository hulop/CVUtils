/*******************************************************************************
 * Copyright (c) 2017  IBM Corporation, Carnegie Mellon University and others
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *******************************************************************************/

#include "Display2D.hpp"



Mat Display2D::displayFeaturesOnFrame(const cv::Mat &img, const vector<Point2d> &pts, int radius, Scalar colour, float scale) {

    //set input
    Mat dShow;
    if (img.channels() == 3)
        dShow = img.clone();
    else
        cvtColor(img,dShow,CV_GRAY2BGR);
    
    //draw matches
    for (int i = 0; i < pts.size(); i++) {
        if ((pts[i].x >= 0) && (pts[i].x < dShow.cols) && (pts[i].y >= 0) && (pts[i].y < dShow.rows)) {
            circle(dShow, pts[i], radius, colour, -1, CV_AA);
        }
    }
    
    //scale down
    Mat dShow_small;
    resize(dShow, dShow_small, Size(round(scale*dShow.cols), round(scale*dShow.rows)));

    return dShow_small;
}

Mat Display2D::displayFeaturesOnFrame(const cv::Mat &img, const vector<Point2f> &pts, int radius, Scalar colour, float scale) {
    
    //set input
    Mat dShow;
    if (img.channels() == 3)
        dShow = img.clone();
    else
        cvtColor(img,dShow,CV_GRAY2BGR);
    
    //draw matches
    for (int i = 0; i < pts.size(); i++) {
        if ((pts[i].x >= 0) && (pts[i].x < dShow.cols) && (pts[i].y >= 0) && (pts[i].y < dShow.rows)) {
            circle(dShow, pts[i], radius, colour, -1, CV_AA);
        }
    }
    
    //scale down
    Mat dShow_small;
    resize(dShow, dShow_small, Size(round(scale*dShow.cols), round(scale*dShow.rows)));
    
    return dShow_small;
}

Mat Display2D::displayFeatureMatches(const cv::Mat &img0, const cv::Mat &img1, const vector<Point2d> &pts0, const vector<Point2d> &pts1, int radius, Scalar colour, float scale) {
    
    //build match structure
    vector<DMatch> matches;
    vector<KeyPoint> k0, k1;
    for (int i = 0; i < pts0.size(); i++) {
        matches.push_back(DMatch(i,i,0));
        k0.push_back(KeyPoint(pts0[i], 0));
        k1.push_back(KeyPoint(pts1[i], 0));
    }

    
    //draw matches
    Mat dShow;
    drawMatches(img0, k0, img1, k1, matches, dShow, colour);

    //scale down
    Mat dShow_small;
    resize(dShow, dShow_small, Size(round(scale*dShow.cols), round(scale*dShow.rows)));
    
    return dShow_small;
}

Mat Display2D::displayFeatureMatches(const cv::Mat &img0, const cv::Mat &img1, const vector<Point2f> &pts0, const vector<Point2f> &pts1, int radius, Scalar colour, float scale) {
    
    //build match structure
    vector<DMatch> matches;
    vector<KeyPoint> k0, k1;
    for (int i = 0; i < pts0.size(); i++) {
        matches.push_back(DMatch(i,i,0));
        k0.push_back(KeyPoint(pts0[i], 0));
        k1.push_back(KeyPoint(pts1[i], 0));
    }
    
    
    //draw matches
    Mat dShow;
    drawMatches(img0, k0, img1, k1, matches, dShow, colour);
    
    //scale down
    Mat dShow_small;
    resize(dShow, dShow_small, Size(round(scale*dShow.cols), round(scale*dShow.rows)));
    
    return dShow_small;
}


Mat Display2D::display3DProjections(const cv::Mat &img, const Matx33d &K, const Matx33d &R, const Matx31d &t, const vector<Matx31d> &pts, int radius, Scalar colour, float scale) {
    
    //set input
    Mat dShow;
    if (img.channels() == 3)
        dShow = img.clone();
    else
        cvtColor(img,dShow,CV_GRAY2BGR);
    
    //build projection matrix
    Matx34d P(R(0,0), R(0,1), R(0,2), t(0), R(1,0), R(1,1), R(1,2), t(1), R(2,0), R(2,1), R(2,2), t(2));

    //project points and draw
    vector<Point2d> pts2D;
    for (int i = 0; i < pts.size(); i++) {
        Matx41d pt3D(pts[i](0),pts[i](1),pts[i](2),1);
        Matx31d ptn = P*pt3D;
        ptn *= 1.0/ptn(2);
        Matx31d pt2D = K*ptn;
        pt2D *= 1.0/pt2D(2);
        //check boundaries
        if ((pt2D(0) >= 0) && (pt2D(0) < img.cols) && (pt2D(1) >= 0) && (pt2D(1) < img.rows)) {
            pts2D.push_back(Point2d(pt2D(0),pt2D(1)));
            circle(dShow, Point2d(pt2D(0),pt2D(1)), radius, colour, -1, CV_AA );
        } 
    }
    
    //scale down
    Mat dShow_small;
    resize(dShow, dShow_small, Size(round(scale*dShow.cols), round(scale*dShow.rows)));
    
    return dShow_small;
}

Mat Display2D::displayEpipolarLines(const cv::Mat &img0, const cv::Mat &img1, const Matx33d &F, const vector<Point2d> pts, int pts0or1, int nFeatures, int radius, Scalar colour, float scale) {
    
    //prepare input
    Mat dShow0, dShow1, dShow;
    if (img0.channels() == 3) {
        dShow0 = img0.clone();
        dShow1 = img1.clone();
    }
    else {
        cvtColor(img0,dShow0,CV_GRAY2BGR);
        cvtColor(img1,dShow1,CV_GRAY2BGR);
    }
    
    //compute epipolar lines
    vector<Vec3d> epiLines;
    if (pts0or1 == 0)
        computeCorrespondEpilines(pts, 1, F, epiLines);
    else
        computeCorrespondEpilines(pts, 2, F, epiLines);
    
    //draw epilines
    if (nFeatures <= 0)
        nFeatures = pts.size();
    
    for (int i = 0; i < nFeatures; i++) {
        
        if (pts0or1 == 0) {
            //draw feature
            circle(dShow0,pts[i],radius,colour,-1,CV_AA);
            //draw corresponding epiline
            line(dShow1, Point2d(0,-epiLines[i][2]/epiLines[i][1]),Point2d(dShow1.cols,-(epiLines[i][0]*dShow1.cols + epiLines[i][2])/epiLines[i][1]),colour,1,CV_AA);
        }
        else {
            //draw feature
            circle(dShow1,pts[i],radius,colour,-1,CV_AA);
            //draw corresponding epiline
            line(dShow0, Point2d(0,-epiLines[i][2]/epiLines[i][1]),Point2d(dShow1.cols,-(epiLines[i][0]*dShow1.cols + epiLines[i][2])/epiLines[i][1]),colour,1,CV_AA);
        }
    }
    
    //concatenate images
    hconcat(dShow0,dShow1,dShow);
    
    //scale down
    Mat dShow_small;
    resize(dShow, dShow_small, Size(round(scale*dShow.cols), round(scale*dShow.rows)));
    
    return dShow_small;
}


Mat Display2D::drawCubeWireframe(const Mat &img, const Matx33d &K, const Matx34d &P, const vector<Matx31d> &frontFace, const vector<Matx31d> &backFace, int thickness, Scalar colour, float scale) {
    
    //set input
    Mat dShow;
    if (img.channels() == 3)
        dShow = img.clone();
    else
        cvtColor(img,dShow,CV_GRAY2BGR);
    
    if ((frontFace.size() == 4) && (backFace.size() == 4)) {
        //project cube points onto image plane
        vector<Point2d> front2D, back2D;
        GeometryUtils::projectPoints(P, K, frontFace, front2D);
        GeometryUtils::projectPoints(P, K, backFace, back2D);
    
        //display lines
        for (int i = 0; i < 4; i++) {
            int nextPtIdx = (i+1) % 4;
            line(dShow, front2D[i], front2D[nextPtIdx], colour, thickness, CV_AA);
            line(dShow, back2D[i], back2D[nextPtIdx], colour, thickness, CV_AA);
            line(dShow, front2D[i], back2D[i], colour, thickness, CV_AA);
        }
    
    }
    //scale down
    Mat dShow_small;
    resize(dShow, dShow_small, Size(round(scale*dShow.cols), round(scale*dShow.rows)));
    
    return dShow_small;
}

Mat Display2D::drawRotatedRectangle(const Mat &img, const RotatedRect &rect, int thickness, Scalar colour, float scale) {
    //set input
    Mat dShow;
    if (img.channels() == 3)
        dShow = img.clone();
    else
        cvtColor(img,dShow,CV_GRAY2BGR);
    
    Point2f vtx[4], center;
    rect.points(vtx);
    center = rect.center;
    for (int i = 0; i < 4; i++) {
        line(dShow, vtx[i], vtx[(i+1)%4], colour, thickness, LINE_AA);
    }
    circle(dShow, center, thickness, colour, -1, CV_AA);
    
    //scale down
    Mat dShow_small;
    resize(dShow, dShow_small, Size(round(scale*dShow.cols), round(scale*dShow.rows)));
    
    return dShow_small;
}
