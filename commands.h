

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include <string.h>

#include <fstream>
#include <vector>
#include <sstream>
#include "HybridAnomalyDetector.h"

using namespace std;

class DefaultIO{
public:
	virtual string read()=0;
	virtual void write(string text)=0;
	virtual void write(float f)=0;
	virtual void read(float* f)=0;
	virtual ~DefaultIO(){}

};

class Command{
protected:
	DefaultIO* dio;
public:
    string description;
	Command(DefaultIO* dio):dio(dio){}
	virtual void execute()=0;
	virtual ~Command(){}

};

class uploadTs : public Command {
private:
    string description;
    HybridAnomalyDetector* hybridDetect;
public:
    uploadTs(DefaultIO *dio, HybridAnomalyDetector* hybridDetect): Command(dio){
        this->description = "1.upload a time series csv file\n";
        this->hybridDetect = hybridDetect;
    }
    //Read input file
    void readFile(DefaultIO *dio, const string fileName) {
        ofstream input(fileName);
        string buffer;
        //Run until done
        while(buffer != "done") {
            buffer = dio->read();
            //If different done put buffer into input file
            if(buffer != "done") {
                input << buffer + "\n";
            }
        }
        input.close();
    }
    void execute() override {
        dio->write("Please upload your local train CSV file.\n");
        readFile(dio, "anomalyTrain.csv");
        dio->write("Upload complete.\n");
        dio->write("Please upload your local test CSV file.\n");
        readFile(dio, "anomalyTest.csv");
        dio->write("Upload complete.\n");
    }
};

class algoSettings : public Command {
private:
    string description;
    float* threshold;
public:
    algoSettings(DefaultIO *dio, float* threshold) : Command(dio) {
        this->description = "2.algorithm settings";
        this->threshold = threshold;
    }
    void execute() override {
        float newThreshold;
        int flag = 0;
        dio->write("The current correlation threshold is ");
        dio->write(*threshold);
        dio->write("\nType a new threshold\n");
        while(flag == 0) {
            newThreshold = stof(dio->read());
            if (newThreshold < 1 && newThreshold > 0) {
                flag = 1;
                *threshold = newThreshold;
                continue;
            }
            dio->write("please choose a value between 0 and 1\n");
        }
    }
};

class detectAnom : public Command {
private:
    string description;
    HybridAnomalyDetector* hybridDetect;
    const char *testFile;
public:
    detectAnom(DefaultIO *dio, HybridAnomalyDetector *hybridDetect, const char *&testFile) : Command(dio){
        this->description = "3. detect anomalies\n";
        this->hybridDetect = hybridDetect;
        this->testFile = testFile;
    }
    void execute() override {
        this->hybridDetect->learnNormal(TimeSeries("anomalyTrain.csv"));
        vector<AnomalyReport> reports = this->hybridDetect->detect(TimeSeries(this->testFile));
        ofstream input;
        input.open("anomalyReports.txt");
        //For each report write in file timestep and description
        for(int i = 0; i < reports.size(); i++) {
            input << reports[i].timeStep << " \t" << reports[i].description << endl;
        }
        input.close();
        dio->write("anomaly detection complete.\n");
    }
};

class dispRes : public Command {
private:
    string description = "4. display results";
public:
    dispRes(DefaultIO *dio) : Command(dio){}
    void execute() override {
        ifstream readFile("anomalyReports.txt");
        string line;
        //Copy anomalies detected from written file
        while (getline(readFile, line)) {
            dio->write(line);
            dio->write("\n");
        }
        readFile.close();
        dio->write("Done.\n");
    }
};

class uploadAnalyzeRes : public Command {
private:
    string description = "5. upload anomalies and analyze results";
    const char *testFile;
public:
    uploadAnalyzeRes(DefaultIO *dio, const char *&testFile) : Command(dio){
        this->testFile = testFile;
    }

    vector<pair <int, int>> readUserAnomFile() {
        string line, anom1, anom2;
        vector<pair <int, int>> userReports;
        //Iterate over user anomalies and add each range to vector
        while (line != "done") {
            line = dio->read();
            if (line != "done") {
                stringstream lineStream(line);
                getline(lineStream, anom1, ',');
                getline(lineStream, anom2);
                userReports.emplace_back(stoi(anom1), stoi(anom2));
            }
        }
        return userReports;
    }

    vector<pair <int, int>> findRange(vector<int> rangeHelp) {
        int length = 1;
        vector<pair<int, int>> anomReports;
        //Check if vector is empty return the vector
        if (rangeHelp.size() == 0) {
            return anomReports;
        }
        //Move over whole vector find continuity
        for (int i = 1; i <= rangeHelp.size(); i++) {
            //Check difference between two elements
            if (i == rangeHelp.size() || rangeHelp[i] - rangeHelp[i - 1] != 1) {
                //If contains 1 element add to vector
                if (length == 1) {
                    anomReports.emplace_back(rangeHelp[i - length], rangeHelp[i - length]);
                } else {
                    //Else push the range of previous element
                    anomReports.emplace_back(rangeHelp[i - length], rangeHelp[i - 1]);
                }
                length = 1;
            } else {
                length++;
            }
        }
        return anomReports;
    }

    vector<pair <int, int>> readAnomFile() {
        ifstream inputFile("anomalyReports.txt");
        vector<int> rangeHelp;
        vector<pair <int, int>> anomReports;
        string line, num;
        while (getline(inputFile, line)) {
            int pos = line.find('\t');
            num = line.substr(0, pos);
            rangeHelp.push_back(stoi(num));
        }
        anomReports = findRange(rangeHelp);
        inputFile.close();
        return anomReports;
    }

    static int countRows(const char *nameFile) {
        int counter = 0;
        ifstream input(nameFile);
        string line;
        while(getline(input, line)) {
            counter++;
        }
        counter--;
        input.close();
        return counter;
    }

    void execute() override {
        dio->write("Please upload your local anomalies file.\n");
        float numReports = 0;
        vector<pair <int, int>> userReports = readUserAnomFile();
        vector<pair <int, int>> anomReports = readAnomFile();
        dio->write("Upload complete.\n");
        float truePositive = 0;
        int n = countRows(this->testFile);
        for(pair <int, int> userRange : userReports) {
            n = n - (userRange.second - userRange.first + 1);
            for(pair <int, int> anomRange : anomReports) {
                //Check each case, if ranges similar, different, cup, cap
                if(anomRange.first < userRange.first && anomRange.second < userRange.second && anomRange.second >= userRange.first) {
                    truePositive++;
                } else if(anomRange.first > userRange.first && anomRange.second > userRange.second && anomRange.first <= userRange.second) {
                    truePositive++;
                } else if(anomRange.first <= userRange.first && anomRange.second >= userRange.second) {
                    truePositive++;
                } else if(anomRange.first >= userRange.first && anomRange.second <= userRange.second) {
                    truePositive++;
                }
            }
        }
        float num = 1000;
        float falsePositive = anomReports.size() - truePositive;
        float truePositiveRate = truePositive / userReports.size();
        truePositiveRate = truePositiveRate * num;
        truePositiveRate = floor(truePositiveRate) / num;
        float falsePositiveRate = (falsePositive / (float)n);
        falsePositiveRate = falsePositiveRate * num;
        falsePositiveRate = floor(falsePositiveRate) / num;
        dio->write("True Positive Rate: ");
        dio->write(truePositiveRate);
        dio->write("\nFalse Positive Rate: ");
        dio->write(falsePositiveRate);
        dio->write("\n");
    }
};

#endif /* COMMANDS_H_ */
