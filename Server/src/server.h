/**
Copyright (c) 2018 Che-Hung Lin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

/*
 * server.h
 *
 *  Created on: 2018/06/28
 *      Author: Che-Hung Lin
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <mutex>
#include <string>

using std::mutex;
using std::string;

#define MAX_CLIENT_NUM 100

class Exception {
private:
	string _msg;
public:
	Exception(const char *msg);
	virtual ~Exception();

	const char* get_msg();
};

class ClientHandler {
private:
	int _client_socket;
	int _sequence_id;
	void (*_set_value)(int);
	int (*_get_value)(void);
public:
	ClientHandler(int client_socket, int sequence_id, void (*set_value)(int),
			int (*get_value)(void));
	void operator()();
};

class ServerBusyHandler {
private:
	int _client_socket;
public:
	ServerBusyHandler(int client_socket);
	void operator()();
};

std::mutex mtx;

class Server {
private:
	int _port;
	int _sockfd;
	int _sequence_id;
	static int _x;
	static int _client_num;
public:
	Server(int port);
	void start();
	static void increase_client_num() {
		mtx.lock();
		Server::_client_num++;
		mtx.unlock();
	}
	static void decrease_client_num() {
		mtx.lock();
		Server::_client_num--;
		mtx.unlock();
	}
	static int get_client_num() {
		mtx.lock();
		return Server::_client_num;
		mtx.unlock();
	}

private:
	int waitClientConnection();
	static int more_client_acceptable() {
		mtx.lock();
		int ret = Server::_client_num < MAX_CLIENT_NUM;
		mtx.unlock();
		return ret;
	}

	static void set_value(int x) {
		std::lock_guard<std::mutex> lock(mtx);
		_x = x;
	}

	static int get_value() {
		std::lock_guard<std::mutex> lock(mtx);
		return _x;
	}
};

#endif /* SERVER_H_ */
