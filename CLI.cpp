
#include "CLI.h"
#include "commands.h"
#include "HybridAnomalyDetector.h"

CLI::CLI(DefaultIO* dio) {
    this->dio = dio;
}

void CLI::start(){
    const int numOfCommands = 5;
    int userChoice = 0;
    float threshold = 0.9;
    const char *testFile = "anomalyTest.csv";
    HybridAnomalyDetector hybridDetect;
    uploadTs *uploadTimeseries = new uploadTs(dio, &hybridDetect);
    algoSettings *algorithmSettings = new algoSettings(dio, &threshold);
    hybridDetect.setCorrelationThreshold(threshold);
    detectAnom *detectAnomalies = new detectAnom(dio, &hybridDetect, testFile);
    dispRes *displayResults = new dispRes(dio);
    uploadAnalyzeRes *uploadAnalyzeResults = new uploadAnalyzeRes(dio, testFile);
    Command *arr[numOfCommands] = {uploadTimeseries, algorithmSettings, detectAnomalies, displayResults, uploadAnalyzeResults};
    //Repeat running menu untill exit
    while (true) {
        dio->write("Welcome to the Anomaly Detection Server.\n"
                   "Please choose an option:\n"
                   "1.upload a time series csv file\n"
                   "2.algorithm settings\n"
                   "3.detect anomalies\n"
                   "4.display results\n"
                   "5.upload anomalies and analyze results\n"
                   "6.exit\n");

        userChoice = stoi(dio->read()) - 1; //stoi(dio->read()) - 1;
        //Check if user want to exit
        if (userChoice == 5) {
            break;
        }
        arr[userChoice]->execute();
    }
}


CLI::~CLI() {
}

