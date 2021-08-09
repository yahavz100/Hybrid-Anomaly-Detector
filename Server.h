
/*
 * Server.h
 *
 *  Created on: Dec 13, 2020
 *      Author: Eli
 *		Edited: Yahav Zarfati
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "unistd.h"
#include <thread>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include "CLI.h"

using namespace std;


class ClientHandler{
    public:
    virtual void handle(int clientID)=0;
};



class SocketIO: public DefaultIO {
    int sd;
public:
    SocketIO(int sd) {
        this->sd = sd;
    }

    string read() override {
        string inputServer = string();
        char input;
        ::read(this->sd, &input, sizeof(input));
        while (input != '\n') {
            inputServer += input;
            ::read(this->sd, &input, sizeof(input));
        }
        return inputServer;
    }

    void write(string text) override {
        const char *conv = text.c_str();
        send(this->sd, conv, strlen(conv), 0);
    }

    void write(float f) override {
        stringstream str;
        str << f;
        send(this->sd, str.str().c_str(), str.str().length(), 0);
    }
    void read(float* f) override {
        *f = stof(read());
    }
};


class AnomalyDetectionHandler:public ClientHandler{
	public:
    virtual void handle(int clientID){
        SocketIO socketIo(clientID);
        CLI cli(&socketIo);
        cli.start();
    }
};


// implement on Server.cpp
class Server {
    int fd;
    sockaddr_in server;
    sockaddr_in client;
	thread* t; // the thread to run the start() method in
	bool stop_thread;


public:
	Server(int port) throw (const char*);
	virtual ~Server();
	void start(ClientHandler& ch)throw(const char*);
	void stop();
};

#endif /* SERVER_H_ */
