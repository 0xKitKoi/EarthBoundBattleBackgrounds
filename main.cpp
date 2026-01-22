#include <SDL.h>
#include <vector>
#include <cstdint>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <memory>
#include "rom/Rom.h"
#include "rom/BackgroundLayer.h"

constexpr int SNES_WIDTH  = 256;
constexpr int SNES_HEIGHT = 224;
constexpr int WINDOW_SCALE = 3;
constexpr int FPS = 30;
constexpr int FRAME_TIME_MS = 1000 / FPS;

// Helper function to parse integer arguments with defaults
int parseIntArg(const char* arg, int defaultValue, int minVal, int maxVal) {
    if (!arg) return defaultValue;
    int val = std::stoi(arg);
    if (val < minVal || val > maxVal) return defaultValue;
    return val;
}

int main(int argc, char** argv)
{

    if (argc < 2) {
        std::cerr << "Usage: ebbg <path-to-rom> [layer1] [layer2] [frameskip] [aspectRatio]\n";
        std::cerr << "  layer1: 0-326 (default: 270)\n";
        std::cerr << "  layer2: 0-326 (default: 269)\n";
        std::cerr << "  frameskip: 1-10 (default: 1)\n";
        std::cerr << "  aspectRatio: 0, 16, 48, 64 (default: 0)\n";
        return 1;
    }

    const std::string romPath = argv[1];
    
    // Parse optional parameters
    int layer1 = parseIntArg(argc > 2 ? argv[2] : nullptr, 270, 0, 326);
    int layer2 = parseIntArg(argc > 3 ? argv[3] : nullptr, 269, 0, 326);
    int frameskip = parseIntArg(argc > 4 ? argv[4] : nullptr, 1, 1, 10);
    int aspectRatio = argc > 5 ? std::stoi(argv[5]) : 0;
    
    // Validate aspect ratio
    if (aspectRatio != 0 && aspectRatio != 16 && aspectRatio != 48 && aspectRatio != 64) {
        aspectRatio = 0;
    }
    
    // Check for debug flag (dump 3 frames to PPM)
    bool dumpFrames = (argc > 6 && std::string(argv[6]) == "--dump");
    
    std::cout << "Loading with layer1=" << layer1 << " layer2=" << layer2 
              << " frameskip=" << frameskip << " aspectRatio=" << aspectRatio << "\n";
    if (!ROM::loadFromFile(romPath)) {
        std::cerr << "Failed to load ROM: " << romPath << "\n";
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "EarthBound Background Test",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SNES_WIDTH * WINDOW_SCALE,
        SNES_HEIGHT * WINDOW_SCALE, 
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_RenderSetLogicalSize(renderer, SNES_WIDTH, SNES_HEIGHT);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SNES_WIDTH,
        SNES_HEIGHT
    );

    if (!texture) {
        std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << "\n";
        return 1;
    }

    std::vector<uint8_t> framebuffer(SNES_WIDTH * SNES_HEIGHT * 4);

    // Create background layers using the unified BackgroundLayer class
    std::shared_ptr<BackgroundLayer> bgLayer1;
    std::shared_ptr<BackgroundLayer> bgLayer2;
    
    try {
        bgLayer1 = std::make_shared<BackgroundLayer>(layer1);
        if (!bgLayer1->isValid()) {
            std::cerr << "Error: Failed to load layer1 (" << layer1 << ")\n";
            return 1;
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: Failed to load layer1 (" << layer1 << "): " << e.what() << "\n";
        return 1;
    }

    // Set alpha blending based on whether we have 2 layers
    float alpha1 = 1.0f;
    float alpha2 = 0.0f;
    
    if (layer2 != 0) {
        try {
            bgLayer2 = std::make_shared<BackgroundLayer>(layer2);
            if (bgLayer2->isValid()) {
                alpha1 = 0.5f;
                alpha2 = 0.5f;
            } else {
                std::cerr << "Warning: Failed to load layer2 (" << layer2 << ")\n";
                std::cerr << "Continuing with layer1 only\n";
                layer2 = 0;
                alpha1 = 1.0f;
            }
        } catch (const std::exception &e) {
            std::cerr << "Warning: Failed to load layer2 (" << layer2 << "): " << e.what() << "\n";
            std::cerr << "Continuing with layer1 only\n";
            layer2 = 0;
            alpha1 = 1.0f;
        }
    }

    bool running = true;
    uint32_t tick = 0;
    uint32_t frameCount = 0;

    while (running) {
        uint32_t frameStart = SDL_GetTicks();

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }

        // Clear framebuffer
        memset(framebuffer.data(), 0, framebuffer.size());

        // Apply layer 1 distortion (without letterbox yet, erase=true for first layer)
        bgLayer1->overlayFrame(framebuffer.data(), SNES_WIDTH, SNES_HEIGHT, 0, 
                               static_cast<float>(tick), alpha1, true);

        // Apply layer 2 distortion if present (erase=false to blend)
        if (bgLayer2 && bgLayer2->isValid()) {
            bgLayer2->overlayFrame(framebuffer.data(), SNES_WIDTH, SNES_HEIGHT, 0, 
                                   static_cast<float>(tick), alpha2, false);
        }

        // Apply letterbox AFTER distortion (just fill the edge pixels)
        if (aspectRatio > 0) {
            for (int y = 0; y < aspectRatio; ++y) {
                for (int x = 0; x < SNES_WIDTH; ++x) {
                    // Top letterbox
                    int idx = (y * SNES_WIDTH + x) * 4;
                    framebuffer[idx + 0] = 0;      // B
                    framebuffer[idx + 1] = 0;      // G
                    framebuffer[idx + 2] = 0;      // R
                    framebuffer[idx + 3] = 255;    // A
                    
                    // Bottom letterbox
                    idx = ((SNES_HEIGHT - 1 - y) * SNES_WIDTH + x) * 4;
                    framebuffer[idx + 0] = 0;      // B
                    framebuffer[idx + 1] = 0;      // G
                    framebuffer[idx + 2] = 0;      // R
                    framebuffer[idx + 3] = 255;    // A
                }
            }
        }

        // Output sample frames as PPM if --dump flag specified
        if (dumpFrames && (frameCount == 0 || frameCount == 15 || frameCount == 29)) {
            char filename[64];
            snprintf(filename, sizeof(filename), "frame_%03u.ppm", frameCount);
            FILE *f = fopen(filename, "wb");
            if (f) {
                fprintf(f, "P6\n%d %d\n255\n", SNES_WIDTH, SNES_HEIGHT);
                for (int i = 0; i < SNES_WIDTH * SNES_HEIGHT; ++i) {
                    // Framebuffer is [B, G, R, A] format
                    uint8_t b = framebuffer[i * 4 + 0];
                    uint8_t g = framebuffer[i * 4 + 1];
                    uint8_t r = framebuffer[i * 4 + 2];
                    fwrite(&r, 1, 1, f);
                    fwrite(&g, 1, 1, f);
                    fwrite(&b, 1, 1, f);
                }
                fclose(f);
                std::cout << "Wrote " << filename << " at tick=" << tick << "\n";
            }
        }

        SDL_UpdateTexture(texture, nullptr, framebuffer.data(), SNES_WIDTH * 4);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        tick += frameskip;
        frameCount++;

        // Stop after 30 frames if dumping frames
        if (dumpFrames && frameCount >= 30) {
            running = false;
        }

        uint32_t frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_TIME_MS) {
            SDL_Delay(FRAME_TIME_MS - frameTime);
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // Shared pointers handle cleanup automatically

    return 0;
}
