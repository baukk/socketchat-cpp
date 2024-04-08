#include <iostream>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<thread>
#include <string>

using namespace std;


#pragma comment(lib,"ws2_32.lib")

/*
init winsock

create socket

connect to server

send rcv


close
*/

bool initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}
void SndMessage(SOCKET s) {
	cout << "Enter your name to join chatroom" << endl;
	string name;
	getline(cin, name);
	cout << endl << endl;
	cout << "\t\t ===WELCOME TO THE CHATROOM=== "<<endl;
	cout << "You may leave the chatroom by typing \" \/quit\" " << endl;
	cout << endl << endl;
	cout << "----------------------------------------------------------" << endl;
	string message;
	while (1) {
		getline(cin, message);
		string msg = name + " : " + message;
		int bytessent = send(s, msg.c_str(), msg.length(), 0);
		if (bytessent == SOCKET_ERROR) {
			cout << "error sending msg";
			break;
		
		}
		if (message == "/quit") {
			cout << "stopping program" << endl;
			break;
		}
	}
	closesocket(s);
	WSACleanup();
}
void ReceiveMessage(SOCKET s) {
	char buffer[4096];
	string msg = "";
	while (1) {
		int recvlength = recv(s, buffer, sizeof(buffer), 0);
		if (recvlength <= 0) {
			cout << "dc from serv3r" << endl;
			break;

		}
			msg = string(buffer, recvlength);
			cout << msg << endl;
	}
	closesocket(s);
	WSACleanup();
}


int main() {
	cout << "Client program started" << endl;
	
	if (!initialize()) {
		cout << "init failed" << endl;

	}

	SOCKET s;

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		cout << "invalid socket" << endl;
		return 1;
	}

	int port = 12345;
	string serveraddress = "127.0.0.1";

	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	inet_pton(AF_INET, serveraddress.c_str(), &serveraddr.sin_addr);

	if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "soccekt error" << endl;
		closesocket(s);
		WSACleanup();
		return 1;
	}

	cout << "successfully connected to server" << endl<<endl;
	cout << "----------------------------------------------------------" << endl;
	
	
	thread senderthread(SndMessage, s);
	thread recieverthread(ReceiveMessage, s);
	//send/rcv

	senderthread.join();
	recieverthread.join();

	return 0;
}


















