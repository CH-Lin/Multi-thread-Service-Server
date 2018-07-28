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
 * client.cpp
 *
 *  Created on: 2018/06/28
 *      Author: Che-Hung Lin
 */

#include <cstring>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>

#define BUFFER_SIZE 200
#define SERVER_PORT 1024

using std::cin;
using std::cout;
using std::cerr;

/*------------------------------------------------------------------
 main function
 ------------------------------------------------------------------*/
int main() {
	int connection;

	if ((connection = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		cerr << "Create socket failed\n";
		return -1;
	}

	// Destination information
	struct sockaddr_in destination;
	bzero((char *) &destination, sizeof(destination));
	destination.sin_addr.s_addr = htonl(INADDR_ANY);
	destination.sin_port = htons(SERVER_PORT);
	destination.sin_family = AF_INET;

	char send_buffer[BUFFER_SIZE], received_buffer[BUFFER_SIZE];
	// Connect to server
	if (connect(connection, (struct sockaddr*) &destination,
			sizeof(destination)) >= 0) {

		// Handshake
		bzero((char *) &received_buffer, BUFFER_SIZE);
		int read_size = read(connection, received_buffer, BUFFER_SIZE);
		received_buffer[read_size] = '\0';

		// Check the content of handshake
		int start = 0;
		if (strcmp(received_buffer, "Hello") == 0) {
			start = 1;
		}

		int val;
		while (start) {
			bzero((char *) &send_buffer, BUFFER_SIZE);
			bzero((char *) &received_buffer, BUFFER_SIZE);

			// Get user input
			cout << "Please select an action:"
					" (0 to SET VALUE; 1 to GET VALUE)\n";
			cin >> val;

			switch (val) {
			case 0:
				cout << "Please enter an integer value:";
				cin >> val;
				sprintf(send_buffer, "SET VALUE %d", val);
				write(connection, send_buffer, strlen(send_buffer));
				break;
			case 1:
				sprintf(send_buffer, "GET VALUE");
				write(connection, send_buffer, strlen(send_buffer));
				read_size = read(connection, received_buffer, BUFFER_SIZE);
				received_buffer[read_size] = '\0';
				cout << "Received from Server: " << received_buffer
						<< std::endl;
				break;
			}
		}

		close(connection); // Close connection
	} else {
		cerr << "Create connection failed\n";
		return 1;
	}
	return 0;
}
