#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

constexpr int PORT = 8080;

std::vector<int> active_clients;
std::mutex client_mutex; // Protects the active_clients vector from race conditions

// Safely send a message to all connected clients except the sender
void broadcast(const std::string& message, int sender_fd) {
    std::lock_guard<std::mutex> lock(client_mutex);
    for (int client_fd : active_clients) {
        if (client_fd != sender_fd) {
            send(client_fd, message.c_str(), message.length(), 0);
        }
    }
}

// The infinite loop that runs on a separate thread for EVERY connected client
void handle_client(int client_fd) {
    char buffer[1024];
    std::string welcome = "New user joined the server.\n";
    broadcast(welcome, client_fd);

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        
        // If recv returns 0, the client disconnected cleanly. If < 0, an error occurred.
        if (bytes_received <= 0) {
            std::cout << "Client disconnected (FD: " << client_fd << ")" << std::endl;
            close(client_fd);
            
            // Safely remove the client from our active list
            std::lock_guard<std::mutex> lock(client_mutex);
            active_clients.erase(std::remove(active_clients.begin(), active_clients.end(), client_fd), active_clients.end());
            
            broadcast("A user left the server.\n", -1);
            break;
        }
        
        std::string msg(buffer, bytes_received);
        std::cout << "Received from FD " << client_fd << ": " << msg;
        broadcast(msg, client_fd);
    }
}

int main() {
    // 1. Create a raw TCP/IPv4 socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return -1;
    }

    // 2. Allow the port to be reused immediately after the server closes
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 3. Bind the socket to port 8080
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed. Is port " << PORT << " in use?" << std::endl;
        return -1;
    }

    // 4. Start listening for incoming connections
    if (listen(server_fd, 10) < 0) {
        std::cerr << "Listen failed." << std::endl;
        return -1;
    }

    std::cout << "🚀 TCP Relay Server listening on port " << PORT << "..." << std::endl;

    // 5. Infinite loop to accept new clients
    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_fd < 0) {
            std::cerr << "Failed to accept connection." << std::endl;
            continue;
        }

        std::cout << "New connection accepted (FD: " << client_fd << ")" << std::endl;

        // Add client to our list safely
        {
            std::lock_guard<std::mutex> lock(client_mutex);
            active_clients.push_back(client_fd);
        }

        // Spawn a detached thread to handle this specific client
        std::thread client_thread(handle_client, client_fd);
        client_thread.detach(); 
    }

    close(server_fd);
    return 0;
}
