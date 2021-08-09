
#include "SimpleAnomalyDetector.h"
#include "minCircle.h"
#include <stdlib.h>

SimpleAnomalyDetector::SimpleAnomalyDetector() {}

SimpleAnomalyDetector::~SimpleAnomalyDetector() {}


//Function learn data and check if there is any correlated features,if correlation found pushes info to vector
void SimpleAnomalyDetector::learnNormal(const TimeSeries& ts) {
    int features = ts.numOfFeatures();
    int size = ts.getColumn(0).size();
    //float cor = 0.9;
    float const threshold = 1.1;
    //Check correlation between all features
    for (int i = 0; i < features; i++) {
        correlatedFeatures info;
        //Iterate comparison between different features
        for (int j = i + 1; j < features; j++) {
            Point *points[size];
            float col1[size], col2[size];
            //Insert values to arrays
            for (int k = 0; k < size; k++) {
                col1[k] = ts.getValue(k, i);
                col2[k] = ts.getValue(k, j);
                points[k] = new Point(col1[k], col2[k]);
            }
            float pear = pearson(col1, col2, size);
            info.corrlation = pear;
            info.feature1 = ts.getFeature(i);
            info.feature2 = ts.getFeature(j);
            //Check if correlation is valid
            if (abs(pear) > cor) {
                Line line1 = linear_reg(points, size);
                info.lin_reg = line1;
                float maxDev = 0;
                //Check biggest deviation
                for (int k = 0; k < size; k++) {
                    float devRes = dev(*points[k], line1);
                    //Check if max deviation
                    if (devRes > maxDev) {
                        maxDev = devRes;
                    }
                }
                info.threshold = maxDev * threshold;
                cf.push_back(info);
                //Check correlation using minCircle
            } else if(abs(pear) < cor && abs(pear) > 0.5) {
                Circle c1 = findMinCircle(points, size);
                info.radius = c1.radius;
                info.xCenter = c1.center.x;
                info.yCenter = c1.center.y;
                info.threshold = c1.radius * threshold;
                info.usedCircle = true;
                cf.push_back(info);
            }
            //Release memory
            for (int k = 0; k < size; k++) {
                delete points[k];
            }
        }
    }
}

//Function check on real time for deviation, check if point is beyond learned threshold, returns vector of reports
vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries& ts){
    vector<AnomalyReport> reports;
    int size = ts.getColumn(0).size();
    int sizeReports = this->cf.size();
    //Iterate on cf vector events
    for (int i = 0; i < sizeReports; i++) {
        //Iterate on each row, on correlated features from vector
        for (int j = 0; j < size; j++) {
            int feature1 = ts.getFeatureStr(cf[i].feature1);
            int feature2 = ts.getFeatureStr(cf[i].feature2);
            Point p1(ts.getValue(j,feature1), ts.getValue(j,feature2));
            Circle c1({cf[i].xCenter, cf[i].yCenter}, cf[i].threshold);
            bool isInside = checkInside(c1, p1);
            //Check if point is beyond threshold
            if(dev(p1, cf[i].lin_reg) > cf[i].threshold) {
                AnomalyReport rep1(cf[i].feature1+"-"+cf[i].feature2,j + 1);
                reports.push_back(rep1);
                //Check if point is outside circle
            } else if(cf[i].usedCircle && !isInside) {
                AnomalyReport rep1(cf[i].feature1+"-"+cf[i].feature2,j + 1);
                reports.push_back(rep1);
            }
        }
    }
    return reports;
}

