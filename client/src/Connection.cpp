#include <Connection.hpp>
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

void Connection::connect(std::string ip, uint16_t port) {
    std::cout << "Starting Socket" << std::endl;
    sck = socket(AF_INET, SOCK_STREAM, 0);
    if(sck == -1) {
        perror("Socket");
        throw std::runtime_error("Couldn't make socket!");
    }

    struct sockaddr_in name {};
    name.sin_family = AF_INET;
    name.sin_port = htons(port);
    name.sin_addr.s_addr = inet_addr(ip.c_str());

    std::cout << "Connecting" << std::endl;
    if(::connect(sck, (struct sockaddr*)&name, sizeof(name)) == -1) {
        perror("Error");
        throw std::runtime_error("Could not connect to host! (Is the server open?)");
    }

    connected = true;
}

size_t Connection::send(uint8_t id, RefPtr<Network::ByteBuffer> bbuf) {
    auto bbuf2 = new Network::ByteBuffer(bbuf->GetUsedSpace() + 5);
    bbuf2->WriteU8(id);
    bbuf2->WriteU32(bbuf->GetUsedSpace());
    bbuf2->Write(bbuf->m_Buffer, bbuf->GetUsedSpace());

    auto sent = ::send(sck, bbuf2->m_Buffer, bbuf2->GetUsedSpace(), 0);
    delete bbuf2;
    return sent;
}