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
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        cerr << "Socket creation error" << endl;
        WSACleanup();
        exit(1);
    }
    return sock;
}

//Mengkoneksi ke server
void connect_to_server(SOCKET sock, struct sockaddr_in &serv_addr){
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Connection Failed" << endl;
        closesocket(sock);
        WSACleanup();
        exit(1);
    }
    cout << "Connected to server" << endl;
}

//Menerima Data
string receive_data(SOCKET socket){
    char buffer[1024];
    int bytes_received = recv(socket, buffer, 1024, 0);
    if(bytes_received <= 0){
        cerr << "Gagal untuk menerima player dari server" << endl;
        closesocket(socket);
        WSACleanup();
        exit(1);
    }
    return string(buffer, bytes_received);
}

//Mengirim data
void send_data(SOCKET socket, const string &data){
    send(socket, data.c_str(), data.length(), 0);
}

int main() {
    WSADATA wsaData;
    SOCKET sock = 0;
    struct sockaddr_in serv_addr;
    
    initialize_winsock(wsaData);
    sock = create_socket();

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Mengkonversi alamat IPv4 dan IPv6 dari teks ke biner
    if (InetPton(AF_INET, "192.168.1.2", &serv_addr.sin_addr) <= 0) {
        cerr << "Invalid address/ Address not supported" << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    connect_to_server(sock, serv_addr);

    string player_msg = receive_data(sock);
    cout << "Player: " << player_msg << endl;

    string choice;
    cout << "Enter your choice (rock, paper, scissors): ";
    cin >> choice;

    send_data(sock, choice);
    cout << "Choice sent: " << choice << endl;

    string result = receive_data(sock);
    cout << "Result: " << result << endl;

    closesocket(sock);
    WSACleanup();

    cout << "Press any key to exit...";
    cin.get();
    cin.get();
    return 0;
}
