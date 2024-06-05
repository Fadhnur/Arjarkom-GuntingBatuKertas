#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h> // Untuk InetPton

#define PORT 8080

using namespace std;

// Inisialisasi Winsock
void initialize_winsock(WSADATA &wsaData){
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed" << endl;
        exit(1);
    }
}

// Membuat file descriptor socket
SOCKET create_socket(){
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        cerr << "Socket failed" << endl;
        WSACleanup();
        exit(1);
    }
    return server_fd;
}

// Mengikat socket ke alamat dan port
void bind_socket(SOCKET server_fd, struct sockaddr_in &address){
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        cerr << "Bind failed" << endl;
        closesocket(server_fd);
        WSACleanup();
        exit(1);
    }
}

//Mendengarkan listening masuk
void start_listening(SOCKET server_fd){
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        cerr << "Listen failed" << endl;
        closesocket(server_fd);
        WSACleanup();
        exit(1);
    }
    cout << "Server is listening..." << PORT << endl;
}

//Menerima koneksi dari player
SOCKET accept_connection(SOCKET server_fd, struct sockaddr_in &address, int &addrlen, const std::string &player_msg) {
    SOCKET new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    if (new_socket == INVALID_SOCKET) {
        std::cerr << "Accept failed" << std::endl;
        closesocket(server_fd);
        WSACleanup();
        exit(1);
    }
    std::cout << player_msg << " connected" << std::endl;
    send(new_socket, player_msg.c_str(), player_msg.length(), 0);
    return new_socket;
}

//Menerima pilihan dari player
string receive_data(SOCKET socket){
    char buffer[1024] = {0};
    int bytes_received = recv(socket, buffer, 1024, 0);
    if (bytes_received <= 0) {
        cerr << "Gagal menerima data" << endl;
        closesocket(socket);
        WSACleanup();
        exit(1);
    }
    return string(buffer, bytes_received);
}

//Mengirimkan data 
void send_data(SOCKET socket, const string &data){
    send(socket, data.c_str(), data.length(), 0);
}

//Menentukan pemenang
string determine_winner(const string &choice1, const string &choice2) {
    if (choice1 == choice2) {
        return "Draw";
    }
    if ((choice1 == "rock" && choice2 == "scissors") ||
        (choice1 == "scissors" && choice2 == "paper") ||
        (choice1 == "paper" && choice2 == "rock")) {
        return "Player 1 wins!";
    } else {
        return "Player 2 wins!";
    }
}

int main() {
    WSADATA wsaData;
    SOCKET server_fd, player1_socket, player2_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    initialize_winsock(wsaData);
    server_fd = create_socket();
    bind_socket(server_fd, address);
    start_listening(server_fd);

    player1_socket = accept_connection(server_fd, address, addrlen, "Player 1");
    player2_socket = accept_connection(server_fd, address, addrlen, "Player 2");

    string choice1 = receive_data(player1_socket);
    cout << "Player 1 chose: " << choice1 << endl;

    string choice2 = receive_data(player2_socket);
    cout << "Player 2 chose: " << choice2 << endl;

    string result = determine_winner(choice1, choice2);
    cout << "Result: " << result << endl;

    send_data(player1_socket, result);
    send_data(player2_socket, result);

    closesocket(player1_socket);
    closesocket(player2_socket);
    closesocket(server_fd);
    WSACleanup();

    cout << "Press any key to exit...";
    cin.get();

    return 0;
}
