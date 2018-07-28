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
 * server.cpp
 *
 *  Created on: 2018/06/28
 *      Author: Che-Hung Lin
 */
#include <thread>
#include <cstring>
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <strings.h>
#include <netinet/in.h>

#include <server.h>

#define BUFFER_SIZE 200
#define SERVER_PORT 1024

using std::cin;
using std::cout;
using std::cerr;
using std::thread;

/******************************************************************************
 * Exception
 *****************************************************************************/
Exception::Exception(const char *msg) {
	_msg = msg;
}

Exception::~Exception() {
}

const char* Exception::get_msg() {
	return _msg.c_str();
}

/******************************************************************************
 * ClientHandler
 *****************************************************************************/
ClientHandler::ClientHandler(int client_socket, int sequence_id,
		void (*set_value)(int), int (*get_value)(void)) :
		_client_socket(client_socket), _sequence_id(sequence_id), _set_value(
				set_value), _get_value(get_value) {
}

void ClientHandler::operator()() {
	write(_client_socket, "Hello", strlen("Hello"));

	int read_size;
	char msg[BUFFER_SIZE], buffer[BUFFER_SIZE];

	// Receive a message from client
	while ((read_size = recv(_client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
		buffer[read_size] = '\0';
		cout << "Command received from client (" << _sequence_id << "): "
				<< buffer << "." << std::endl;

		// Set value
		if (strncmp(buffer, "SET VALUE ", strlen("SET VALUE ")) == 0) {
			const char *value_str = buffer + strlen("SET VALUE ");
			int val = atoi(value_str);
			cout << "Set value by client (" << _sequence_id << "): " << val
					<< "." << std::endl;
			_set_value(val);
		}
		// Get value
		else if (strncmp(buffer, "GET VALUE", strlen("GET VALUE")) == 0) {
			int val = _get_value();
			sprintf(msg, "%d", val);
			write(_client_socket, msg, strlen(msg));
			cout << "Send value to client (" << _sequence_id << "): " << msg
					<< "." << std::endl;
		}
		// Not found
		else {
			cerr << "Command not found." << std::endl;
		}

		// clear buffer
		memset(msg, 0, BUFFER_SIZE);
		memset(buffer, 0, BUFFER_SIZE);
	}

	Server::decrease_client_num();
	if (read_size == 0) {
		cout << "Client (" << _sequence_id << ") disconnected. There "
				<< (Server::get_client_num() > 1 ? "are " : "is ")
				<< Server::get_client_num() << " client"
				<< (Server::get_client_num() > 1 ? "s" : "") << " connected."
				<< std::endl;
	} else if (read_size == -1) {
		cerr << "Receive failed" << std::endl;
	}

}

/******************************************************************************
 * ServerBusyHandler
 *****************************************************************************/
ServerBusyHandler::ServerBusyHandler(int client_socket) :
		_client_socket(client_socket) {
}

void ServerBusyHandler::operator()() {
	// Handshake
	string str("Busy. Please try again later.");
	// Send busy message to client
	write(_client_socket, str.c_str(), str.size());
	close(_client_socket);
	cout << "Reject new connection request from client due to server busy.\n";
}

/******************************************************************************
 * Server
 *****************************************************************************/

int Server::_x = 0;
int Server::_client_num = 0;

Server::Server(int port) :
		_port(port), _sequence_id(0) {

	// Create socket
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd < 0) {
		throw Exception("Failed to create socket");
	}

	struct sockaddr_in server;
	bzero((char *) &server, sizeof(server));
	server.sin_family = AF_INET; // Address family AF_INET
	server.sin_addr.s_addr = INADDR_ANY; // Binds the socket to all available interfaces
	server.sin_port = htons(port); // Converts port value from host byte order to network byte order.

	// Bind
	if (bind(_sockfd, reinterpret_cast<sockaddr*>(&server), sizeof(server))
			< 0) {
		throw Exception("Bind failed");
	}
}

void Server::start() {
	int result = listen(_sockfd, 5); // Listen
	if (result < 0) {
		throw Exception("Listen failed");
	}

	int connection;
	// Accept new connection and then create new thread to handle the connection
	while ((connection = waitClientConnection())) {

		if (Server::more_client_acceptable()) {
			thread t(
					(ClientHandler(connection, ++_sequence_id, set_value,
							get_value)));
			increase_client_num();
			t.detach();
		} else {
			// Server busy
			thread t((ServerBusyHandler(connection)));
			t.detach();
		}

	}
}

int Server::waitClientConnection() {
	cout << _client_num << " client connected."
			<< " Wait new client connection...\n";

	sockaddr_in client;
	socklen_t clilen = sizeof(client);

	// Accept new connection
	int client_socket = accept(_sockfd, reinterpret_cast<sockaddr*>(&client),
			&clilen);
	if (client_socket < 0) {
		throw Exception("Accept failed");
	}
	return client_socket;
}

/******************************************************************************
 main function
 *****************************************************************************/
int main() {
	try {
		Server server(SERVER_PORT);
		server.start();
	} catch (Exception &e) {
		cerr << e.get_msg() << std::endl;
	}
	return 0;
}
