#include "timeseries.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

//Constructor given input file name
TimeSeries::TimeSeries(const char *CSVfileName) {
    fileName = CSVfileName;
    initialize(fileName);
}

//Constructor
TimeSeries::TimeSeries() {
}

//Initialize data, load CSV file into vector of vectors of floats, vector of string for features
void TimeSeries::initialize(const char *CSVfileName) {
    fstream file;
    file.open(CSVfileName, ios::in);
    vector<string> row;
    string line, word;
    int flag = 1;
    //Run on file untill the end, seperate each line by new line
    while (file.good()) {
        row.clear();
        getline(file, line, '\n');
        stringstream lineStream(line);
        //Iterate over row, seperate each word by comma
        while (getline(lineStream, word, ',')) {
            //Push only feaures row to a seperate vector
            if(flag) {
                features.push_back(word);
            } else {
                row.push_back(word);
            }
        }
        //Check if flag is on
        if(flag) {
            flag = 0;
            continue;
        }
        //Avoid pushing empty vectors
        if(line == "") {
            continue;
        }
        this->data.push_back(row);
    }
}

//Returns a row(vector<float>) from data
vector<float> TimeSeries::getRow(int num) const {
    vector<float> row;
    int size = this->data[0].size();
    //Iterate over given row and convert from string to float
    for(int i = 0; i < size; i++) {
        row.push_back(stof(this->data[num][i]));
    }
    return row;
}

//Returns a specific value from data
float TimeSeries::getValue(int i, int j) const {
    return stof(this->data[i][j]);
}

//Change specific row in data
void TimeSeries::setRow(vector<float> row, int num) {
    vector<string> stRow;
    int size = this->data[0].size();
    //Iterate over given row and convert from float to string
    for(int i = 0; i < size; i++) {
        stRow.push_back(to_string(row[i]));
    }
    this->data[num] = stRow;
}

//Change specific value in data
void TimeSeries::setValue(float value, int i, int j) {
    this->data[i][j] = to_string(value);
}

//Returns a column(vector<float>) from data
vector<float> TimeSeries::getColumn(int num) const {
    std::vector<float> column;
    int size = this->data.size();
    //Iterate over row vectors, push elements in [num] position to column vector and convert
    for(int i = 0; i < size; i++) {
        column.push_back(stof(this->data[i][num]));
    }
    return column;
}

//Change specific column in data
void TimeSeries::setColumn(vector<float> column, int num) {
    int size;
    //Check for valid size
    if (column.size() > this->data.size()) {
        size = this->data.size();
    } else {
        size = column.size();
    }
    //Iterate over row vectors, replace data column vector elements to given column vector and convert
    for(int i = 0; i < size; i++) {
        this->data[i][num] = to_string(column[i]);
    }
}

//Returns number of features in data
int TimeSeries::numOfFeatures() const {
    return this->features.size();
}

//Returns a feature(string)
string TimeSeries::getFeature(int num) const {
    return this->features[num];
}

//Returns feature column by given feature name
int TimeSeries::getFeatureStr(string feature) const {
    int size = this->features.size();
    int const notFound = -1;
    //Iterate on features
    for (int i = 0; i < size; i++) {
        //Check if given feature same to feature in data
        if(feature.compare(this->features[i]) == 0) {
            return i;
        }
    }
    return notFound;
}


