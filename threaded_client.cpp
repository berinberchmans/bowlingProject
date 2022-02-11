#include "threaded_client.h"
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
void Client::start(const char* ipaddress, const char* port) {
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL,DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return;
	}

	printf("Connected!\n");
}

void Client::sendthis(const char* buffer) {
	iResult = send(ConnectSocket, buffer, sizeof(buffer), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		terminate();
		return;
	}
}

const char* Client::getthis() {
	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) return recvbuf;
}

void Client::sendmsg() {
	while (true) {
		printf("Client: ");
		fgets(recvbuf, 255, stdin);
		iResult = send(ConnectSocket, recvbuf, sizeof(recvbuf), 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			terminate();
			return;
		}
	}
}

void Client::getmsg() {
	while (true) {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("Server says: %s\n", recvbuf);
	}
}

void Client::interact() {
	std::thread t(&Client::getmsg, this);
	//sendmsg();
	t.detach();
}

void Client::terminate() {
	closesocket(ConnectSocket);
	WSACleanup();
}

/*int main(int argc, const char* argv[]) {
	if (argc < 3) {
		printf("Usage: %s hotname port\n", argv[0]);

	}
	else {
		Client client;
		client.start(argv[1], argv[2]);
		client.interact();
	}

	return 0;
}*/