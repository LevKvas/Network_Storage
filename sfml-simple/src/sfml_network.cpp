#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include "SFML/Network/TcpSocket.hpp"
#include "SFML/Network/TcpListener.hpp"

#pragma comment(lib, "ws2_32.lib")

// Глобальная инициализация WinSock
namespace {
    struct WSASingleton {
        WSASingleton() {
            WSADATA wsa;
            WSAStartup(MAKEWORD(2,2), &wsa);
        }
        ~WSASingleton() { WSACleanup(); }
    } wsaInit;
}

// Реализация TcpSocket
sf::TcpSocket::TcpSocket() : m_handle(-1) {}
sf::TcpSocket::~TcpSocket() { disconnect(); }

sf::SocketStatus sf::TcpSocket::connect(const std::string& host, unsigned short port, Time) {
    if (m_handle != -1) disconnect();

    m_handle = socket(AF_INET, SOCK_STREAM, 0);
    if (m_handle == INVALID_SOCKET) return Error;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    // Преобразуем строку в IP-адрес
    if (host == "localhost" || host == "127.0.0.1") {
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    } else {
        addr.sin_addr.s_addr = inet_addr(host.c_str());
        if (addr.sin_addr.s_addr == INADDR_NONE) {
            // Попробуем resolve через DNS
            hostent* he = gethostbyname(host.c_str());
            if (he) {
                addr.sin_addr = *((in_addr*)he->h_addr);
            } else {
                disconnect();
                return Error;
            }
        }
    }

    if (::connect(m_handle, (sockaddr*)&addr, sizeof(addr)) == 0) {
        return Done;
    }

    disconnect();
    return Error;
}

sf::SocketStatus sf::TcpSocket::send(const void* data, std::size_t size) {
    if (m_handle == -1) return Error;

    int sent = ::send(m_handle, (const char*)data, size, 0);
    if (sent == SOCKET_ERROR) return Error;
    return (std::size_t)sent == size ? Done : Partial;
}

sf::SocketStatus sf::TcpSocket::receive(void* data, std::size_t size, std::size_t& received) {
    if (m_handle == -1) return Error;

    int result = recv(m_handle, (char*)data, size, 0);
    if (result > 0) {
        received = result;
        return Done;
    } else if (result == 0) {
        return Disconnected;
    }
    return Error;
}

void sf::TcpSocket::disconnect() {
    if (m_handle != -1) {
        closesocket(m_handle);
        m_handle = -1;
    }
}

unsigned short sf::TcpSocket::getLocalPort() const {
    if (m_handle == -1) return 0;
    sockaddr_in addr{};
    int addrLen = sizeof(addr);
    getsockname(m_handle, (sockaddr*)&addr, &addrLen);
    return ntohs(addr.sin_port);
}

// Реализация TcpListener
sf::TcpListener::TcpListener() : m_handle(-1) {}
sf::TcpListener::~TcpListener() { close(); }

sf::SocketStatus sf::TcpListener::listen(unsigned short port) {
    if (m_handle != -1) close();

    m_handle = socket(AF_INET, SOCK_STREAM, 0);
    if (m_handle == INVALID_SOCKET) return Error;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    // Позволяем повторно использовать порт
    int yes = 1;
    setsockopt(m_handle, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));

    if (bind(m_handle, (sockaddr*)&addr, sizeof(addr)) != 0) {
        close();
        return Error;
    }

    if (::listen(m_handle, SOMAXCONN) != 0) {
        close();
        return Error;
    }

    return Done;
}

sf::SocketStatus sf::TcpListener::accept(TcpSocket& socket) {
    if (m_handle == -1) return Error;

    sockaddr_in clientAddr{};
    int addrLen = sizeof(clientAddr);
    SOCKET client = ::accept(m_handle, (sockaddr*)&clientAddr, &addrLen);

    if (client == INVALID_SOCKET) return Error;

    // Закрываем старый сокет клиента если был открыт
    socket.disconnect();

    // Прямой доступ к приватному полю (некрасиво, но работает)
    socket.setHandle(client);
    return Done;
}

void sf::TcpListener::close() {
    if (m_handle != -1) {
        ::closesocket(m_handle);
        m_handle = -1;
    }
}