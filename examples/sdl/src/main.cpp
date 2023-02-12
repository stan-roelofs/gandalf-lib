#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>

#include <SDL.h>

#include <gandalf/gameboy.h>

#include "audio_handler.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;
std::unique_ptr<gandalf::Gameboy> gameboy;
std::unique_ptr<gandalf::LCD::VideoBuffer> front_buffer, back_buffer;
std::thread gameboy_thread;

static bool run_gb = false;

void GameboyThread()
{
    while (run_gb)
        gameboy->Run();
}

// Sets up SDL and creates a window, nothing special here
static bool InitializeSDL()
{
    front_buffer = std::make_unique<gandalf::LCD::VideoBuffer>();
    back_buffer = std::make_unique<gandalf::LCD::VideoBuffer>();
    front_buffer->fill(0);
    back_buffer->fill(0);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cout << "failed to init: " << SDL_GetError();
        return false;
    }
    window = SDL_CreateWindow(
        "Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gandalf::kScreenWidth * 4, gandalf::kScreenHeight * 4,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );
    if (window == NULL) {
        std::cout << "Failed to create window: " << SDL_GetError();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL)
    {
        std::cout << "Failed to create renderer: " << SDL_GetError();
        return false;
    }

    // The gameboy stores its pixels in a 15-bit color format, so we need to use SDL_PIXELFORMAT_ABGR1555 here
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR1555, SDL_TEXTUREACCESS_STREAMING, gandalf::kScreenWidth, gandalf::kScreenHeight);
    if (texture == NULL)
    {
        std::cout << "Failed to create texture: " << SDL_GetError();
        return false;
    }
}

// The VBlank interrupt is called when the gameboy is done drawing the current frame.
// We implement the VBlankListener interface to receive this interrupt and copy the
// gameboy's video buffer to our own buffer.
struct: gandalf::PPU::VBlankListener
{
    void OnVBlank() override
    {
        const auto& framebuffer = gameboy->GetLCD().GetVideoBuffer();
        std::swap(front_buffer, back_buffer);
        std::copy(framebuffer.begin(), framebuffer.end(), back_buffer->begin());
    }
} vblank_listener;

static bool InitializeGameboy(const std::string& path)
{
    const std::filesystem::path rom_path(path);
    if (!std::filesystem::exists(rom_path))
    {
        std::cout << "File not found: " << path << std::endl;
        return false;
    }

    // Load ROM file
    std::ifstream input(rom_path, std::ios::binary);
    std::vector<gandalf::byte> rom_data = std::vector<gandalf::byte>(std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>());

    // Create gameboy instance and load the ROM
    gameboy = std::make_unique<gandalf::Gameboy>(gandalf::Model::CGB);
    if (!gameboy->LoadROM(rom_data))
    {
        std::cout << "Failed to load ROM file: " << path << std::endl;
        return false;
    }

    // The gameboy will call our listener when it's ready to draw a frame, we will use this to update our screen buffer
    gameboy->AddVBlankListener(&vblank_listener);
    gameboy->SetAudioHandler(std::make_unique<SDLAudioHandler>(run_gb));
    gameboy_thread = std::thread(GameboyThread);
    run_gb = true;
    return true;
}

static void CleanupSDL()
{
    if (renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (window != NULL)
    {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    SDL_Quit();
}

// Forwards key presses to the gameboy
static void HandleKey(SDL_Keycode key, bool pressed)
{
    switch (key)
    {
    case SDLK_UP:
        gameboy->SetButtonState(gandalf::Joypad::kUp, pressed);
        break;
    case SDLK_DOWN:
        gameboy->SetButtonState(gandalf::Joypad::kDown, pressed);
        break;
    case SDLK_LEFT:
        gameboy->SetButtonState(gandalf::Joypad::kLeft, pressed);
        break;
    case SDLK_RIGHT:
        gameboy->SetButtonState(gandalf::Joypad::kRight, pressed);
        break;
    case SDLK_z:
        gameboy->SetButtonState(gandalf::Joypad::kA, pressed);
        break;
    case SDLK_x:
        gameboy->SetButtonState(gandalf::Joypad::kB, pressed);
        break;
    case SDLK_RETURN:
        gameboy->SetButtonState(gandalf::Joypad::kStart, pressed);
        break;
    case SDLK_TAB:
        gameboy->SetButtonState(gandalf::Joypad::kSelect, pressed);
        break;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: %s <rom file>" << argv[0] << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Controls: " << std::endl;
    std::cout << "  Arrow keys: D-Pad" << std::endl;
    std::cout << "  Z: A" << std::endl;
    std::cout << "  X: B" << std::endl;
    std::cout << "  Enter: Start" << std::endl;
    std::cout << "  Tab: Select" << std::endl;
    std::cout << "  Esc: Quit" << std::endl;

    if (!InitializeSDL())
        return EXIT_FAILURE;


    if (!InitializeGameboy(argv[1]))
        return EXIT_FAILURE;

    // ----------- Render loop  -------------
    SDL_bool quit = SDL_FALSE;
    while (!quit)
    {
        SDL_Event e;

        // we need to call SDL_PollEvent to let window rendered, otherwise
        // no window will be shown
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
            {
                quit = SDL_TRUE;
                run_gb = false;
            }

            if (e.type == SDL_KEYDOWN)
            {
                HandleKey(e.key.keysym.sym, true);
            }
            else if (e.type == SDL_KEYUP)
            {
                HandleKey(e.key.keysym.sym, false);
            }
        }

        SDL_UpdateTexture(texture, NULL, front_buffer->data(), gandalf::kScreenWidth * sizeof(gandalf::LCD::ABGR1555));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    gameboy_thread.join();

    return EXIT_SUCCESS;
}
