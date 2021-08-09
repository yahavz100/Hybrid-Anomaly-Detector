

#ifndef TIMESERIES_H_
#define TIMESERIES_H_

#include <string>
#include <vector>

using namespace std;

class TimeSeries{
private:
    const char *fileName;
    vector<vector<string>> data;
    vector<string> features;

public:
    TimeSeries(const char* CSVfileName);        
    TimeSeries();
    void initialize(const char *CSVfileName);
    vector<float> getRow(int num) const;
    float getValue(int i, int j) const;
    void setRow(vector<float> row, int num);
    void setValue(float value, int i, int j);
    vector<float> getColumn(int num) const;
    void setColumn(vector<float> column, int num);
    int numOfFeatures() const;
    string getFeature(int num) const;
    int getFeatureStr(string feature) const;
};



#endif /* TIMESERIES_H_ */
