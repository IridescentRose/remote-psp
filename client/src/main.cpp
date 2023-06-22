#include <Stardust-Celeste.hpp>
#include <Utilities/Input.hpp>
#include <Connection.hpp>
#include <Utilities/Controllers/PSPController.hpp>
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
    Hold
};

struct KeyInfo{
    int key = 0;
    int held = 0;
};

class RemoteState : public Core::ApplicationState {
    public:
        static auto KeyCMD(std::any p) -> void {
            auto state = std::any_cast<KeyInfo>(p);
            send_key_press(state.key, true, state.held);
        }

        static void send_hello_ping() {
            auto bbuf = create_refptr<Network::ByteBuffer>(sizeof(HelloPingPacket));
            bbuf->WriteI32(rand());
            Connection::get().send(PacketID::HelloPing, bbuf);
        }

        static void send_key_press(int key, bool pressed, bool held) {
            auto bbuf = create_refptr<Network::ByteBuffer>(sizeof(KeyEventPacket));
            bbuf->WriteI32(key);
            bbuf->WriteI8(pressed);
            bbuf->WriteI8(held);
            printf("Sending key press: %d, %d, %d\n", key, pressed, held);
            Connection::get().send(PacketID::KeyEvent, bbuf);
        }

        void on_start() override {
            using namespace Utilities;
            pspc = new Input::PSPController();
            pspc->add_command({(int)Input::PSPButtons::Circle, KeyFlag::Press}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Circle, Pressed}});
            pspc->add_command({(int)Input::PSPButtons::Circle, KeyFlag::Held}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Circle, Held}});
            pspc->add_command({(int)Input::PSPButtons::Triangle, KeyFlag::Press}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Triangle, Pressed}});
            pspc->add_command({(int)Input::PSPButtons::Triangle, KeyFlag::Held}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Triangle, Held}});
            pspc->add_command({(int)Input::PSPButtons::Cross, KeyFlag::Press}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Cross, Pressed}});
            pspc->add_command({(int)Input::PSPButtons::Cross, KeyFlag::Held}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Cross, Held}});
            pspc->add_command({(int)Input::PSPButtons::Square, KeyFlag::Press}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Square, Pressed}});
            pspc->add_command({(int)Input::PSPButtons::Square, KeyFlag::Held}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Square, Held}});

            pspc->add_command({(int)Input::PSPButtons::Up, KeyFlag::Press}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Up, Pressed}});
            pspc->add_command({(int)Input::PSPButtons::Up, KeyFlag::Held}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Up, Held}});
            pspc->add_command({(int)Input::PSPButtons::Down, KeyFlag::Press}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Down, Pressed}});
            pspc->add_command({(int)Input::PSPButtons::Down, KeyFlag::Held}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Down, Held}});
            pspc->add_command({(int)Input::PSPButtons::Left, KeyFlag::Press}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Left, Pressed}});
            pspc->add_command({(int)Input::PSPButtons::Left, KeyFlag::Held}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Left, Held}});
            pspc->add_command({(int)Input::PSPButtons::Right, KeyFlag::Press}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Right, Pressed}});
            pspc->add_command({(int)Input::PSPButtons::Right, KeyFlag::Held}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Right, Held}});

            pspc->add_command({(int)Input::PSPButtons::Select, KeyFlag::Press}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Select, Pressed}});
            pspc->add_command({(int)Input::PSPButtons::Select, KeyFlag::Held}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Select, Held}});
            pspc->add_command({(int)Input::PSPButtons::Start, KeyFlag::Press}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Start, Pressed}});
            pspc->add_command({(int)Input::PSPButtons::Start, KeyFlag::Held}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::Start, Held}});
            pspc->add_command({(int)Input::PSPButtons::LTrigger, KeyFlag::Press}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::LTrigger, Pressed}});
            pspc->add_command({(int)Input::PSPButtons::LTrigger, KeyFlag::Held}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::LTrigger, Held}});
            pspc->add_command({(int)Input::PSPButtons::RTrigger, KeyFlag::Press}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::RTrigger, Pressed}});
            pspc->add_command({(int)Input::PSPButtons::RTrigger, KeyFlag::Held}, {KeyCMD, KeyInfo{(int)Input::PSPButtons::RTrigger, Held}});

            Input::add_controller(pspc);

#if BUILD_PLAT == BUILD_PSP
            if(!Network::NetworkDriver::initGUI())
                throw std::runtime_error("Invalid Network Configuration!");
#endif
            srand(time(NULL));

            std::ifstream file("ip.txt");
            std::string ip;
            std::getline(file, ip);
            file.close();

            Connection::get().connect(ip, 3000);
            send_hello_ping();

            timer = 0.0f;
        }

        void on_update(Core::Application *app, double dt) override {
            timer += dt;

            if(timer > 1.0f / 30.0f) {
                Utilities::Input::update();
                timer = 0;
                counter++;
                if(counter > 30) {
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
        Utilities::Controller* pspc;
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