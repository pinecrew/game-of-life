#pragma once
#include <SDL2/SDL.h>

enum {
    COLOR_WHITE     = 0xffffff,
    COLOR_BLACK     = 0x000000,
    COLOR_RED       = 0xff0000,
    COLOR_GREEN     = 0x00ff00,
    COLOR_BLUE      = 0x0000ff,
    COLOR_YELLOW    = 0xffff00, 
};

void draw_init( SDL_Renderer * render );
void draw_rectangle_param( int x, int y, int w, int h, Uint32 color, bool param );
void draw_rectangle_outline( int x, int y, int w, int h, Uint32 color );
void draw_rectangle_fill( int x, int y, int w, int h, Uint32 color );
void draw_pixel_size( int x, int y, int size, Uint32 color );
