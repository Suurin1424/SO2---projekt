#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <algorithm>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

#define PORT 12345
#define MAX_CLIENTS 10

vector<int> clients;
mutex clients_mutex;

void broadcast_message(const string& msg, int sender_sock) {
    lock_guard<mutex> lock(clients_mutex);
    for (int client_sock : clients) {
        if (client_sock != sender_sock) {
            send(client_sock, msg.c_str(), msg.size(), 0);
        }
    }
}

void handle_client(int client_sock) {
    char buffer[1024];
    while (true) {
        ssize_t bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            break;
        }
        buffer[bytes_received] = '\0';
        string msg = "Client " + to_string(client_sock) + ": " + buffer;
        broadcast_message(msg, client_sock);
    }
    {
        lock_guard<mutex> lock(clients_mutex);
        clients.erase(remove(clients.begin(), clients.end(), client_sock), clients.end());
    }
    close(client_sock);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, MAX_CLIENTS);

    cout << "Server started on port " << PORT << endl;

    while (true) {
        int client_sock = accept(server_fd, nullptr, nullptr);
        if (client_sock < 0) continue;
        {
            lock_guard<mutex> lock(clients_mutex);
            clients.push_back(client_sock);
        }
        thread(handle_client, client_sock).detach();
    }
    close(server_fd);
    return 0;
}
