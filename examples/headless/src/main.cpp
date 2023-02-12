#include <iostream>
#include <filesystem>
#include <fstream>

#include <gandalf/gameboy.h>

struct: public gandalf::PPU::VBlankListener
{
    void OnVBlank() override
    {
        ++gb_fps_;

        using namespace std::chrono;

        static high_resolution_clock::time_point t1 = high_resolution_clock::now();;

        high_resolution_clock::time_point t2 = high_resolution_clock::now();

        duration<double, std::milli> time_span = t2 - t1;

        if (time_span.count() > 1000) {
            t1 = t2;
            std::cout << "FPS: " << std::to_string(gb_fps_) << std::endl;
            gb_fps_ = 0;
        }
    }

    uint32_t gb_fps_ = 0;
} frame_logger;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: %s <rom file>" << argv[0] << std::endl;
        return EXIT_FAILURE;
    }

    const std::filesystem::path rom_path(argv[1]);
    if (!std::filesystem::exists(rom_path))
    {
        std::cout << "File not found: " << argv[1] << std::endl;
        return false;
    }

    // Load ROM file
    std::ifstream input(rom_path, std::ios::binary);
    std::vector<gandalf::byte> rom_data = std::vector<gandalf::byte>(std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>());

    std::unique_ptr<gandalf::Gameboy> gb = std::make_unique<gandalf::Gameboy>(gandalf::Model::DMG);
    if (!gb->LoadROM(rom_data))
    {
        std::cout << "Failed to load ROM file: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    // The gameboy will call our listener when it's ready to draw a frame, we will use this to log the FPS
    gb->AddVBlankListener(&frame_logger);

    while (true)
        gb->Run();

    return 0;
}