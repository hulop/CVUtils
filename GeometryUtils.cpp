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

#include "GeometryUtils.hpp"

Matx31d GeometryUtils::linearTriangulation(const Matx34d &P0, const Matx34d &P1, const Point3d pt0, const Point3d pt1, int iter) {
    
    //TODO: include two or three equations from each image?
    //TODO: currently using inhomogeneous solution
    Mat X;
    double wi = 1, wi1 = 1, p2x = 0, p2x1 = 0;
    double eps = 1e-04;
    for(int i =0; i < iter; i++) {
        
        Matx43d A((pt0.x*P0(2,0) - P0(0,0))/wi, (pt0.x*P0(2,1) - P0(0,1))/wi, (pt0.x*P0(2,2) - P0(0,2))/wi,
                  (pt0.y*P0(2,0) - P0(1,0))/wi, (pt0.y*P0(2,1) - P0(1,1))/wi, (pt0.y*P0(2,2) - P0(1,2))/wi,
                  (pt1.x*P1(2,0) - P1(0,0))/wi1, (pt1.x*P1(2,1) - P1(0,1))/wi1, (pt1.x*P1(2,2) - P1(0,2))/wi1,
                  (pt1.y*P1(2,0) - P1(1,0))/wi1, (pt1.y*P1(2,1) - P1(1,1))/wi1, (pt1.y*P1(2,2) - P1(1,2))/wi1);
        Matx41d B(-(pt0.x*P0(2,3) - P0(0,3))/wi, -(pt0.y*P0(2,3) - P0(1,3))/wi, -(pt1.x*P1(2,3) - P1(0,3))/wi1, -(pt1.y*P1(2,3) - P1(1,3))/wi1);
        
        solve(A,B,X,DECOMP_SVD);
        //cout << "Iter " << i << ": " << X << endl;
        
        //check if time to break
        Matx41d xcol(X.ptr<double>(0)[0],X.ptr<double>(0)[1],X.ptr<double>(0)[2],1.0);
        p2x = (P0.row(2)*xcol)(0);
        p2x1 = (P1.row(2)*xcol)(0);
        
        if ((fabs(wi - p2x) <= eps) && (fabs(wi1 - p2x1) <= eps))
            break;
        
        //update weights
        wi = p2x;
        wi1 = p2x1;
    }
    Matx31d sol(X.ptr<double>(0)[0],X.ptr<double>(0)[1],X.ptr<double>(0)[2]);
    return sol;
}

void GeometryUtils::triangulatePoints(const Matx34d &P0, const Matx34d &P1, const Matx33d &K0, const Matx33d &K1, const vector<Point2d> &f0, const vector<Point2d> &f1, vector<Matx31d> &outPts) {
    
    //preallocate for speed
    outPts.reserve(f0.size());
    
    Matx33d K0i = K0.inv();
    Matx33d K1i = K1.inv();
    for (int i = 0; i < f0.size(); i++) {
        Point3d pt0(f0[i].x,f0[i].y,1);
        Point3d pt1(f1[i].x,f1[i].y,1);
        //convert to normalised coordinates
        Point3d pt0n = K0i*pt0;
        Point3d pt1n = K1i*pt1;
        //solve linear system
        Matx31d X = linearTriangulation(P0, P1, pt0n, pt1n, 10);
        outPts.push_back(X);
    }
}

void GeometryUtils::triangulatePoints(const Matx34d &P0, const Matx34d &P1, const Matx33d &K0, const Matx33d &K1, const vector<Point2f> &f0, const vector<Point2f> &f1, vector<Matx31d> &outPts) {
    
    //preallocate for speed
    outPts.reserve(f0.size());
    
    Matx33d K0i = K0.inv();
    Matx33d K1i = K1.inv();
    for (int i = 0; i < f0.size(); i++) {
        Point3d pt0(f0[i].x,f0[i].y,1);
        Point3d pt1(f1[i].x,f1[i].y,1);
        //convert to normalised coordinates
        Point3d pt0n = K0i*pt0;
        Point3d pt1n = K1i*pt1;
        //solve linear system
        Matx31d X = linearTriangulation(P0, P1, pt0n, pt1n, 10);
        outPts.push_back(X);
    }
}

void GeometryUtils::projectPoints(const Matx34d &P, const Matx33d &K, const vector<Matx31d> &pts3D, vector<Point2d> &pts2D) {
    
    Matx34d Pmat = K*P;
    for (int i = 0; i < pts3D.size(); i++) {
        //project point to 2d
        Matx31d pt = Pmat*Matx41d(pts3D[i].val[0],pts3D[i].val[1],pts3D[i].val[2],1.0);
        pts2D.push_back(Point2d(pt.val[0]/pt.val[2],pt.val[1]/pt.val[2]));
    }
}

void GeometryUtils::projectPoints(const Matx33d &R, const Matx31d &t, const Matx33d &K, const vector<Matx31d> &pts3D, vector<Point2d> &pts2D) {
    
    Matx34d P;
    P << R(0,0), R(0,1), R(0,2), t(0), R(1,0), R(1,1), R(1,2), t(1), R(2,0), R(2,1), R(2,2), t(2);
    
    projectPoints(P, K, pts3D, pts2D);
}

Point2d GeometryUtils::projectPoint(const Matx33d &R, const Matx31d &t, const Matx33d &K, const Matx31d &pt3D) {
    
    Matx34d P;
    P << R(0,0), R(0,1), R(0,2), t(0), R(1,0), R(1,1), R(1,2), t(1), R(2,0), R(2,1), R(2,2), t(2);
    
    return projectPoint(P, K, pt3D);
}

Point2d GeometryUtils::projectPoint(const Matx34d &P, const Matx33d &K, const Matx31d &pt3D) {
    
    Matx34d Pmat = K*P;
    Matx31d pt =Pmat*Matx41d(pt3D.val[0],pt3D.val[1],pt3D.val[2],1.0);
    Point2d res(pt.val[0]/pt.val[2],pt.val[1]/pt.val[2]);
    return res;
}

Point2d GeometryUtils::projectPoint(const Matx34d &P, const Matx33d &K, const double *pt3D) {
    Matx34d Pmat = K*P;
    Matx31d pt =Pmat*Matx41d(pt3D[0],pt3D[1],pt3D[2],1.0);
    Point2d res(pt.val[0]/pt.val[2],pt.val[1]/pt.val[2]);
    return res;
}

bool GeometryUtils::RtFromEssentialMatrix(const Matx33d &E, const Matx33d &K0, const Matx33d &K1, const vector<Point2d> &pts0, const vector<Point2d> &pts1,Matx33d &R, Vec3d &t) {
    //find SVD of the essential matrix
    SVD svd(E,SVD::MODIFY_A);
    
    const double minSVDRatio = 0.7;
    const double minGoodRatio = 0.85;
    //    //debug
    //    cout << "U: " << svd.u << endl;
    //    cout << "W: " << svd.w << endl;
    //    cout << "V: " << svd.vt << endl;
    
    //two singular values should be equal and the third zero
    double ratio = fabs(svd.w.ptr<float>(0)[0]/svd.w.ptr<float>(0)[1]);
    if (ratio < 0.7) {
        cerr << "singular values too far apart" << endl;
        return false;
    }
    
    Matx33d W(0,-1,0,1,0,0,0,0,1);
    Matx33d Wt(0,1,0,-1,0,0,0,0,1);
    
    Mat R0 = svd.u*Mat(W)*svd.vt;
    Mat R1 = svd.u*Mat(Wt)*svd.vt;
    Mat t0 = svd.u.col(2);
    Mat t1 = -svd.u.col(2);
    
    //calculate determinant of rotation to check validity of essential matrix
    double d = determinant(R0);
    double tol = 1e-05;
    if (d + 1.0 < tol) {
        svd(-E,SVD::MODIFY_A);
        R0 = svd.u*Mat(W)*svd.vt;
        R1 = svd.u*Mat(Wt)*svd.vt;
        t0 = svd.u.col(2);
        t1 = -svd.u.col(2);
        d = determinant(R0);
    }
    if (d -1.0 > tol) {
        cerr << "Not a proper rotation" << endl;
        return false;
    }
    
    //test all possibilities
    Matx34d P0(1,0,0,0,0,1,0,0,0,0,1,0);
    vector<Mat> rots{R0,R1};
    vector<Mat> trans{t0,t1};
    vector<Matx31d> pts3D;
    int bestCount = 0, bestRIdx, bestTIdx;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            //triangulate points
            //TODO: only triangulate a subset to limit complexity?
            pts3D.clear();
            Matx34d P(rots[i].ptr<float>(0)[0],rots[i].ptr<float>(0)[1],rots[i].ptr<float>(0)[2],trans[j].ptr<float>(0)[0],rots[i].ptr<float>(1)[0],rots[i].ptr<float>(1)[1],rots[i].ptr<float>(1)[2],trans[j].ptr<float>(0)[1],rots[i].ptr<float>(2)[0],rots[i].ptr<float>(2)[1],rots[i].ptr<float>(2)[2],trans[j].ptr<float>(0)[2]);
            triangulatePoints(P0,P,K0,K1,pts0,pts1,pts3D);
            
            //check if points are in front of the camera plane
            int countGood = 0;
            for (int k = 0; k < pts3D.size(); k++) {
                if (pts3D[k].val[2] > 1)
                    countGood++;
            }
            
            //save best transformation
            if (countGood > bestCount) {
                bestCount = countGood;
                bestRIdx = i;
                bestTIdx = j;
            }
        }
    }
    
    if (bestCount/pts3D.size() < minGoodRatio) {
        cerr << "No valid rotations/translations" << endl;
        return false;
    }
    
    R = rots[bestRIdx];
    t = trans[bestTIdx];
    
    return true;
}

bool GeometryUtils::RtFromEssentialMatrix(const Matx33f &E, const Matx33f &K0, const Matx33f &K1, const vector<Point2f> &pts0, const vector<Point2f> &pts1,Matx33d &R, Vec3d &t) {
    //find SVD of the essential matrix
    SVD svd(E,SVD::MODIFY_A);
    
    const double minSVDRatio = 0.7;
    const double minGoodRatio = 0.85;
    //    //debug
    //    cout << "U: " << svd.u << endl;
    //    cout << "W: " << svd.w << endl;
    //    cout << "V: " << svd.vt << endl;
    
    //two singular values should be equal and the third zero
    double ratio = fabs(svd.w.ptr<float>(0)[1]/svd.w.ptr<float>(0)[0]);
    if (ratio < 0.7) {
        cerr << "singular values too far apart" << endl;
        return false;
    }
    
    Matx33f W(0,-1,0,1,0,0,0,0,1);
    Matx33f Wt(0,1,0,-1,0,0,0,0,1);
    
    Mat R0 = svd.u*Mat(W)*svd.vt;
    Mat R1 = svd.u*Mat(Wt)*svd.vt;
    Mat t0 = svd.u.col(2);
    Mat t1 = -svd.u.col(2);
    
    //calculate determinant of rotation to check validity of essential matrix
    double d = determinant(R0);
    double tol = 1e-05;
    if (d + 1.0 < tol) {
        svd(-E,SVD::MODIFY_A);
        R0 = svd.u*Mat(W)*svd.vt;
        R1 = svd.u*Mat(Wt)*svd.vt;
        t0 = svd.u.col(2);
        t1 = -svd.u.col(2);
        d = determinant(R0);
    }
    if (d -1.0 > tol) {
        cerr << "Not a proper rotation" << endl;
        return false;
    }
    
    //test all possibilities
    Matx34d P0(1,0,0,0,0,1,0,0,0,0,1,0);
    vector<Mat> rots{R0,R1};
    vector<Mat> trans{t0,t1};
    vector<Matx31d> pts3D;
    int bestCount = 0, bestRIdx, bestTIdx;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            //triangulate points
            //TODO: only triangulate a subset to limit complexity?
            pts3D.clear();
            Matx34d P(rots[i].ptr<float>(0)[0],rots[i].ptr<float>(0)[1],rots[i].ptr<float>(0)[2],trans[j].ptr<float>(0)[0],rots[i].ptr<float>(1)[0],rots[i].ptr<float>(1)[1],rots[i].ptr<float>(1)[2],trans[j].ptr<float>(0)[1],rots[i].ptr<float>(2)[0],rots[i].ptr<float>(2)[1],rots[i].ptr<float>(2)[2],trans[j].ptr<float>(0)[2]);
            triangulatePoints(P0,P,K0,K1,pts0,pts1,pts3D);
            
            //check if points are in front of the camera plane
            int countGood = 0;
            for (int k = 0; k < pts3D.size(); k++) {
                if (pts3D[k].val[2] > 1)
                    countGood++;
            }
            
            //save best transformation
            if (countGood >= bestCount) {
                bestCount = countGood;
                bestRIdx = i;
                bestTIdx = j;
            }
        }
    }
    
    if ((float)bestCount/pts3D.size() < minGoodRatio) {
        cerr << "No valid rotations/translations" << endl;
        return false;
    }
    
    R = Matx33d(rots[bestRIdx]);
    t = Vec3d(trans[bestTIdx]);
    
    return true;
}

bool GeometryUtils::RtFromHomographyMatrix(const Matx33f &H, const Matx33f &K0, const Matx33f &K1, const vector<Point2f> &pts0, const vector<Point2f> &pts1, Matx33d &R, Vec3d &t) {
    const double minGoodRatio = 0.85;
    
    //find all possible decompositions
    vector<Mat> rots;
    vector<Mat> trans;
    vector<Mat> nh;
    decomposeHomographyMat(H, K0, rots, trans, nh); //OpenCV implementation takes only one intrinsic
    
    //try triangulating
    Matx34d P0(1,0,0,0,0,1,0,0,0,0,1,0);
    vector<Matx31d> pts3D;
    int bestCount = 0, bestIdx;
    for (int i = 0; i < rots.size(); i++) {
        
        //triangulate points
        //TODO: only triangulate a subset to limit complexity?
        pts3D.clear();
        Matx34d P(rots[i].ptr<double>(0)[0],rots[i].ptr<double>(0)[1],rots[i].ptr<double>(0)[2],trans[i].ptr<double>(0)[0],rots[i].ptr<double>(1)[0],rots[i].ptr<double>(1)[1],rots[i].ptr<double>(1)[2],trans[i].ptr<double>(0)[1],rots[i].ptr<double>(2)[0],rots[i].ptr<double>(2)[1],rots[i].ptr<double>(2)[2],trans[i].ptr<double>(0)[2]);
        triangulatePoints(P0,P,K0,K1,pts0,pts1,pts3D);
        
        //check if points are in front of the camera
        int countGood = 0;
        for (int k = 0; k < pts3D.size(); k++) {
            if (pts3D[k].val[2] > 1)
                countGood++;
        }
        
        //save best transformation
        if (countGood > bestCount) {
            bestCount = countGood;
            bestIdx = i;
        }
    }
    
    if ((float)bestCount/pts3D.size() < minGoodRatio) {
        cerr << "No valid rotations/translations" << endl;
        return false;
    }
    
    R = Matx33d(rots[bestIdx]);
    t = Vec3d(trans[bestIdx]);
    
    return true;
}

double GeometryUtils::distancePointLine2D(const Point2d &pt, const Vec3d &l) {
    return (l[0]*pt.x + l[1]*pt.y + l[2])*(l[0]*pt.x + l[1]*pt.y + l[2])/(l[0]*l[0] + l[1]*l[1]);
}

double GeometryUtils::calculateFundamentalAvgError(const vector<Point2d> &pts0, const vector<Point2d> &pts1, const Matx33d &F) {
    //return average symmetric distance from epilines
    double e = 0;
    
    //compute epipolar lines
    vector<Vec3d> epiLines0, epiLines1;
    computeCorrespondEpilines(pts0, 1, F, epiLines1);
    computeCorrespondEpilines(pts1, 2, F, epiLines0);
    
    double e01,e10;
    int count = 0;
    for (int i = 0; i < pts0.size(); i++) {
        //compute distance from epilines
        e01 = GeometryUtils::distancePointLine2D(pts0[i], epiLines0[i]);
        e10 = GeometryUtils::distancePointLine2D(pts1[i], epiLines1[i]);
        e += e10 + e01;
        count++;
    }
    
    return e/count;
}

float GeometryUtils::calculateFundamentalAvgError(const vector<Point2f> &pts0, const vector<Point2f> &pts1, const Matx33f &F) {
    //return average symmetric distance from epilines
    double e = 0;
    
    //compute epipolar lines
    vector<Vec3f> epiLines0, epiLines1;
    computeCorrespondEpilines(pts0, 1, F, epiLines1);
    computeCorrespondEpilines(pts1, 2, F, epiLines0);
    
    float e01,e10;
    int count = 0;
    for (int i = 0; i < pts0.size(); i++) {
        //compute distance from epilines
        e01 = GeometryUtils::distancePointLine2D(pts0[i], epiLines0[i]);
        e10 = GeometryUtils::distancePointLine2D(pts1[i], epiLines1[i]);
        e += e10 + e01;
        count++;
    }
    
    return e/count;
}

double GeometryUtils::calculateHomographyAvgError(const vector<Point2d> &pts0, const vector<Point2d> &pts1, const Matx33d &H) {
    double e = 0;
    //average symmetric transfer error
    
    //compute matrix inverse
    Matx33d Hinv = H.inv();
    
    //get forward and backward transformed points
    vector<Point2d> fwd, bwd;
    perspectiveTransform(pts0, fwd, H);
    perspectiveTransform(pts1, bwd, Hinv);
    
    double e01, e10;
    int count = 0;
    for (int i = 0; i < pts0.size(); i++) {
        e01 = (pts1[i].x - fwd[i].x)*(pts1[i].x - fwd[i].x) + (pts1[i].y - fwd[i].y)*(pts1[i].y - fwd[i].y);
        e10 = (pts0[i].x - bwd[i].x)*(pts0[i].x - bwd[i].x) + (pts0[i].y - bwd[i].y)*(pts0[i].y - bwd[i].y);
        e += e01 + e10;
        count++;
    }
    
    return e/count;
}

float GeometryUtils::calculateHomographyAvgError(const vector<Point2f> &pts0, const vector<Point2f> &pts1, const Matx33f &H) {
    float e = 0;
    //average symmetric transfer error
    
    //compute matrix inverse
    Matx33f Hinv = H.inv();
    
    //get forward and backward transformed points
    vector<Point2f> fwd, bwd;
    perspectiveTransform(pts0, fwd, H);
    perspectiveTransform(pts1, bwd, Hinv);
    
    float e01, e10;
    int count = 0;
    for (int i = 0; i < pts0.size(); i++) {
        e01 = (pts1[i].x - fwd[i].x)*(pts1[i].x - fwd[i].x) + (pts1[i].y - fwd[i].y)*(pts1[i].y - fwd[i].y);
        e10 = (pts0[i].x - bwd[i].x)*(pts0[i].x - bwd[i].x) + (pts0[i].y - bwd[i].y)*(pts0[i].y - bwd[i].y);
        e += e01 + e10;
        count++;
    }
    
    return e/count;
}

void GeometryUtils::calculateFundamentalMatrix(const Matx33d &K0, const Matx33d &R0, const Matx31d &t0, const Matx33d &K1, const Matx33d &R1, const Matx31d &t1, Matx33d &F) {
    
    //calculate projection matrices
    Matx34d Rt0(R0(0,0), R0(0,1), R0(0,2), t0(0), R0(1,0), R0(1,1), R0(1,2), t0(1), R0(2,0), R0(2,1), R0(2,2), t0(2));
    Matx34d Rt1(R1(0,0), R1(0,1), R1(0,2), t1(0), R1(1,0), R1(1,1), R1(1,2), t1(1), R1(2,0), R1(2,1), R1(2,2), t1(2));
    Matx34d P0 = K0*Rt0;
    Matx34d P1 = K1*Rt1;
    
    //calculate epipole
    Matx31d c0 = (-R0.t())*t0;
    Matx31d e1 = P1*Matx41d(c0(0),c0(1),c0(2),1.0);
    Matx33d ex1 = getSkewSymmetric(e1);
    
    //calculate pseudoinverse
    Matx43d Ppi0;
    Mat(Mat(P0).inv(DECOMP_SVD)).copyTo(Ppi0);
    
    //calculate fundamental
    F =  ex1*P1*Ppi0;
    F *= 1.0/F(2,2);
}


Matx33d GeometryUtils::getSkewSymmetric(const Matx31d &v) {
    return Matx33d (0,-v(2),v(1),v(2),0,-v(0),-v(1),v(0),0);
    
}

int GeometryUtils::filterOutliers(const Matx34d &P, const Matx33d &K, const Size &imSize, const vector<Matx31d> &pts3D, const vector<Point2f> &pts2D, vector<uchar> &status, double threshold) {
    
    Matx34d Pmat = K*P;
    double threSq = threshold*threshold;
    int count = 0;
    for (int i = 0; i < pts3D.size(); i++) {
        //project point to 2d
        Matx31d pt = Pmat*Matx41d(pts3D[i].val[0],pts3D[i].val[1],pts3D[i].val[2],1.0);
        pt *= 1.0/pt.val[2];
        double d = (pts2D[i].x - pt.val[0])*(pts2D[i].x - pt.val[0]) + (pts2D[i].y - pt.val[1])*(pts2D[i].y - pt.val[1]);
        
        //check if point is outside the image boundaries or if distance from supposed projection is too large
        if ((pt.val[0] < 0) || (pt.val[0] >= imSize.width) || (pt.val[1] >= imSize.height) || (pt.val[1] < 0 ) || (d > threSq)) {
            status.push_back(0);
            count++;
        }
        else
            status.push_back(1);
    }
    return count;
}

int GeometryUtils::filterOutliers(const Matx34d &P, const Matx33d &K, const Size &imSize, const vector<Matx31d> &pts3D, const vector<Point2d> &pts2D, vector<uchar> &status, double threshold) {
    
    Matx34d Pmat = K*P;
    double threSq = threshold*threshold;
    int count = 0;
    for (int i = 0; i < pts3D.size(); i++) {
        //project point to 2d
        Matx31d pt = Pmat*Matx41d(pts3D[i].val[0],pts3D[i].val[1],pts3D[i].val[2],1.0);
        pt *= 1.0/pt.val[2];
        double d = (pts2D[i].x - pt.val[0])*(pts2D[i].x - pt.val[0]) + (pts2D[i].y - pt.val[1])*(pts2D[i].y - pt.val[1]);
        
        //check if point is outside the image boundaries or if distance from supposed projection is too large
        if ((pt.val[0] < 0) || (pt.val[0] >= imSize.width) || (pt.val[1] >= imSize.height) || (pt.val[1] < 0 ) || (d > threSq)) {
            status.push_back(0);
            count++;
        }
        else
            status.push_back(1);
    }
    return count;
}

int GeometryUtils::filterMatches(const Matx33d &F, const vector<Point2d> &pts0, const vector<Point2d> &pts1, vector<uchar> &status, double distThreshold) {
    
    //compute epipolar lines
    vector<Vec3d> epiLines0, epiLines1;
    computeCorrespondEpilines(pts0, 1, F, epiLines1);
    computeCorrespondEpilines(pts1, 2, F, epiLines0);
    
    //square threshold since we compute the square distance
    double sqThreshold = distThreshold*distThreshold;
    
    //check if the symmetric transfer error is too high for each point
    float e01,e10;
    int count = 0;
    for (int i = 0; i < pts0.size(); i++) {
        //compute distance from epilines
        e01 = GeometryUtils::distancePointLine2D(pts0[i], epiLines0[i]);
        e10 = GeometryUtils::distancePointLine2D(pts1[i], epiLines1[i]);
        if (0.5*(e01+e10) >= sqThreshold) {
            status.push_back(0);
            count++;
        }
        else
            status.push_back(1);
    }
    return count;
}

int GeometryUtils::filterMatches(const Matx33f &F, const vector<Point2f> &pts0, const vector<Point2f> &pts1, vector<uchar> &status, double distThreshold) {
    
    //compute epipolar lines
    vector<Vec3f> epiLines0, epiLines1;
    computeCorrespondEpilines(pts0, 1, F, epiLines1);
    computeCorrespondEpilines(pts1, 2, F, epiLines0);
    
    //square threshold since we compute the square distance
    double sqThreshold = distThreshold*distThreshold;
    
    //check if the symmetric transfer error is too high for each point
    float e01,e10;
    int count = 0;
    for (int i = 0; i < pts0.size(); i++) {
        //compute distance from epilines
        e01 = GeometryUtils::distancePointLine2D(pts0[i], epiLines0[i]);
        e10 = GeometryUtils::distancePointLine2D(pts1[i], epiLines1[i]);
        if (0.5*(e01+e10) >= sqThreshold) {
            status.push_back(0);
            count++;
        }
        else {
            status.push_back(1);
        }
    }
    return count;
}

int GeometryUtils::filterMatches(const Matx33f &F, const vector<Point2f> &pts0, const vector<Point2f> &pts1, vector<Matx31d> &pts3D, vector<uchar> &status, double distThreshold) {
    
    //compute epipolar lines
    vector<Vec3f> epiLines0, epiLines1;
    computeCorrespondEpilines(pts0, 1, F, epiLines1);
    computeCorrespondEpilines(pts1, 2, F, epiLines0);
    
    //square threshold since we compute the square distance
    double sqThreshold = distThreshold*distThreshold;
    
    //check if the symmetric transfer error is too high for each point
    float e01,e10;
    int count = 0;
    for (int i = 0; i < pts0.size(); i++) {
        //compute distance from epilines
        e01 = GeometryUtils::distancePointLine2D(pts0[i], epiLines0[i]);
        e10 = GeometryUtils::distancePointLine2D(pts1[i], epiLines1[i]);
        if ((0.5*(e01+e10) >= sqThreshold) || (pts3D[i](2) <= 1.0)) { //check also that point is in front of the camera
            status.push_back(0);
            count++;
        }
        else {
            status.push_back(1);
        }
    }
    return count;
}


int GeometryUtils::filterMatches(const Matx33d &F, const vector<Point2d> &pts0, const vector<Point2d> &pts1, vector<Matx31d> &pts3D, vector<uchar> &status, double distThreshold) {
    
    //compute epipolar lines
    vector<Vec3d> epiLines0, epiLines1;
    Matx33f Fhat = F;
    computeCorrespondEpilines(pts0, 1, F, epiLines1);
    computeCorrespondEpilines(pts1, 2, F, epiLines0);
    
    //square threshold since we compute the square distance
    double sqThreshold = distThreshold*distThreshold;
    
    //check if the symmetric transfer error is too high for each point
    float e01,e10;
    int count = 0;
    for (int i = 0; i < pts0.size(); i++) {
        //compute distance from epilines
        e01 = GeometryUtils::distancePointLine2D(pts0[i], epiLines0[i]);
        e10 = GeometryUtils::distancePointLine2D(pts1[i], epiLines1[i]);
        if ((0.5*(e01+e10) >= sqThreshold) || (pts3D[i](2) <= 1.0)) { //check also that point is in front of the camera
            status.push_back(0);
            count++;
        }
        else {
            status.push_back(1);
        }
    }
    return count;
}
