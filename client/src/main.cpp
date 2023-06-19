#include <Stardust-Celeste.hpp>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#ifdef _WIN32
extern "C" {
    __declspec(dllexport) DWORD NvOptimusEnablement = 1;
    __declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;
}
#endif

using namespace Stardust_Celeste;

class RemoteState : public Core::ApplicationState {
    public:
        void on_start() override {
            SC_APP_INFO("Showing main menu");
            if(!Network::NetworkDriver::initGUI())
                throw std::runtime_error("Invalid Network Configuration!");

            std::cout << "Starting Socket" << std::endl;
            sck = socket(AF_INET, SOCK_STREAM, 0);
            if(sck == -1) {
                perror("Socket");
                throw std::runtime_error("Couldn't make socket!");
            }

            struct sockaddr_in name {};
            name.sin_family = AF_INET;
            name.sin_port = htons(3000);
            name.sin_addr.s_addr = inet_addr("192.168.54.142");

            std::cout << "Connecting" << std::endl;
            if(connect(sck, (struct sockaddr*)&name, sizeof(name)) == -1) {
                perror("Error");
                throw std::runtime_error("Could not connect to host! (Is the server open?)");
            }

            std::cout << "Sending Message" << std::endl;
            char message[] = "Hello, server!";
            int message_len = strlen(message);

            // Send the message
            int bytes_sent = send(sck, message, message_len, 0);
            if (bytes_sent == -1) {
                perror("send");
            }
        }

        void on_update(Core::Application *app, double dt) override {
        }

        void on_draw(Core::Application *app, double dt) override {

        }

        void on_cleanup() override {

        }

private:
    int sck;
};

class GameApplication : public Core::Application {
public:
    void on_start() override {
        // Create new Game State
        auto state = create_refptr<RemoteState>();
        // Set to our state
        this->set_state(state);

        // Set the background color
        Rendering::Color color{};
        color.color = 0xFFFFFFFF;

        Rendering::RenderContext::get().vsync = false;
        Rendering::RenderContext::get().set_color(color);
    }
};


Core::Application *CreateNewSCApp() {
    // Configure the engine
    Core::AppConfig config;
    config.headless = false;
    config.networking = true;

    Core::PlatformLayer::get().initialize(config);

    // Return the game
    return new GameApplication();
}