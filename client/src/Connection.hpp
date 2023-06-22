#include <string>
#include <cstdint>
#include <Network/Packet.hpp>

using namespace Stardust_Celeste;
struct Connection {

    inline static auto get() -> Connection& {
        static Connection instance;
        return instance;
    }

    void connect(std::string ip, uint16_t port);
    size_t send(uint8_t id, RefPtr<Network::ByteBuffer> bbuf);

    int sck = -1;
    bool connected = false;
};
