#include <Stardust-Celeste.hpp>
#include <GameState.hpp>
#include <Debug/OptimizerState.hpp>

#ifdef _WIN32
extern "C" {
    __declspec(dllexport) DWORD NvOptimusEnablement = 1;
    __declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;
}
#endif

using namespace Stardust_Celeste;

class RemoteState : public Core::RemoteState { 
    public:
        void on_start() override {
            Network::NetworkDriver::get().initGUI();
        }

        void on_update(Core::Application *app, double dt) override {

        }

        void on_draw(Core::Application *app, double dt) override {

        }

        void on_cleanup() override {

        }
}

class GameApplication : public Core::Application {
public:
    void on_start() override {
        // Create new Game State
        auto state = create_refptr<>();
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