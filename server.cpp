#include <iostream>
#include <string>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
// для компилирования - g++ server.cpp -lws2_32
#pragma comment(lib, "ws2_32.lib")

using namespace std;

string readHTML() {
    ifstream file("index.html");
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return content;
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    
    string last_button = "nothing";
    
    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);
    
    bind(server, (sockaddr*)&addr, sizeof(addr));
    listen(server, 1);
    
    cout << "server in http://localhost:8080" << endl;
    
    while (true) {
        SOCKET client = accept(server, NULL, NULL);
        char buffer[4096] = {0};
        recv(client, buffer, 4096, 0);
        
        string req(buffer);
        
        if (req.find("POST") != string::npos) {
            if (req.find("power_on") != string::npos) last_button = "On";
            else if (req.find("power_off") != string::npos) last_button = "Off";
            else if (req.find("cmd1") != string::npos) last_button = "grab";
            else if (req.find("cmd2") != string::npos) last_button = "release";
            
            cout << "press: " << last_button << endl;
            
            string resp = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n{\"last\":\"" + last_button + "\"}";
            send(client, resp.c_str(), resp.size(), 0);
        } 
        else {
            string html = readHTML();
            string resp = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + html;
            send(client, resp.c_str(), resp.size(), 0);
        }
        
        closesocket(client);
    }
    
    closesocket(server);
    WSACleanup();
    return 0;
}
