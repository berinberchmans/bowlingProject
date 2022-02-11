#include "threaded_server.h"

#define DEFAULT_BUFLEN 512
#define MAX_NUM_CLIENTS 2

#define DEFAULT_PORT "27015"

namespace info {
	std::mutex mtx;
	SOCKET ListenSocket;
	SOCKET ClientSocket;
}





void Server::connect() {
	

	info::ListenSocket = INVALID_SOCKET;
	info::ClientSocket = INVALID_SOCKET;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	std::string s = std::to_string(portnum);
	const char* portstr = s.c_str();
	iResult = getaddrinfo(NULL, portstr, &hints, &result);
	//iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}


	// Create a SOCKET for connecting to server
	info::ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (info::ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	// Setup the TCP listening socket
	iResult = bind(info::ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(info::ListenSocket);
		WSACleanup();
		return;
	}

	freeaddrinfo(result);

	iResult = listen(info::ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(info::ListenSocket);
		WSACleanup();
		return;
	}

	// Accept a client socket
	info::ClientSocket = accept(info::ListenSocket, NULL, NULL);
	if (info::ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(info::ListenSocket);
		WSACleanup();
		return;
	}
	printf("getting here asdasd");
}

void Server::interact() {
	while (true) {
		printf("listening\n");
		iResult = recv(info::ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Client %d says: %s\n", portnum, recvbuf);

			info::mtx.lock();
			
			printf("broadcasting\n");
		
				//iSendResult = send(info::ClientSocket[i], reply.c_str(), iResult, 0);
				iSendResult = send(info::ClientSocket, recvbuf, iResult, 0);
				if (iSendResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					terminate();
					return;
				}

			info::mtx.unlock();
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			terminate();
			return;
		}
	}
}

void Server::terminate() {
	// shutdown the connection since we're done
	iResult = shutdown(info::ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(info::ClientSocket);
		WSACleanup();
		return;
	}

	// cleanup
	closesocket(info::ClientSocket);
	WSACleanup();
}

