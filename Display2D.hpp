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

#ifndef Display2D_hpp
#define Display2D_hpp

#include <stdio.h>
#include <opencv2/opencv.hpp>



using namespace std;
using namespace cv;


class Display2D {

public:
    
    static Mat displayFeaturesOnFrame(const Mat &img, const vector<Point2d> &pts, int radius = 3, Scalar colour = Scalar(255,0,0), float scale = 0.5);

    static Mat displayFeaturesOnFrame(const Mat &img, const vector<Point2f> &pts, int radius = 3, Scalar colour = Scalar(255,0,0), float scale = 0.5);
    
    static Mat displayFeatureMatches(const Mat &img0, const Mat &img1, const vector<Point2d> &pts0, const vector<Point2d> &pts1, int radius = 3, Scalar colour = Scalar(255,0,0), float scale = 0.5);
    
    static Mat displayFeatureMatches(const Mat &img0, const Mat &img1, const vector<Point2f> &pts0, const vector<Point2f> &pts1, int radius = 3, Scalar colour = Scalar(255,0,0), float scale = 0.5);
    
    static Mat display3DProjections(const Mat &img, const Matx33d &K, const Matx33d &R, const Matx31d &t, const vector<Matx31d> &pts, int radius = 3, Scalar colour = Scalar(255,0,0), float scale = 0.5);
    
    static Mat displayEpipolarLines(const cv::Mat &img0, const cv::Mat &img1, const Matx33d &F, const vector<Point2d> pts, int pts0or1, int nFeatures = 10, int radius = 3, Scalar colour = Scalar(255,0,0), float scale = 0.5);

};

#endif /* Display2D_hpp */
