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

#ifndef GeometryUtils_hpp
#define GeometryUtils_hpp

#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class GeometryUtils {
    
public:
    //triangulation
    static void triangulatePoints(const Matx34d &P0, const Matx34d &P1, const Matx33d &K0, const Matx33d &K1, const vector<Point2d> &f0, const vector<Point2d> &f1, vector<Matx31d> &outPts);
    static void triangulatePoints(const Matx34d &P0, const Matx34d &P1, const Matx33d &K0, const Matx33d &K1, const vector<Point2f> &f0, const vector<Point2f> &f1, vector<Matx31d> &outPts);
    
    //projection
    static void projectPoints(const Matx34d &P, const Matx33d& K, const vector<Matx31d> &pts3D, vector<Point2d> &pts2D);
    static void projectPoints(const Matx33d &R, const Matx31d& t, const Matx33d& K, const vector<Matx31d> &pts3D, vector<Point2d> &pts2D);
    static Point2d projectPoint(const Matx33d &R, const Matx31d &t, const Matx33d &K, const Matx31d &pt3D);
    static Point2d projectPoint(const Matx34d &P, const Matx33d &K, const Matx31d &pt3D);
    static Point2d projectPoint(const Matx34d &P, const Matx33d &K, const double* pt3D);
    
    //matrix decomposition
    static bool RtFromEssentialMatrix(const Matx33d &E, const Matx33d &K0, const Matx33d &K1, const vector<Point2d> &pts0, const vector<Point2d> &pts1, Matx33d &R, Vec3d &t);
    static bool RtFromEssentialMatrix(const Matx33f &E, const Matx33f &K0, const Matx33f &K1, const vector<Point2f> &pts0, const vector<Point2f> &pts1, Matx33d &R, Vec3d &t);
    static bool RtFromHomographyMatrix(const Matx33f &H, const Matx33f &K0, const Matx33f &K1, const vector<Point2f> &pts0, const vector<Point2f> &pts1, Matx33d &R, Vec3d &t);
    static void calculateFundamentalMatrix(const Matx33d &K0, const Matx33d &R0, const Matx31d &t0, const Matx33d &K1, const Matx33d &R1, const Matx31d &t1, Matx33d &F);
    static Matx33d getSkewSymmetric(const Matx31d &v);
    
    //projection errors
    static double calculateFundamentalAvgError(const vector<Point2d> &pts0, const vector<Point2d> &pts1, const Matx33d &F);
    static double calculateHomographyAvgError(const vector<Point2d> &pts0, const vector<Point2d> &pts1, const Matx33d &H);
    static float calculateFundamentalAvgError(const vector<Point2f> &pts0, const vector<Point2f> &pts1, const Matx33f &F);
    static float calculateHomographyAvgError(const vector<Point2f> &pts0, const vector<Point2f> &pts1, const Matx33f &H);
    
    //filtering outliers
    static int filterOutliers(const Matx34d &P, const Matx33d &K, const Size &imSize, const vector<Matx31d> &pts3D, const vector<Point2f> &pts2D, vector<uchar> &status, double threshold = 3.0);
    static int filterOutliers(const Matx34d &P, const Matx33d &K, const Size &imSize, const vector<Matx31d> &pts3D, const vector<Point2d> &pts2D, vector<uchar> &status, double threshold = 3.0);
    static int filterMatches(const Matx33d &F, const vector<Point2d> &pts0, const vector<Point2d> &pts1, vector<uchar> &status, double distThreshold);
    static int filterMatches(const Matx33f &F, const vector<Point2f> &pts0, const vector<Point2f> &pts1, vector<uchar> &status, double distThreshold);
    static int filterMatches(const Matx33f &F, const vector<Point2f> &pts0, const vector<Point2f> &pts1, vector<Matx31d> &pts3D, vector<uchar> &status, double distThreshold);
    static int filterMatches(const Matx33d &F, const vector<Point2d> &pts0, const vector<Point2d> &pts1, vector<Matx31d> &pts3D, vector<uchar> &status, double distThreshold);
    
    //geometry operations
    static double distancePointLine2D(const Point2d &pt, const Vec3d &l);
    
private:
    
    static Matx31d linearTriangulation(const Matx34d &P0, const Matx34d &P1, const Point3d pt0, const Point3d pt1, int iter = 10);//
};

#endif /* GeometryUtils_hpp */
