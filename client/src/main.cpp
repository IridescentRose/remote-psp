#include <Stardust-Celeste.hpp>
#include <Utilities/Input.hpp>
#include <Connection.hpp>
#include <Utilities/Controllers/KeyboardController.hpp>
#include <Protocol.hpp>

#ifdef _WIN32
extern "C" {
    __declspec(dllexport) DWORD NvOptimusEnablement = 1;
    __declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;
}
#endif

using namespace Stardust_Celeste;

enum KeyState {
    Pressed,
    Hold,
    Released
};

class RemoteState : public Core::ApplicationState {
    public:
        static auto KeyCrossP(std::any p) -> void {
            auto state = std::any_cast<KeyState>(p);
            switch(state) {
                case Pressed:
                    std::cout << "Pressed" << std::endl;
                    break;
                case Hold:
                    std::cout << "Held" << std::endl;
                    break;
                case Released:
                    std::cout << "Released" << std::endl;
                    break;
            }
        }

        void send_hello_ping() {
            auto bbuf = create_refptr<Network::ByteBuffer>(sizeof(HelloPingPacket));
            bbuf->WriteI32(rand());
            Connection::get().send(PacketID::HelloPing, bbuf);

            int val;
            bbuf->ReadI32(val);
            printf("%X\n", val);
        }

        void on_start() override {
            using namespace Utilities;
            kb = new Input::KeyboardController();
            kb->add_command({(int)Input::Keys::X, KeyFlag::Press}, {KeyCrossP, Pressed});
            kb->add_command({(int)Input::Keys::X, KeyFlag::Held}, {KeyCrossP, Hold});
            Input::add_controller(kb);

#if BUILD_PLAT == BUILD_PSP
            if(!Network::NetworkDriver::initGUI())
                throw std::runtime_error("Invalid Network Configuration!");
#endif
            srand(time(NULL));

            Connection::get().connect("127.0.0.1", 3000);
            send_hello_ping();
        }

        void on_update(Core::Application *app, double dt) override {
            timer += dt;

            if(timer > 0.05) {
                Utilities::Input::update();
                timer = 0;
                counter++;
                if(counter > 20) {
                    send_hello_ping();
                    counter = 0;
                }
            }
        }

        void on_draw(Core::Application *app, double dt) override {

        }

        void on_cleanup() override {

        }

    private:
        Utilities::Input::KeyboardController* kb;
        double timer;
        int counter = 0;
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

        Rendering::RenderContext::get().vsync = true;
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