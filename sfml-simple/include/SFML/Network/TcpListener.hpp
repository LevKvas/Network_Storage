#pragma once

#include "TcpSocket.hpp"

namespace sf {

    class TcpListener : public Socket {
        int m_handle;

    public:
        TcpListener();
        ~TcpListener();

        SocketStatus listen(unsigned short port);
        SocketStatus accept(TcpSocket& socket);
        void close();

    private:
        // Скрытая реализация
    };

} // namespace sf