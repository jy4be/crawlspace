#include "crsp.h"

window_data_t CRSP_window_data;

screen_t CRSP_screen;
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

void present() {
    SDL_UpdateTexture(CRSP_window_data.texture, NULL, &CRSP_screen.bytes, CRSP_window_data.width * 3);
        SDL_RenderCopyEx(
            CRSP_window_data.renderer,
            CRSP_window_data.texture,
            NULL,
            NULL,
            0.0,
            NULL,
            0);
        SDL_RenderPresent(CRSP_window_data.renderer);

        SDL_Delay(16);
}


void cleanup() {
    SDL_DestroyTexture(CRSP_window_data.texture);
    SDL_DestroyRenderer(CRSP_window_data.renderer);
    SDL_DestroyWindow(CRSP_window_data.window);
    destroy_textures();
}
