#pragma once

#include "Socket.hpp"
#include <string>

namespace sf {

    class TcpSocket : public Socket {
        int m_handle;

    public:
        TcpSocket();
        ~TcpSocket();

        SocketStatus connect(const std::string& host, unsigned short port, Time timeout = Time::Zero);
        SocketStatus send(const void* data, std::size_t size);
        SocketStatus receive(void* data, std::size_t size, std::size_t& received);
        void disconnect();

        unsigned short getLocalPort() const;
        std::string getRemoteAddress() const;
        unsigned short getRemotePort() const;

        void setHandle(int handle) { m_handle = handle; }

    private:
        // Скрытая реализация
    };

} // namespace sf