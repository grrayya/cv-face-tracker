#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <csignal>
#include <chrono>

const int PORT = 8080;
const size_t MAX_USERS = 100;

std::vector<int> clients;
std::mutex mtx;
volatile sig_atomic_t running = 1;

void handle_sig(int) {
    running = 0;
}

void broadcast(const std::string& msg, int sender) {
    std::vector<int> targets;
    {
        std::lock_guard<std::mutex> lock(mtx);
        targets = clients;
    }

    for (int fd : targets) {
        if (fd != sender) {
            if (send(fd, msg.c_str(), msg.length(), MSG_NOSIGNAL) < 0) {
                std::cerr << "Send failed on FD " << fd << "\n";
            }
        }
    }
}

void handle_client(int fd) {
    char buf[1024];
    broadcast("New user joined.\n", fd);

    while (running) {
        int bytes = recv(fd, buf, sizeof(buf), 0);
        
        if (bytes <= 0) {
            close(fd);
            {
                std::lock_guard<std::mutex> lock(mtx);
                clients.erase(std::remove(clients.begin(), clients.end(), fd), clients.end());
            }
            broadcast("User left.\n", -1);
            break;
        }
        
        // TCP is a byte stream. For production, buffer until '\n' is found.
        broadcast(std::string(buf, bytes), fd);
    }
}

int main() {
    signal(SIGINT, handle_sig);
    signal(SIGTERM, handle_sig);

    int server = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    listen(server, 10);

    std::cout << "Listening on " << PORT << "...\n";

    while (running) {
        int client = accept(server, nullptr, nullptr);
        
        if (client < 0) {
            if (!running) break; 
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            if (clients.size() >= MAX_USERS) {
                close(client);
                continue;
            }
            clients.push_back(client);
        }

        std::thread(handle_client, client).detach();
    }

    std::cout << "\nShutting down...\n";
    close(server);
    for (int fd : clients) close(fd);
    
    return 0;
}
