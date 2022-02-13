// Pool Game.cpp : Defines the entry point for the console application.
//



// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#undef UNICODE

#define WIN32_LEAN_AND_MEAN


#include <winsock2.h>
#include <ws2tcpip.h>


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"


#include<stdio.h>
#include <string.h>
#include <iostream> 
#include <stdlib.h>   
#include <time.h> 
#include<windows.h>
#include <sstream>

#include <thread>         // std::this_thread::sleep_for
#include <chrono>     

#include "stdafx.h"
#include "stdafx.h"
#include<glut.h>
#include<math.h>
#include"simulation.h"
#include <mutex>

//#include "threaded_client.h"
//#include "threaded_server.h"

/*namespace net {
	Client client;
	Server server;
}*/
namespace info {

	SOCKET ListenSocketo;
	SOCKET ClientSocketo;
	std::mutex mtxo;
}
namespace info2 {
	SOCKET ListenSocketi;
	SOCKET ClientSocketi;
}



int Gameturn = 0;
int maxGameTurn = 10;
bool nonRepeat = false;
bool gameOver = false;
bool canhit = true;
bool welcomeOn = true;
bool playOffline = true;
bool chosenMode = false;
bool startgame = false;
bool startMultiplayer = false;
bool chooseRoleDone = false;
bool collectionMode = true;
int pickedKey = 0;
float gCueAnglePass = 0.0;
float gCuePowerPass = 0.0;
int identity = 0;

bool canhitM = true;


bool sendMessage = false;




//cue variables
float gCueAngle = 0.0;
float gCuePower = 0.25;
bool gCueControl[4] = {false,false,false,false};
float gCueAngleSpeed = 2.0f; //radians per second
float gCuePowerSpeed = 0.25f;
float gCuePowerMax = 0.75;
float gCuePowerMin = 0.1;
float gCueBallFactor = 8.0;
bool gDoCue = true;



//camera variables
vec3 gCamPos(0.0,0.7,2.1);
vec3 gCamLookAt(0.0,0.0,0.0);
bool gCamRotate = true;
float gCamRotSpeed = 0.2;
float gCamMoveSpeed = 0.5;
bool gCamL = false;
bool gCamR = false;
bool gCamU = false;
bool gCamD = false;
bool gCamZin = false;
bool gCamZout = false;

//rendering options
#define DRAW_SOLID	(0)

void DoCamera(int ms)
{
	static const vec3 up(0.0,1.0,0.0);

	if(gCamRotate)
	{
		if(gCamL)
		{
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localL = up.Cross(camDir);
			vec3 inc = (localL* ((gCamRotSpeed*ms)/1000.0) );
			gCamLookAt = gCamPos + camDir + inc;
		}
		if(gCamR)
		{
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = up.Cross(camDir);
			vec3 inc = (localR* ((gCamRotSpeed*ms)/1000.0) );
			gCamLookAt = gCamPos + camDir - inc;
		}
		if(gCamU)
		{
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = camDir.Cross(up);
			vec3 localUp = localR.Cross(camDir);
			vec3 inc = (localUp* ((gCamMoveSpeed*ms)/1000.0) );
			gCamLookAt = gCamPos + camDir + inc;
		}
		if(gCamD)
		{
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = camDir.Cross(up);
			vec3 localUp = localR.Cross(camDir);
			vec3 inc = (localUp* ((gCamMoveSpeed*ms)/1000.0) );
			gCamLookAt = gCamPos + camDir - inc;
		}		
	}
	else
	{
		if(gCamL)
		{
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localL = up.Cross(camDir);
			vec3 inc = (localL* ((gCamMoveSpeed*ms)/1000.0) );
			gCamPos += inc;
			gCamLookAt += inc;
		}
		if(gCamR)
		{
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = camDir.Cross(up);
			vec3 inc = (localR* ((gCamMoveSpeed*ms)/1000.0) );
			gCamPos += inc;
			gCamLookAt += inc;
		}
		if(gCamU)
		{
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = camDir.Cross(up);
			vec3 localUp = localR.Cross(camDir);
			vec3 inc = (localUp* ((gCamMoveSpeed*ms)/1000.0) );
			gCamPos += inc;
			gCamLookAt += inc;
		}
		if(gCamD)
		{
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = camDir.Cross(up);
			vec3 localDown = camDir.Cross(localR);
			vec3 inc = (localDown* ((gCamMoveSpeed*ms)/1000.0) );
			gCamPos += inc;
			gCamLookAt += inc;
		}
	}

	if(gCamZin)
	{
		vec3 camDir = (gCamLookAt - gCamPos).Normalised();
		vec3 inc = (camDir* ((gCamMoveSpeed*ms)/1000.0) );
		gCamPos += inc;
		gCamLookAt += inc;
	}
	if(gCamZout)
	{
		vec3 camDir = (gCamLookAt - gCamPos).Normalised();
		vec3 inc = (camDir* ((gCamMoveSpeed*ms)/1000.0) );
		gCamPos -= inc;
		gCamLookAt -= inc;
	}
}

void RenderWelcome(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set camera
	glLoadIdentity();
	gluLookAt(gCamPos(0), gCamPos(1), gCamPos(2), gCamLookAt(0), gCamLookAt(1), gCamLookAt(2), 0.0f, 1.0f, 0.0f);
	//draw the ball
	glColor3f(0.3, 0.3, 0.3);
	glColor3f(1.0, 1.0, 1.0);
	//draw text
	char  da[] = "Welcome to the pool game";
	glRasterPos2f(-0.3f, 0.71f);
	glColor3f(1., 0., 0.);
	int len = strlen(da);
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, da[i]);
	}

	char  c01[] = "Press Enter to choose an option";
	char  c02[] = "Play offline";
	char  c03[] = "Play Multiplayer LAN";
	char  c04[] = "->";
	glRasterPos2f(-0.3f, 0.51f);
	glColor3f(1., 0., 0.);
	int lenca = strlen(c01);
	for (int i = 0; i < lenca; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c01[i]);
	}
	glRasterPos2f(-0.3f, 0.41f);
	glColor3f(1., 0., 0.);
	int lencb = strlen(c02);
	for (int i = 0; i < lencb; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c02[i]);
	}
	glRasterPos2f(-0.3f, 0.31f);
	glColor3f(1., 0., 0.);
	int lencc = strlen(c03);
	for (int i = 0; i < lencc; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c03[i]);
	}
	if (playOffline == true) {
		glRasterPos2f(-0.38f, 0.41f);
	}
	else {
		glRasterPos2f(-0.38f, 0.31f);
	}
	glColor3f(1., 0., 0.);
	int lensel = strlen(c04);
	for (int i = 0; i < lensel; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c04[i]);
	}

	glFlush();
	glutSwapBuffers();

}

void connectServer() {
	bool dotheaction = false;
	while (dotheaction == false) {
		if (chooseRoleDone == true && collectionMode == true) {
			dotheaction = true;

			WSADATA wsaData;
			int iResult;

			info::ListenSocketo = INVALID_SOCKET;
			info::ClientSocketo = INVALID_SOCKET;

			struct addrinfo* result = NULL;
			struct addrinfo hints;

			int iSendResult;
			char recvbuf[DEFAULT_BUFLEN];
			int recvbuflen = DEFAULT_BUFLEN;

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
			iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
			if (iResult != 0) {
				printf("getaddrinfo failed with error: %d\n", iResult);

				WSACleanup();
				return;
			}



			// Create a SOCKET for connecting to server
			info::ListenSocketo = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
			if (info::ListenSocketo == INVALID_SOCKET) {
				printf("socket failed with error: %ld\n", WSAGetLastError());
				freeaddrinfo(result);
				WSACleanup();
				return;
			}

			// Setup the TCP listening socket
			iResult = bind(info::ListenSocketo, result->ai_addr, (int)result->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				printf("bind failed with error: %d\n", WSAGetLastError());
				freeaddrinfo(result);
				closesocket(info::ListenSocketo);
				WSACleanup();
				return;
			}



			freeaddrinfo(result);

			iResult = listen(info::ListenSocketo, SOMAXCONN);
			if (iResult == SOCKET_ERROR) {
				printf("listen failed with error: %d\n", WSAGetLastError());
				closesocket(info::ListenSocketo);
				WSACleanup();
				return;
			}



			// Accept a client socket
			info::ClientSocketo = accept(info::ListenSocketo, NULL, NULL);
			if (info::ClientSocketo == INVALID_SOCKET) {
				printf("accept failed with error: %d\n", WSAGetLastError());
				closesocket(info::ListenSocketo);
				WSACleanup();
				return;
			}

			printf("getting here asdasd");
			while (true) {
				printf("listening\n");
				iResult = recv(info::ClientSocketo, recvbuf, recvbuflen, 0);
				if (iResult > 0) {

					//info::mtxo.lock();
					printf("broadcasting\n");


					// finding the gCueAngle and gCuePower
					std::string s = recvbuf;
					std::string delimiter = ":";
					size_t pos = 0;
					std::string pangle;
					std::string ppower;
					while ((pos = s.find(delimiter)) != std::string::npos) {
						pangle = s.substr(0, pos);

						s.erase(0, pos + delimiter.length());
					}
					ppower = s;
					double qangle = ::atof(pangle.c_str());
					double qpower = ::atof(ppower.c_str());
					gCueAngle = qangle;
					gCuePower = qpower;
					vec2 imp((-sin(gCueAngle) * gCuePower * gCueBallFactor),
						(-cos(gCueAngle) * gCuePower * gCueBallFactor));
					std::cout << " --- " << canhitM << "------" << qangle << " --- " << qpower << std::endl;
					if (strcmp(recvbuf, "0:0") != 0) {
						std::cout << "server says" << recvbuf << "------" << qangle << " --- " << qpower << " --- " << canhitM << std::endl;
						if (canhitM == true) {
							std::cout << "Enter inside" << std::endl;
							gTable.balls[0].ApplyImpulse(imp);
							gCueAngle = 0.0;
							gCuePower = 0.0;
							gCueAnglePass = 0.0;
							gCuePowerPass = 0.0;
							canhitM = false;
							nonRepeat = true;
						}
					}




					//info::mtxo.unlock();
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
	}
	
}

void startClient(const char* ipaddress, const char* port) {
	bool dotheaction = false;
	while (dotheaction == false) {
		if (chosenMode == true) {
			dotheaction = true;
			std::cout << "xCLIENTx";

			WSADATA wsaData;
			info2::ClientSocketi = INVALID_SOCKET;
			struct addrinfo* result = NULL, * ptr = NULL, hints;
			auto* sendbuf = "this is a test";
			char recvbuf[DEFAULT_BUFLEN];
			int iResult;
			int iSendResult;
			int recvbuflen = DEFAULT_BUFLEN;
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
			std::cout << ipaddress << "  ----  " << port;
			iResult = getaddrinfo(ipaddress, port, &hints, &result);
			if (iResult != 0) {
				printf("getaddrinfo failed with error: %d\n", iResult);
				WSACleanup();
				return;
			}

			// Attempt to connect to an address until one succeeds
			for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

				// Create a SOCKET for connecting to server
				info2::ClientSocketi = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
				if (info2::ClientSocketi == INVALID_SOCKET) {
					printf("socket failed with error: %ld\n", WSAGetLastError());
					WSACleanup();
					return;
				}

				// Connect to server.
				iResult = connect(info2::ClientSocketi, ptr->ai_addr, (int)ptr->ai_addrlen);
				if (iResult == SOCKET_ERROR) {
					closesocket(info2::ClientSocketi);
					info2::ClientSocketi = INVALID_SOCKET;
					continue;
				}
				break;
			}

			freeaddrinfo(result);

			if (info2::ClientSocketi == INVALID_SOCKET) {
				printf("Unable to connect to server!\n");
				WSACleanup();
				return;
			}
			else {
				printf("Connected to seerverrr");
				chooseRoleDone = true;
			}
			printf("listening\n");
			while (true) {
				iResult = recv(info2::ClientSocketi, recvbuf, recvbuflen, 0);
				if (iResult > 0) {

					// finding the gCueAngle and gCuePower
					std::string s = recvbuf;
					std::string delimiter = ":";
					size_t pos = 0;
					std::string pangle;
					std::string ppower;
					while ((pos = s.find(delimiter)) != std::string::npos) {
						pangle = s.substr(0, pos);

						s.erase(0, pos + delimiter.length());
					}
					ppower = s;
					double qangle = ::atof(pangle.c_str());
					double qpower = ::atof(ppower.c_str());
					gCueAngle = qangle;
					gCuePower = qpower;
					vec2 imp((-sin(gCueAngle) * gCuePower * gCueBallFactor),
						(-cos(gCueAngle) * gCuePower * gCueBallFactor));
					std::cout << "------" << qangle << " --- " << qpower << std::endl;
					if (strcmp(recvbuf, "0:0") != 0) {
						std::cout << "server says" << recvbuf << "------" << qangle << " --- " << qpower << " --- " << canhitM << std::endl;
						if (canhitM == true) {
							std::cout << "Enter inside" << std::endl;
							gTable.balls[0].ApplyImpulse(imp);
							gCueAngle = 0.0;
							gCuePower = 0.0;
							gCueAnglePass = 0.0;
							gCuePowerPass = 0.0;
							canhitM = false;
							nonRepeat = true;
						}


					}

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
	}
}

void sendServerCommand(float gCueAnglePassval, float gCuePowerPassval) {

	
	auto* sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	//std::cout << " Client what happened";
	char msgc[] = "0.0";
	char powergc[] = "0.0";
	char pssval[] = "0.0";

	// creating stringstream objects
	std::stringstream ss1;
	std::stringstream ss2;

	// assigning the value of num_float to ss1
	ss1 << gCueAnglePass;
	ss2 << gCuePowerPass;

	std::string str1 = ss1.str();
	std::string str2 = ss2.str();
	std::string p1 = str1;
	std::string p2 = str2;
	std::string p3 = p1 + ":" + p2;
	
		std::cout << "Client Connected!- sending" << std::endl;
	
			std::cout << "SENDING  - IMPLULSEEE" << std::endl;
			iResult = send(info2::ClientSocketi, p3.c_str(), sizeof(recvbuf), 0);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(info2::ClientSocketi);
				WSACleanup();
				return;
			}
			
	


}

void sendCommand(float gCueAnglePassval,float gCuePowerPassval) {
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult;
	char msgc[] = "0.0";
	char powergc[] = "0.0";
	char pssval[] = "0.0";


	// creating stringstream objects
	std::stringstream ss1;
	std::stringstream ss2;

	// assigning the value of num_float to ss1
	ss1 << gCueAnglePassval;

	// assigning the value of num_float to ss2
	ss2 << gCuePowerPassval;

	// initializing two string variables with the values of ss1 and ss2
	// and converting it to string format with str() function
	std::string str1 = ss1.str();
	std::string str2 = ss2.str();

	//itoa(gCueAnglePass, msgc, 10);
	//itoa(gCuePowerPass, powergc, 10);
	std::string p1 = str1;
	std::string p2 = str2;
	std::string p3 = p1 + ":" + p2;
	std::cout << "Server Connected!- sending" << std::endl;
			iResult = send(info::ClientSocketo, p3.c_str(), sizeof(recvbuf), 0);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(info::ClientSocketo);
				WSACleanup();
				return;

		}

}

void serverinteract() {

	WSADATA wsaData;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult;
	int iSendResult;
	while (true) {
		printf("listening\n");
		iResult = recv(info::ClientSocketo, recvbuf, recvbuflen, 0);
		if (iResult > 0) {

			info::mtxo.lock();
			printf("broadcasting\n");


			// finding the gCueAngle and gCuePower
			std::string s = recvbuf;
			std::string delimiter = ":";
			size_t pos = 0;
			std::string pangle;
			std::string ppower;
			while ((pos = s.find(delimiter)) != std::string::npos) {
				pangle = s.substr(0, pos);

				s.erase(0, pos + delimiter.length());
			}
			ppower = s;
			double qangle = ::atof(pangle.c_str());
			double qpower = ::atof(ppower.c_str());
			gCueAngle = qangle;
			gCuePower = qpower;
			vec2 imp((-sin(gCueAngle) * gCuePower * gCueBallFactor),
				(-cos(gCueAngle) * gCuePower * gCueBallFactor));
			std::cout << " --- " << canhitM << "------" << qangle << " --- " << qpower << std::endl;
			if (strcmp(recvbuf, "0:0") != 0) {
				std::cout << "server says" << recvbuf << "------" << qangle << " --- " << qpower << " --- " << canhitM << std::endl;
				if (canhitM == true) {
					std::cout << "Enter inside" << std::endl;
					gTable.balls[0].ApplyImpulse(imp);
					gCueAngle = 0.0;
					gCuePower = 0.0;
					gCueAnglePass = 0.0;
					gCuePowerPass = 0.0;
					canhitM = false;
					nonRepeat = true;
				}
			}
		



			info::mtxo.unlock();
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


void Clientinteract() {

	WSADATA wsaData;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult;
	int iSendResult;

	while (true) {
		printf("listening\n");
		iResult = recv(info2::ClientSocketi, recvbuf, recvbuflen, 0);
		if (iResult > 0) {

			// finding the gCueAngle and gCuePower
			std::string s = recvbuf;
			std::string delimiter = ":";
			size_t pos = 0;
			std::string pangle;
			std::string ppower;
			while ((pos = s.find(delimiter)) != std::string::npos) {
				pangle = s.substr(0, pos);

				s.erase(0, pos + delimiter.length());
			}
			ppower = s;
			double qangle = ::atof(pangle.c_str());
			double qpower = ::atof(ppower.c_str());
			gCueAngle = qangle;
			gCuePower = qpower;
			vec2 imp((-sin(gCueAngle) * gCuePower * gCueBallFactor),
				(-cos(gCueAngle) * gCuePower * gCueBallFactor));
			std::cout << " --- " << canhitM << "------" << qangle << " --- " << qpower << std::endl;
			if (strcmp(recvbuf, "0:0") != 0) {
				std::cout << "server says" << recvbuf << "------" << qangle << " --- " << qpower << " --- " << canhitM << std::endl;
				if (canhitM == true) {
					std::cout << "Enter inside" << std::endl;
					gTable.balls[0].ApplyImpulse(imp);
					gCueAngle = 0.0;
					gCuePower = 0.0;
					gCueAnglePass = 0.0;
					gCuePowerPass = 0.0;
					canhitM = false;
					nonRepeat = true;
				}
			

			}

		}
		
	}
}
void RenderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set camera
	glLoadIdentity();
	gluLookAt(gCamPos(0),gCamPos(1),gCamPos(2),gCamLookAt(0),gCamLookAt(1),gCamLookAt(2),0.0f,1.0f,0.0f);
	//draw the ball
	glColor3f(0.3, 0.3, 0.3);
	
	if (welcomeOn == true) {
		glColor3f(1.0, 1.0, 1.0);
		//draw text
		char  da[] = "Welcome to the pool game";
		glRasterPos2f(-0.3f, 0.71f);
		glColor3f(1., 0., 0.);
		int len = strlen(da);
		for (int i = 0; i < len; i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, da[i]);
		}

		
		

		if (chosenMode == false) {
			char  c01[] = "Press Enter to choose an option";
			glRasterPos2f(-0.3f, 0.51f);
			glColor3f(1., 0., 0.);
			int lenca = strlen(c01);
			for (int i = 0; i < lenca; i++) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c01[i]);
			}

			char  c02[] = "Play offline";
			char  c03[] = "Play Multiplayer LAN";
			char  c04[] = "->";

			glRasterPos2f(-0.3f, 0.41f);
			glColor3f(1., 0., 0.);
			int lencb = strlen(c02);
			for (int i = 0; i < lencb; i++) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c02[i]);
			}
			glRasterPos2f(-0.3f, 0.31f);
			glColor3f(1., 0., 0.);
			int lencc = strlen(c03);
			for (int i = 0; i < lencc; i++) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c03[i]);
			}
			if (playOffline == true) {
				glRasterPos2f(-0.38f, 0.41f);
			}
			else {
				glRasterPos2f(-0.38f, 0.31f);
			}
			//glRasterPos2f(-0.38f, 0.31f);
			glColor3f(1., 0., 0.);
			int lensel = strlen(c04);
			for (int i = 0; i < lensel; i++) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c04[i]);
			}
		}
		else {
			//welcomeOn = false;
			if (playOffline == true) {
				welcomeOn = false;
				startgame = true;

			}
			else {
				if (chooseRoleDone == true) {
					welcomeOn = false;
					startgame = true;

				}
				else {
					char  c02[] = "Connecting....";
					char  c03[] = "Join";
					char  c04[] = "->";

					glRasterPos2f(-0.3f, 0.41f);
					glColor3f(1., 0., 0.);
					int lencb = strlen(c02);
					for (int i = 0; i < lencb; i++) {
						glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c02[i]);
					}


					if (collectionMode == true) {
						identity = 1;
					}
					else {
						identity = 1;
					}
					
				}
				

			}


		}


	}

	if (startgame == true) {
		for (int i = 0; i < NUM_BALLS; i++)
		{
			if (i == 0) {

				glPushMatrix();
				glTranslatef(gTable.balls[i].position(0), (BALL_RADIUS / 2.0), gTable.balls[i].position(1));
				glutSolidSphere(gTable.balls[i].radius, 32, 32);
				glPopMatrix();
				glColor3f(0.5, 0, 1.0);
			}
			else {
				if (gTable.balls[i].touched == 0) {
					GLUquadricObj* cylin = gluNewQuadric();
					glPushMatrix();
					glTranslatef(gTable.balls[i].position(0), (BALL_RADIUS / 2.0), gTable.balls[i].position(1));
					GLUquadricObj* quadratic;
					quadratic = gluNewQuadric();
					glColor3f(1, 0, 0);
					glPushMatrix();
					glRotatef(90, -1, 0, 0);
					glColor3f(1, 0, 0);
					gluCylinder(quadratic, 0.05f, 0.01f, 0.2f, 32, 32);
					glPopMatrix();


					GLUquadricObj* ball;
					ball = gluNewQuadric();
					glPushMatrix();
					glTranslatef(gTable.balls[i].position(0), (BALL_RADIUS)+0.19f, gTable.balls[i].position(1));
					gluSphere(ball, 0.02f, 30, 30);
					glPopMatrix();
					/*
					GLUquadricObj* ball2;
					ball2 = gluNewQuadric();
					glPushMatrix();
					glTranslatef(gTable.balls[i].position(0), (BALL_RADIUS), gTable.balls[i].position(1) + 0.01);
					gluSphere(ball2, 0.02f, 30, 30);
					glPopMatrix();
					*/
				}
				else {
					GLUquadricObj* cylin = gluNewQuadric();
					glPushMatrix();
					glTranslatef(gTable.balls[i].position(0), (BALL_RADIUS), gTable.balls[i].position(1));
					GLUquadricObj* quadratic;
					quadratic = gluNewQuadric();
					glColor3f(1, 0, 0);
					glPushMatrix();
					glRotatef(0, 1, 0, 0);
					glColor3f(1, 1, 1);
					gluCylinder(quadratic, 0.01f, 0.05f, 0.2f, 32, 32);
					glPopMatrix();


					GLUquadricObj* ball;
					ball = gluNewQuadric();
					glPushMatrix();
					glTranslatef(gTable.balls[i].position(0), (BALL_RADIUS), gTable.balls[i].position(1));
					gluSphere(ball, 0.02f, 30, 30);
					glPopMatrix();
				}


			}
		}



		int gg = 0;
		for (int i = 0; i < NUM_BALLS; i++)
		{
			if (gTable.balls[i].touched == 1) {
				gg++;
			}
		}
		if (gg == 10) {
			char  strikechar[] = "Strike!";
			glRasterPos2f(0.1f, 0.71f);
			glColor3f(1., 0., 0.);
			int lenx = strlen(strikechar);
			for (int i = 0; i < lenx; i++) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, strikechar[i]);
			}
			//std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		if (Gameturn >= maxGameTurn) {
			char  strikechariX[] = "Game Over!";
			gameOver = true;
			glRasterPos2f(0.0f, 0.61f);
			glColor3f(1., 0., 0.);
			int lenOi = strlen(strikechariX);
			for (int i = 0; i < lenOi; i++) {
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, strikechariX[i]);
			}
			char  whowon[2] = "";
			char  resultx[] = " Wins!";
			if (gTable.players[0].score > gTable.players[1].score) {

				std::strcat(whowon, "1");
			}
			else {
				std::strcat(whowon, "2");
			}
			char* newArray = new char[std::strlen("Player ") + std::strlen(whowon) + 1];
			std::strcpy(newArray, "Player ");
			std::strcat(newArray, whowon);
			int len12 = strlen(resultx);
			int len22 = strlen(newArray);
			//std::cout << lenOi << " str" << len12 << " res" << len22 << " who";

			for (int j = 0; j < len22; j++) {
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, newArray[j]);
			}
			for (int k = 0; k < len12; k++) {
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, resultx[k]);
			}
		}

		if (gTable.AnyBallsMoving() == false && nonRepeat == true) {
			gTable.players[Gameturn % NUM_PLAYERS].score = gTable.players[Gameturn % NUM_PLAYERS].score + gg;
			nonRepeat = false;
			//std::cout <<" Soldier Boy " << gTable.players[Gameturn % NUM_PLAYERS].score;
		}
		//gTable.players[0].score = gTable.players[0].score + gg;


		char  da1[20] = "0";
		itoa(gTable.players[0].score, da1, 10);
		glRasterPos2f(-0.6f, 0.7f);
		glColor3f(1., 0., 0.);
		char  da2[20] = "player 1 Score :";
		int lenc1 = strlen(da2);
		for (int i = 0; i < lenc1; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, da2[i]);
		}
		int lenc = strlen(da1);
		for (int i = 0; i < lenc; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, da1[i]);
		}

		char  da3[20] = "0";
		itoa(gTable.players[1].score, da3, 10);
		glRasterPos2f(-0.6f, 0.8f);
		glColor3f(1., 0., 0.);
		char  da4[20] = "player 2 Score :";
		int lenc2 = strlen(da4);
		for (int i = 0; i < lenc2; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, da4[i]);
		}
		int lenc3 = strlen(da3);
		for (int i = 0; i < lenc3; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, da3[i]);
		}

		char  playerOnstage[20] = "You are Player";
		char playeridv[] = "0";
		int currentplayer = identity+1;
		itoa(currentplayer, playeridv, 10);
		glRasterPos2f(-0.2f, 0.8f);
		glColor3f(1., 0., 0.);
		std::string p10 = playerOnstage;
		std::string p20 = playeridv;
		std::string p30 = p10 + " " + p20;
		int lenc20 = strlen(p30.c_str());
		for (int i = 0; i < lenc20; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, p30.c_str()[i]);
		}
		
		char  playerOnstage1[] = "Playing now - Player";
		char playeridv1[] = "0";
		int currentplayer1 = (Gameturn % NUM_PLAYERS) + 1;
		itoa(currentplayer1, playeridv1, 10);
		glRasterPos2f(-0.2f, 0.7f);
		glColor3f(1., 0., 0.);
		std::string p101 = playerOnstage1;
		std::string p201 = playeridv1;
		std::string p301 = p101 + " " + p201;
		int lenc201 = strlen(p301.c_str());
		for (int i = 0; i < lenc201; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, p301.c_str()[i]);
		}




		//draw the table
		for (int i = 0; i < NUM_CUSHIONS; i++)
		{

			glBegin(GL_LINE_LOOP);
			glVertex3f(gTable.cushions[i].vertices[0](0), 0.0, gTable.cushions[i].vertices[0](1));
			glVertex3f(gTable.cushions[i].vertices[0](0), 0.1, gTable.cushions[i].vertices[0](1));
			glVertex3f(gTable.cushions[i].vertices[1](0), 0.1, gTable.cushions[i].vertices[1](1));
			glVertex3f(gTable.cushions[i].vertices[1](0), 0.0, gTable.cushions[i].vertices[1](1));
			glEnd();
		}
		for (int i = 0; i < NUM_CUSHIONS; i++)
		{

			glBegin(GL_LINE_LOOP);
			glVertex3f(gLane.cushions[i].vertices[0](0), 0.0, gLane.cushions[i].vertices[0](1));
			glVertex3f(gLane.cushions[i].vertices[0](0), 0.1, gLane.cushions[i].vertices[0](1));
			glVertex3f(gLane.cushions[i].vertices[1](0), 0.1, gLane.cushions[i].vertices[1](1));
			glVertex3f(gLane.cushions[i].vertices[1](0), 0.0, gLane.cushions[i].vertices[1](1));
			glEnd();
		}

		for (int i = 0; i < gTable.parts.num; i++)
		{
			glColor3f(1.0, 0.0, 0.0);
			glPushMatrix();
			glTranslatef(gTable.parts.particles[i]->position(0), gTable.parts.particles[i]->position(1), gTable.parts.particles[i]->position(2));
#if DRAW_SOLID
			glutSolidSphere(0.002f, 32, 32);
#else
			glutWireSphere(0.002f, 12, 12);
#endif
			glPopMatrix();
		}
		/*
		glBegin(GL_LINE_LOOP);
		glVertex3f (TABLE_X, 0.0, -TABLE_Z);
		glVertex3f (TABLE_X, 0.1, -TABLE_Z);
		glVertex3f (TABLE_X, 0.1, TABLE_Z);
		glVertex3f (TABLE_X, 0.0, TABLE_Z);
		glEnd();
		glBegin(GL_LINE_LOOP);
		glVertex3f (TABLE_X, 0.0, -TABLE_Z);
		glVertex3f (TABLE_X, 0.1, -TABLE_Z);
		glVertex3f (-TABLE_X, 0.1, -TABLE_Z);
		glVertex3f (-TABLE_X, 0.0, -TABLE_Z);
		glEnd();
		glBegin(GL_LINE_LOOP);
		glVertex3f (TABLE_X, 0.0, TABLE_Z);
		glVertex3f (TABLE_X, 0.1, TABLE_Z);
		glVertex3f (-TABLE_X, 0.1, TABLE_Z);
		glVertex3f (-TABLE_X, 0.0, TABLE_Z);
		glEnd();
		*/

		//draw the cue
		if (gDoCue)
		{
			glBegin(GL_LINES);
			float cuex = sin(gCueAngle) * gCuePower;
			float cuez = cos(gCueAngle) * gCuePower;
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(gTable.balls[0].position(0), (BALL_RADIUS / 2.0f), gTable.balls[0].position(1));
			glVertex3f((gTable.balls[0].position(0) + cuex), (BALL_RADIUS / 2.0f), (gTable.balls[0].position(1) + cuez));
			glColor3f(1.0, 0, 0);
			glEnd();
		}

		//glPopMatrix();

	}

	if (startMultiplayer == true) {
		for (int i = 0; i < NUM_BALLS; i++)
		{
			if (i == 0) {

				glPushMatrix();
				glTranslatef(gTable.balls[i].position(0), (BALL_RADIUS / 2.0), gTable.balls[i].position(1));
				glutSolidSphere(gTable.balls[i].radius, 32, 32);
				glPopMatrix();
				glColor3f(0.5, 0, 1.0);
			}
			else {
				if (gTable.balls[i].touched == 0) {
					GLUquadricObj* cylin = gluNewQuadric();
					glPushMatrix();
					glTranslatef(gTable.balls[i].position(0), (BALL_RADIUS / 2.0), gTable.balls[i].position(1));
					GLUquadricObj* quadratic;
					quadratic = gluNewQuadric();
					glColor3f(1, 0, 0);
					glPushMatrix();
					glRotatef(90, -1, 0, 0);
					glColor3f(1, 0, 0);
					gluCylinder(quadratic, 0.05f, 0.01f, 0.2f, 32, 32);
					glPopMatrix();


					GLUquadricObj* ball;
					ball = gluNewQuadric();
					glPushMatrix();
					glTranslatef(gTable.balls[i].position(0), (BALL_RADIUS)+0.19f, gTable.balls[i].position(1));
					gluSphere(ball, 0.02f, 30, 30);
					glPopMatrix();
					/*
					GLUquadricObj* ball2;
					ball2 = gluNewQuadric();
					glPushMatrix();
					glTranslatef(gTable.balls[i].position(0), (BALL_RADIUS), gTable.balls[i].position(1) + 0.01);
					gluSphere(ball2, 0.02f, 30, 30);
					glPopMatrix();
					*/
				}
				else {
					GLUquadricObj* cylin = gluNewQuadric();
					glPushMatrix();
					glTranslatef(gTable.balls[i].position(0), (BALL_RADIUS), gTable.balls[i].position(1));
					GLUquadricObj* quadratic;
					quadratic = gluNewQuadric();
					glColor3f(1, 0, 0);
					glPushMatrix();
					glRotatef(0, 1, 0, 0);
					glColor3f(1, 1, 1);
					gluCylinder(quadratic, 0.01f, 0.05f, 0.2f, 32, 32);
					glPopMatrix();


					GLUquadricObj* ball;
					ball = gluNewQuadric();
					glPushMatrix();
					glTranslatef(gTable.balls[i].position(0), (BALL_RADIUS), gTable.balls[i].position(1));
					gluSphere(ball, 0.02f, 30, 30);
					glPopMatrix();
				}


			}
		}



		int gg = 0;
		for (int i = 0; i < NUM_BALLS; i++)
		{
			if (gTable.balls[i].touched == 1) {
				gg++;
			}
		}
		if (gg == 10) {
			char  strikechar[] = "Strike!";
			glRasterPos2f(0.1f, 0.71f);
			glColor3f(1., 0., 0.);
			int lenx = strlen(strikechar);
			for (int i = 0; i < lenx; i++) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, strikechar[i]);
			}
			//std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		if (Gameturn >= maxGameTurn) {
			char  strikechariX[] = "Game Over!";
			gameOver = true;
			glRasterPos2f(0.0f, 0.61f);
			glColor3f(1., 0., 0.);
			int lenOi = strlen(strikechariX);
			for (int i = 0; i < lenOi; i++) {
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, strikechariX[i]);
			}
			char  whowon[2] = "";
			char  resultx[] = " Wins!";
			if (gTable.players[0].score > gTable.players[1].score) {

				std::strcat(whowon, "1");
			}
			else {
				std::strcat(whowon, "2");
			}
			char* newArray = new char[std::strlen("Player ") + std::strlen(whowon) + 1];
			std::strcpy(newArray, "Player ");
			std::strcat(newArray, whowon);
			int len12 = strlen(resultx);
			int len22 = strlen(newArray);
			//std::cout << lenOi << " str" << len12 << " res" << len22 << " who";

			for (int j = 0; j < len22; j++) {
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, newArray[j]);
			}
			for (int k = 0; k < len12; k++) {
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, resultx[k]);
			}
		}

		if (gTable.AnyBallsMoving() == false && nonRepeat == true) {
			gTable.players[Gameturn % NUM_PLAYERS].score = gTable.players[Gameturn % NUM_PLAYERS].score + gg;
			nonRepeat = false;
			canhitM = true;
			//std::cout <<" Soldier Boy " << gTable.players[Gameturn % NUM_PLAYERS].score;
		}
		//gTable.players[0].score = gTable.players[0].score + gg;


		char  da1[20] = "0";
		itoa(gTable.players[0].score, da1, 10);
		glRasterPos2f(-0.6f, 0.7f);
		glColor3f(1., 0., 0.);
		char  da2[20] = "player 1 Score :";
		int lenc1 = strlen(da2);
		for (int i = 0; i < lenc1; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, da2[i]);
		}
		int lenc = strlen(da1);
		for (int i = 0; i < lenc; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, da1[i]);
		}

		char  da3[20] = "0";
		itoa(gTable.players[1].score, da3, 10);
		glRasterPos2f(-0.6f, 0.8f);
		glColor3f(1., 0., 0.);
		char  da4[20] = "player 2 Score :";
		int lenc2 = strlen(da4);
		for (int i = 0; i < lenc2; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, da4[i]);
		}
		int lenc3 = strlen(da3);
		for (int i = 0; i < lenc3; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, da3[i]);
		}


		//draw the table
		for (int i = 0; i < NUM_CUSHIONS; i++)
		{

			glBegin(GL_LINE_LOOP);
			glVertex3f(gTable.cushions[i].vertices[0](0), 0.0, gTable.cushions[i].vertices[0](1));
			glVertex3f(gTable.cushions[i].vertices[0](0), 0.1, gTable.cushions[i].vertices[0](1));
			glVertex3f(gTable.cushions[i].vertices[1](0), 0.1, gTable.cushions[i].vertices[1](1));
			glVertex3f(gTable.cushions[i].vertices[1](0), 0.0, gTable.cushions[i].vertices[1](1));
			glEnd();
		}
		for (int i = 0; i < NUM_CUSHIONS; i++)
		{

			glBegin(GL_LINE_LOOP);
			glVertex3f(gLane.cushions[i].vertices[0](0), 0.0, gLane.cushions[i].vertices[0](1));
			glVertex3f(gLane.cushions[i].vertices[0](0), 0.1, gLane.cushions[i].vertices[0](1));
			glVertex3f(gLane.cushions[i].vertices[1](0), 0.1, gLane.cushions[i].vertices[1](1));
			glVertex3f(gLane.cushions[i].vertices[1](0), 0.0, gLane.cushions[i].vertices[1](1));
			glEnd();
		}

		for (int i = 0; i < gTable.parts.num; i++)
		{
			glColor3f(1.0, 0.0, 0.0);
			glPushMatrix();
			glTranslatef(gTable.parts.particles[i]->position(0), gTable.parts.particles[i]->position(1), gTable.parts.particles[i]->position(2));
#if DRAW_SOLID
			glutSolidSphere(0.002f, 32, 32);
#else
			glutWireSphere(0.002f, 12, 12);
#endif
			glPopMatrix();
		}
		/*
		glBegin(GL_LINE_LOOP);
		glVertex3f (TABLE_X, 0.0, -TABLE_Z);
		glVertex3f (TABLE_X, 0.1, -TABLE_Z);
		glVertex3f (TABLE_X, 0.1, TABLE_Z);
		glVertex3f (TABLE_X, 0.0, TABLE_Z);
		glEnd();
		glBegin(GL_LINE_LOOP);
		glVertex3f (TABLE_X, 0.0, -TABLE_Z);
		glVertex3f (TABLE_X, 0.1, -TABLE_Z);
		glVertex3f (-TABLE_X, 0.1, -TABLE_Z);
		glVertex3f (-TABLE_X, 0.0, -TABLE_Z);
		glEnd();
		glBegin(GL_LINE_LOOP);
		glVertex3f (TABLE_X, 0.0, TABLE_Z);
		glVertex3f (TABLE_X, 0.1, TABLE_Z);
		glVertex3f (-TABLE_X, 0.1, TABLE_Z);
		glVertex3f (-TABLE_X, 0.0, TABLE_Z);
		glEnd();
		*/

		//draw the cue
		if (gDoCue)
		{
			glBegin(GL_LINES);
			float cuex = sin(gCueAngle) * gCuePower;
			float cuez = cos(gCueAngle) * gCuePower;
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(gTable.balls[0].position(0), (BALL_RADIUS / 2.0f), gTable.balls[0].position(1));
			glVertex3f((gTable.balls[0].position(0) + cuex), (BALL_RADIUS / 2.0f), (gTable.balls[0].position(1) + cuez));
			glColor3f(1.0, 0, 0);
			glEnd();
		}

		//glPopMatrix();

	}
	glFlush();
	glutSwapBuffers();
	
	
}


void SpecKeyboardFunc(int key, int x, int y) 
{
	switch(key)
	{
		case GLUT_KEY_LEFT:
		{
			/*if (playOffline == true) {
				gCueControl[0] = true;
			}
			else {
				if (Gameturn % NUM_PLAYERS == 0) {
					gCueControl[0] = true;
					pickedKey = 2;
				}
				else {
					pickedKey = 2;
				}
			}*/
			gCueControl[0] = true;			
			break;
		}
		case GLUT_KEY_RIGHT:
		{

			/*if (playOffline == true) {
				gCueControl[1] = true;
			}
			else {
				if (Gameturn % NUM_PLAYERS == 0) {
					gCueControl[1] = true;
					pickedKey = 3;
				}
				else {
					pickedKey = 3;
				}
			}*/
			gCueControl[1] = true;
			break;
		}
		case GLUT_KEY_UP:
		{

				/*if (playOffline == true) {
				gCueControl[2] = true;
			}
			else {
				if (Gameturn % NUM_PLAYERS == 0) {
					gCueControl[2] = true;
					std::cout << "going up";
					pickedKey = 4;
				}
				else {
					pickedKey = 4;
				}
			}*/
			gCueControl[2] = true;
			break;
		}
		case GLUT_KEY_DOWN:
		{

				/*if (playOffline == true) {
				gCueControl[3] = true;
			}
			else {
				if (Gameturn % NUM_PLAYERS == 0) {
					gCueControl[3] = true;
					pickedKey = 5;
				}
				else {
					pickedKey = 5;
				}
			}*/
			gCueControl[3] = true;
			break;
		}
	}
}

void SpecKeyboardUpFunc(int key, int x, int y) 
{
	switch(key)
	{
		case GLUT_KEY_LEFT:
		{
			gCueControl[0] = false;
			
			break;
		}
		case GLUT_KEY_RIGHT:
		{
			gCueControl[1] = false;
			
			break;
		}
		case GLUT_KEY_UP:
		{
			gCueControl[2] = false;
			
			break;
		}
		case GLUT_KEY_DOWN:
		{
			gCueControl[3] = false;
			
			break;
		}
	}
}

void KeyboardFunc(unsigned char key, int x, int y) 
{
	switch(key)
	{
	case('w'):
		{
			if (welcomeOn == true)
			{
				if (chosenMode == false) {
					playOffline = true;
					std::cout << "playOffline";
				}
				else {
					collectionMode = true;
					std::cout << "host";
				}
				
			}
			break;
		}	
	case('s'):
		{
			if (welcomeOn == true)
			{
				if (chosenMode == false) {
					playOffline = false;
					std::cout << "playOfflinefalse";
				}
				else {
					collectionMode = false;
					std::cout << "join";
				}
			
			}
			break;

		}
	case(13):
	{
	
		if (chosenMode == false) {
			chosenMode = true;
			
		}
		
		break;

	}
	case(32):
		{
			if(gDoCue)
			{
				int turn = 0;
				if (Gameturn % NUM_PLAYERS == 0) {
					printf("only server can play");
					turn = 0;
				}
				else {
					printf("only client can play");
					turn = 1;
				}
				std::cout << Gameturn;
				if (playOffline == true) {
					vec2 imp((-sin(gCueAngle) * gCuePower * gCueBallFactor),
						(-cos(gCueAngle) * gCuePower * gCueBallFactor));
					if (gameOver == false && canhit == true) {
						gTable.balls[0].ApplyImpulse(imp);
						canhit = false;
						nonRepeat = true;
					}
				}
				else {
					if (identity==0 ) {
						vec2 imp((-sin(gCueAngle) * gCuePower * gCueBallFactor),
							(-cos(gCueAngle) * gCuePower * gCueBallFactor));
						if (gameOver == false && canhit == true) {
							gTable.balls[0].ApplyImpulse(imp);
							gCueAnglePass = gCueAngle;
							gCuePowerPass = gCuePower;
							canhit = false;
							canhitM = true;
							nonRepeat = true;
							sendMessage = true;
							printf("Sending from sevrer");
							sendCommand(gCueAnglePass, gCuePowerPass);
						}
					}
					else if(identity == 1){
						vec2 imp((-sin(gCueAngle) * gCuePower * gCueBallFactor),
							(-cos(gCueAngle) * gCuePower * gCueBallFactor));
						if (gameOver == false && canhit == true) {
							gTable.balls[0].ApplyImpulse(imp);
							gCueAnglePass = gCueAngle;
							gCuePowerPass = gCuePower;
							canhit = false;
							canhitM = true;
							nonRepeat = true;
							sendMessage = true;
							printf("Sending from client");
							sendServerCommand(gCueAnglePass, gCuePowerPass);
						}
					}
				}
			
				//gCueAnglePass = 0.0;
				//gCuePowerPass = 0.0;
				
				
			}
			break;
		}
	case(27):
		{
			for(int i=0;i<NUM_BALLS;i++)
			{
				gTable.balls[i].Reset();
			}
			break;
		}
	case('m'):
		{
			gCamRotate = false;
			break;
		}
	case('z'):
		{
			gCamL = true;
			break;
		}
	case('c'):
		{
			gCamR = true;
			break;
		}
	case('h'):
		{
			gCamU = true;
			break;
		}
	case('x'):
		{
			gCamD = true;
			break;
		}
	case('f'):
		{
			gCamZin = true;
			break;
		}
	case('v'):
		{
			gCamZout = true;
			break;
		}
	case('q'):
		{
		//welcomeOn = false;
		if (gameOver == false) {
			if (gTable.AnyBallsMoving() == false) {
				nonRepeat = true;
				Gameturn++;
				canhit = true;
				gCueAnglePass = 0.0;
				gCuePowerPass = 0.0;
				gCueAngle = 0.0;
				gCuePower = 0.0;
				for (int i = 0; i < NUM_BALLS; i++)
				{
					gTable.balls[i].Reset();
				}
			}
		}
	

		break;
		}
	}

}

void KeyboardUpFunc(unsigned char key, int x, int y) 
{
	switch(key)
	{
	case(32):
		{
			gCamRotate = true;
			break;
		}
	case('z'):
		{
			gCamL = false;
			break;
		}
	case('c'):
		{
			gCamR = false;
			break;
		}
	case('j'):
		{
			gCamU = false;
			break;
		}
	case('x'):
		{
			gCamD = false;
			break;
		}
	case('f'):
		{
			gCamZin = false;
			break;
		}
	case('v'):
		{
			gCamZout = false;
			break;
		}
	}
}

void ChangeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0) h = 1;
	float ratio = 1.0* w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45,ratio,0.2,1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(0.0,0.7,2.1, 0.0,0.0,0.0, 0.0f,1.0f,0.0f);
	gluLookAt(gCamPos(0),gCamPos(1),gCamPos(2),gCamLookAt(0),gCamLookAt(1),gCamLookAt(2),0.0f,1.0f,0.0f);
}

int clientConnectfn(int argc, char** argv) {
	bool dotheaction = false;
	while (dotheaction == false) {
		if (chooseRoleDone == true && collectionMode == false) {
			dotheaction = true;
			std::cout << "xCLIENTx";

			WSADATA wsaData;
			SOCKET ConnectSocket = INVALID_SOCKET;
			struct addrinfo* result = NULL, * ptr = NULL, hints;
			auto* sendbuf = "this is a test";
			char recvbuf[DEFAULT_BUFLEN];
			int iResult;
			int iSendResult;
			int recvbuflen = DEFAULT_BUFLEN;
			printf(sendbuf);
			// Validate the parameters
			if (argc != 2) {
				printf("usage: %s server-name\n", argv[0]);
				//return 1;
			}


			// Initialize Winsock
			iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (iResult != 0) {
				printf("WSAStartup failed with error: %d\n", iResult);
				return 1;
			}


			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;

			// Resolve the server address and port
			iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
			if (iResult != 0) {
				printf("getaddrinfo failed with error: %d\n", iResult);
				WSACleanup();
				return 1;
			}

			// Attempt to connect to an address until one succeeds
			for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

				// Create a SOCKET for connecting to server
				ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
					ptr->ai_protocol);
				if (ConnectSocket == INVALID_SOCKET) {
					printf("socket failed with error: %ld\n", WSAGetLastError());
					WSACleanup();
					return 1;
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
				return 1;
			}

			

			

		
				// Receive until the peer shuts down the connection
				//iResult = 1;
				while(true) {
					
						//std::cout << " Client what happened";
						char msgc[] = "0.0";
						char powergc[] = "0.0";
						char pssval[] = "0.0";

						// creating stringstream objects
						std::stringstream ss1;
						std::stringstream ss2;

						// assigning the value of num_float to ss1
						ss1 << gCueAnglePass;
						ss2 << gCuePowerPass;

						std::string str1 = ss1.str();
						std::string str2 = ss2.str();
						std::string p1 = str1;
						std::string p2 = str2;
						std::string p3 = p1 + ":" + p2;
						if (Gameturn % NUM_PLAYERS == identity) {
							std::cout << "Client Connected!- sending" << std::endl;
							if (sendMessage == true) {
								std::cout << "SENDING  - IMPLULSEEE" << std::endl;
								iResult = send(ConnectSocket, p3.c_str(), sizeof(recvbuf), 0);
								if (iResult == SOCKET_ERROR) {
									printf("send failed with error: %d\n", WSAGetLastError());
									closesocket(ConnectSocket);
									WSACleanup();
									return 1;
								}
								if (strcmp(p3.c_str(), "0:0") != 0) {
									sendMessage = false;
								}
							}
							

						}
						else {
							//if(Gameturn == 2) std::cout << "Client Connected! - recieving" << std::endl;
							iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
							if (iResult > 0) {

								// finding the gCueAngle and gCuePower
								std::string s = recvbuf;
								std::string delimiter = ":";
								size_t pos = 0;
								std::string pangle;
								std::string ppower;
								while ((pos = s.find(delimiter)) != std::string::npos) {
									pangle = s.substr(0, pos);

									s.erase(0, pos + delimiter.length());
								}
								ppower = s;
								double qangle = ::atof(pangle.c_str());
								double qpower = ::atof(ppower.c_str());
								gCueAngle = qangle;
								gCuePower = qpower;
								vec2 imp((-sin(gCueAngle) * gCuePower * gCueBallFactor),
									(-cos(gCueAngle) * gCuePower * gCueBallFactor));
								std::cout  <<" --- " << canhitM << "------" << qangle << " --- " << qpower << std::endl;
								if (strcmp(recvbuf, "0:0") != 0) {
									std::cout << "server says"<< recvbuf<<"------" << qangle << " --- " << qpower << " --- " << canhitM << std::endl;
									if (canhitM == true) {
										std::cout << "Enter inside" << std::endl;
										gTable.balls[0].ApplyImpulse(imp);
										gCueAngle = 0.0;
										gCuePower = 0.0;
										gCueAnglePass = 0.0;
										gCuePowerPass = 0.0;
										canhitM = false;
										nonRepeat = true;
									}
									/*while (gTable.AnyBallsMoving() == true) {
										//asd
									}
									if (gameOver == false) {
										if (gTable.AnyBallsMoving() == false) {
											nonRepeat = true;
											Gameturn++;
											canhit = true;
											canhitM = true;
											gCueAnglePass = 0.0;
											gCuePowerPass = 0.0;
											gCueAngle = 0.0;
											gCuePower = 0.0;
											for (int i = 0; i < NUM_BALLS; i++)
											{
												gTable.balls[i].Reset();
											}
										}
									}*/

								}
								else {
									canhitM = true;
								}

								// Echo the buffer back to the sender
								/*std::string reply = "Did you say '";
								reply += recvbuf;
								reply += "'?";
								iSendResult = send(ConnectSocket, reply.c_str(), iResult, 0);
								if (iSendResult == SOCKET_ERROR) {
									printf("send failed with error: %d\n", WSAGetLastError());
									closesocket(ConnectSocket);
									WSACleanup();
									return 1;
								}
								*/

							}
							else if (iResult == 0)
								printf("Connection closing...\n");
							else {
								printf("recv failed with error: %d\n", WSAGetLastError());
								closesocket(ConnectSocket);
								WSACleanup();
								return 1;

							}

						}

					
				} 
		
				
		

			
			// cleanup
			closesocket(ConnectSocket);
			WSACleanup();

			return 0;

}
	}
	


}
int serverConnectfn(void) {
	bool dotheaction = false;
	while (dotheaction == false) {
		if (chooseRoleDone == true && collectionMode == true) {
			dotheaction = true;
			std::cout << "SERVER ";
			WSADATA wsaData;
			int iResult;

			SOCKET ListenSocket = INVALID_SOCKET;
			SOCKET ClientSocket = INVALID_SOCKET;

			struct addrinfo* result = NULL;
			struct addrinfo hints;

			int iSendResult;
			char recvbuf[DEFAULT_BUFLEN];
			int recvbuflen = DEFAULT_BUFLEN;

			// Initialize Winsock
			iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (iResult != 0) {
				printf("WSAStartup failed with error: %d\n", iResult);
				return 1;
			}


			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			hints.ai_flags = AI_PASSIVE;

			// Resolve the server address and port
			iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
			if (iResult != 0) {
				printf("getaddrinfo failed with error: %d\n", iResult);

				WSACleanup();
				return 1;
			}



			// Create a SOCKET for connecting to server
			ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
			if (ListenSocket == INVALID_SOCKET) {
				printf("socket failed with error: %ld\n", WSAGetLastError());
				freeaddrinfo(result);
				WSACleanup();
				return 1;
			}

			// Setup the TCP listening socket
			iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				printf("bind failed with error: %d\n", WSAGetLastError());
				freeaddrinfo(result);
				closesocket(ListenSocket);
				WSACleanup();
				return 1;
			}



			freeaddrinfo(result);

			iResult = listen(ListenSocket, SOMAXCONN);
			if (iResult == SOCKET_ERROR) {
				printf("listen failed with error: %d\n", WSAGetLastError());
				closesocket(ListenSocket);
				WSACleanup();
				return 1;
			}



			// Accept a client socket
			ClientSocket = accept(ListenSocket, NULL, NULL);
			if (ClientSocket == INVALID_SOCKET) {
				printf("accept failed with error: %d\n", WSAGetLastError());
				closesocket(ListenSocket);
				WSACleanup();
				return 1;
			}


			// No longer need server socket
			//closesocket(ListenSocket);
			

				//iResult = 1;
				// Receive until the peer shuts down the connection
				while(true) {

						
					
						char msgc[] = "0.0";
						char powergc[] = "0.0";
						char pssval[] = "0.0";


						// creating stringstream objects
						std::stringstream ss1;
						std::stringstream ss2;

						// assigning the value of num_float to ss1
						ss1 << gCueAnglePass;

						// assigning the value of num_float to ss2
						ss2 << gCuePowerPass;

						// initializing two string variables with the values of ss1 and ss2
						// and converting it to string format with str() function
						std::string str1 = ss1.str();
						std::string str2 = ss2.str();

						//itoa(gCueAnglePass, msgc, 10);
						//itoa(gCuePowerPass, powergc, 10);
						std::string p1 = str1;
						std::string p2 = str2;
						std::string p3 = p1 + ":" + p2;
						if (Gameturn % NUM_PLAYERS == identity) {
							std::cout << "Server Connected!- sending" << std::endl;
							if (sendMessage == true) {
								iResult = send(ClientSocket, p3.c_str(), sizeof(recvbuf), 0);
								if (iResult == SOCKET_ERROR) {
									printf("send failed with error: %d\n", WSAGetLastError());
									closesocket(ClientSocket);
									WSACleanup();
									return 1;
								}
								if (strcmp(p3.c_str(), "0:0") != 0) {
									sendMessage = false;
								}
								
							}
							

						}
						else {
							std::cout << "Server Connected!- recieving" << std::endl;

							iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
							if (iResult > 0) {
								std::string s = recvbuf;
								std::string delimiter = ":";
								size_t pos = 0;
								std::string pangle;
								std::string ppower;
								while ((pos = s.find(delimiter)) != std::string::npos) {
									pangle = s.substr(0, pos);

									s.erase(0, pos + delimiter.length());
								}
								ppower = s;
								double qangle = ::atof(pangle.c_str());
								double qpower = ::atof(ppower.c_str());
								gCueAngle = qangle;
								gCuePower = qpower;
								vec2 imp((-sin(gCueAngle) * gCuePower * gCueBallFactor),
									(-cos(gCueAngle) * gCuePower * gCueBallFactor));
								std::cout << " --- " << canhitM << std::endl;
								if (strcmp(recvbuf, "0:0") != 0) {
									//std::cout << "server says"<< recvbuf<<"------" << qangle << " --- " << qpower << " --- " << canhitM << std::endl;
									if (canhitM == true) {
										std::cout << "Enter inside" << std::endl;
										gTable.balls[0].ApplyImpulse(imp);
										gCueAngle = 0.0;
										gCuePower = 0.0;
										gCueAnglePass = 0.0;
										gCuePowerPass = 0.0;
										canhitM = false;
										nonRepeat = true;
									}
									/*while (gTable.AnyBallsMoving() == true) {
										//asd
									}
									if (gameOver == false) {
										if (gTable.AnyBallsMoving() == false) {
											nonRepeat = true;
											Gameturn++;
											canhit = true;
											canhitM = true;
											gCueAnglePass = 0.0;
											gCuePowerPass = 0.0;
											gCueAngle = 0.0;
											gCuePower = 0.0;
											for (int i = 0; i < NUM_BALLS; i++)
											{
												gTable.balls[i].Reset();
											}
										}
									}*/

								}
								else {
									canhitM = true;
								}


								// Echo the buffer back to the sender
								/*std::string reply = "Did you say '";
								reply += recvbuf;
								reply += "'?";
								iSendResult = send(ClientSocket, reply.c_str(), iResult, 0);
								if (iSendResult == SOCKET_ERROR) {
									printf("send failed with error: %d\n", WSAGetLastError());
									closesocket(ClientSocket);
									WSACleanup();
									return 1;
								}
								*/

							}
							else if (iResult == 0)
								printf("Connection closing...\n");
							else {
								printf("recv failed with error: %d\n", WSAGetLastError());
								closesocket(ClientSocket);
								WSACleanup();
								return 1;
							}

						}

			

				}

				
		
			
			

			// shutdown the connection since we're done
			iResult = shutdown(ClientSocket, SD_SEND);
			if (iResult == SOCKET_ERROR) {
				printf("shutdown failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			
			// cleanup
			closesocket(ClientSocket);
			WSACleanup();

			return 0;
		}
	}

	
}

void InitLights(void)
{
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	GLfloat light_ambient[] = { 2.0, 2.0, 2.0, 1.0 };
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);
}

void UpdateScene(int ms) 
{
	if(gTable.AnyBallsMoving()==false) gDoCue = true;
	else gDoCue = false;
	

	if(gDoCue)
	{
		if(gCueControl[0]) gCueAngle -= ((gCueAngleSpeed * ms)/1000);
		if(gCueControl[1]) gCueAngle += ((gCueAngleSpeed * ms)/1000);
		if (gCueAngle <0.0) gCueAngle += TWO_PI;
		if (gCueAngle >TWO_PI) gCueAngle -= TWO_PI;

		if(gCueControl[2]) gCuePower += ((gCuePowerSpeed * ms)/1000);
		if(gCueControl[3]) gCuePower -= ((gCuePowerSpeed * ms)/1000);
		if(gCuePower > gCuePowerMax) gCuePower = gCuePowerMax;
		if(gCuePower < gCuePowerMin) gCuePower = gCuePowerMin;
	}

	DoCamera(ms);

	gTable.Update(ms);

	glutTimerFunc(SIM_UPDATE_MS, UpdateScene, SIM_UPDATE_MS);
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	
	gTable.SetupCushions();
	gLane.SetupCushions();

	//net::server.connect();
	//net::client.start(argv[1], argv[2]);

	

	//td::thread t1(serverConnectfn);
	std::thread t1(connectServer);
	std::thread t2(startClient, argv[1], argv[2]);

	//std::thread t3(serverinteract);
	//std::thread t4(Clientinteract);

	t1.detach();
	t2.detach();

	//t3.detach();
	//t4.detach();

	

	glutInit(&argc, ((char **)argv));
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE| GLUT_RGBA);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(1000,700);
	//glutFullScreen();
	glutCreateWindow("Bowling");
	
	InitLights();
	glutDisplayFunc(RenderScene);
	glutTimerFunc(SIM_UPDATE_MS, UpdateScene, SIM_UPDATE_MS);
	glutReshapeFunc(ChangeSize);
	glutIdleFunc(RenderScene);
	//viewScore();

	//clientConnectfn(argc, argv);
	//serverConnectfn();


	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(KeyboardFunc);
	glutKeyboardUpFunc(KeyboardUpFunc);
	glutSpecialFunc(SpecKeyboardFunc);
	glutSpecialUpFunc(SpecKeyboardUpFunc);
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();

	

	
}
