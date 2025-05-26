#include <iostream>
#include <thread>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

#define PORT 12345

void receive_messages(int sock) {
    char buffer[1024];
    while (true) {
        ssize_t bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';
        cout << buffer << endl;
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));

    thread(receive_messages, sock).detach();

    string msg;
    while (getline(cin, msg)) {
        send(sock, msg.c_str(), msg.size(), 0);
    }
    close(sock);
    return 0;
}