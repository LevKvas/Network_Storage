#pragma once

#include "../System.hpp"

namespace sf {

    enum SocketStatus {
        Done,           // Операция завершилась успешно
        NotReady,       // Сокет не готов
        Partial,        // Отправлены не все данные
        Disconnected,   // Сокет отключён
        Error           // Ошибка
    };

    class Socket {
    protected:
        virtual ~Socket() {}
    };

} // namespace sf