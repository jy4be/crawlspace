#include "crsp.h"

void screen(u32 width, u32 height, char* name) {
    CRSP_window_data.width = width;
    CRSP_window_data.height = height;
    CRSP_window_data.name = name;
    CRSP_window_data.window_flags = WINDOW_FLAGS;
    CRSP_window_data.render_flags = RENDER_FLAGS;
    CRSP_screen = (union Screen_U) {0};

    DYN_ASSERT(!SDL_Init(SDL_INIT_VIDEO), 
               "Unable to initialize SDL, Diagnostic: %s", SDL_GetError());

    CRSP_window_data.window = 
        SDL_CreateWindow(
            CRSP_window_data.name, 
            SDL_WINDOWPOS_UNDEFINED, 
            SDL_WINDOWPOS_UNDEFINED,
            CRSP_window_data.width,
            CRSP_window_data.height,
            CRSP_window_data.window_flags);
    DYN_ASSERT(CRSP_window_data.window,
               "Unable to open SDL_Window, Diagostic: %s", SDL_GetError());

    CRSP_window_data.renderer =
        SDL_CreateRenderer(
                CRSP_window_data.window,
                -1,
                CRSP_window_data.render_flags);
    DYN_ASSERT(CRSP_window_data.renderer,
               "Unable to open SDL_Renderer, Diagostic: %s", SDL_GetError());

    CRSP_window_data.texture =
        SDL_CreateTexture(
                CRSP_window_data.renderer,
                SDL_PIXELFORMAT_RGB24,
                SDL_TEXTUREACCESS_STREAMING,
                width,
                height);
    DYN_ASSERT(CRSP_window_data.texture,
               "Unable to create rendering texture (SDL_Texture), Diagnostic: %s", SDL_GetError());
    CRSP_window_data.is_running = true;
}

void cleanup() {
    SDL_DestroyTexture(CRSP_window_data.texture);
    SDL_DestroyRenderer(CRSP_window_data.renderer);
    SDL_DestroyWindow(CRSP_window_data.window);
}
