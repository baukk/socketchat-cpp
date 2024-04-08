#include <iostream> 
#include<stdio.h>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<tchar.h>
#include<thread>
#include <vector>

using namespace std;
int port = 12345;
#pragma comment(lib, "ws2_32.lib")

bool initialize() {
	WSADATA wsaData;
	return WSAStartup(MAKEWORD(2, 2), &wsaData)==0;

}
void InteractWithClient(SOCKET clientSocket, vector<SOCKET>& clients) {
	cout << "connected with client" << endl;
	char buffer[4096];
	while (1) {
		int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (bytesrecvd <= 0) {
			cout << "client disconnected" << endl;
			break; // Exit the loop when client disconnects
		}
		string message(buffer, bytesrecvd);

		cout << "message from client: " << message << endl;

		for (auto client : clients) {
			if (client != clientSocket) {
				send(client, message.c_str(), message.length(), 0);
			}
		}
	}
	// Remove the client from the vector after the loop
	auto it = find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end()) {
		clients.erase(it);
	}
	closesocket(clientSocket);
}


int main() {

	if (!initialize()) {
		cout << "Initialization failed." << endl;
		return 1;
	}
	
	//sockets creation

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSocket == INVALID_SOCKET) {
		cout << "socket failed" << endl;
		return 1;
	}

	//crt structure

	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);


	//convert ip24

	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
		cout << "setting addr failed"<<endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;

	}
		
	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "bind fILEd" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
				//listen 

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "listen failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
	}

	cout << "Server started listning on " << port << endl;
	//acccept
	vector<SOCKET> clients;

	while (1) {
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET) {
			cout << "invaalid clnt socket" << endl;

		}
		clients.push_back(clientSocket);
		thread t1(InteractWithClient, clientSocket, std::ref(clients));
		t1.detach();
	}
	closesocket(listenSocket);

	WSACleanup();
	return 0;
}
