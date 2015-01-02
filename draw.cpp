#include "draw.hpp"

SDL_Renderer * _render = NULL;

void draw_init( SDL_Renderer * render ) {
    _render = render;
}

void draw_rectangle_param( int x, int y, int w, int h, Uint32 color, bool param ) {
    SDL_Rect rect = { x, y, w, h };
    Uint8 r, ro, g, go, b, bo, ao;
    
    r = ( color >> 16 );
    g = ( ( color >> 8 ) & 0xff );
    b = ( color & 0xff );
    SDL_GetRenderDrawColor( _render, &ro, &go, &bo, &ao );
    SDL_SetRenderDrawColor( _render, r, g, b, 0xff );
    if ( param ) {
        SDL_RenderFillRect( _render, &rect );
    } else {
        SDL_RenderDrawRect( _render, &rect );
        SDL_RenderDrawPoint( _render, x + w - 1, y + h - 1 );
    }
    SDL_SetRenderDrawColor( _render, ro, bo, go, ao );   
}

void draw_rectangle_outline( int x, int y, int w, int h, Uint32 color ) {
    draw_rectangle_param( x, y, w, h, color, false );
}

void draw_rectangle_fill( int x, int y, int w, int h, Uint32 color ) {
    draw_rectangle_param( x, y, w, h, color, true ); 
}

void draw_pixel_size( int x, int y, int size, Uint32 color ) {
    draw_rectangle_fill( x, y, size, size, color );
}