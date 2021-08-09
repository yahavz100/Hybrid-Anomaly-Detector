
#include "minCircle.h"
#include <cmath>
#include <vector>
#include "anomaly_detection_util.h"

using namespace std;

//Calculate distance between 2 points
float dist(Point a, Point b) {
    return sqrt(pow(a.y - b.y, 2) + pow(a.x - b.x, 2));
}

//Check if points inside given circle
bool checkInside(Circle c, Point p) {
    //Check if point inside circle radius
    if (dist(c.center, p) <= c.radius) {
        return true;
    }
    return false;
}

//Calculate circle using 2 points
Circle from2points(Point a,Point b) {
    float midX = (a.x + b.x) / 2;
    float midY = (a.y + b.y) / 2;
    Point midpoint(midX, midY);
    return Circle(midpoint, (dist(a, b)) / 2);
}

//Calculate circle using 3 points
Point getCircleCenterSub(float x1, float y1, float x2, float y2) {
    float c1 = x1 * x1 + y1 * y1;
    float c2 = x2 * x2 + y2 * y2;
    float c3 = x1 * y2 - y1 * x2;
    Point circleCenter((y2 * c1 - y1 * c2) / (2 * c3), (x1 * c2 - x2 * c1) / (2 * c3));
    return circleCenter;
}

//Calculate circle using 3 points
Circle from3Points(Point a, Point b, Point c) {
    Point center = getCircleCenterSub(b.x - a.x, b.y - a.y, c.x - a.x, c.y - a.y);
    center.x = center.x + a.x;
    center.y = center.y + a.y;
    float radius = dist(center, b);
    return Circle(center, radius);
}

//Calculate circle in cases of 0,1,2,3 points
Circle trivial(vector<Point>& setR) {
    int size = setR.size();
    //No points therefore trivial circle
    if(size == 0) {
        return Circle({0,0}, 0);
        //Only one points, also trivial circle
    } else if (size == 1) {
        return Circle({setR[0].x,setR[1].y}, 0);
        //Two points, MC center will be at the midpoint
    } else if (size == 2) {
        return from2points(setR[0], setR[1]);
        //Three points, the circle is the circumcircle of the triangle
    } else {
        Point a(setR[0].x, setR[0].y);
        Point b(setR[1].x, setR[1].y);
        Point c(setR[2].x, setR[2].y);
        return from3Points(a, b ,c);
    }
}

//Welzl algorithm
Circle welzl(Point** P, vector<Point> R, size_t n) {
    //Check if trivial circle case
    if (n == 0 || R.size() == 3) {
        return trivial(R);
    } else {
        //Choose randomly and uniformly
        Point p(P[n - 1]->x, P[n - 1]->y);
        n--;
        Circle minCircle = welzl(P,R, n);
        //Check if point belongs to circle
        if (checkInside(minCircle,p)) {
            return minCircle;
        }
        R.push_back(p);
        return welzl(P,R, n);
    }
}

//Run welzl algorithm to find minimum circle
Circle findMinCircle(Point** points, size_t size) {
    vector<Point> setR;
    Circle minCircle = welzl(points, setR, size);
    return minCircle;
}