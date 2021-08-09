cmake_minimum_required(VERSION 3.17)
project(Hybrid_Anomaly_Detector)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(MainTrain
        anomaly_detection_util.cpp
        anomaly_detection_util.h
        AnomalyDetector.h
        HybridAnomalyDetector.cpp
        HybridAnomalyDetector.h
        MainTrain.cpp
        minCircle.h
        SimpleAnomalyDetector.cpp
        SimpleAnomalyDetector.h
        timeseries.cpp
        timeseries.h minCircle.cpp
        CLI.cpp CLI.h
        Server.h Server.cpp
        commands.h)

set(GCC_COVERAGE_COMPILE_FLAGS "-pthread -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
