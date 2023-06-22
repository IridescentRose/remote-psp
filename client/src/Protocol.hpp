#pragma once
#include <cstdint>

enum PacketID {
    HelloPing = 0,
    KeyEvent = 1,
    CursorEvent = 2,
};

struct HelloPingPacket {
    int32_t key = 0;
};

struct KeyEventPacket {
    int32_t key = 0;
    bool pressed = false;
    bool held = false;
};