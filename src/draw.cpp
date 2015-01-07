#include "draw.hpp"

SDL_Renderer * _render = NULL;

void draw_init( SDL_Renderer * render ) {
    _render = render;
}

void set_coloru( Uint32 color ) {
    Uint8 r, ro, g, go, b, bo, ao;

    r = ( color >> 16 );
    g = ( ( color >> 8 ) & 0xff );
    b = ( color & 0xff );
    SDL_GetRenderDrawColor( _render, &ro, &go, &bo, &ao );
    SDL_SetRenderDrawColor( _render, r, g, b, 0xff );
}

void set_color3u( Uint8 red, Uint8 green, Uint8 blue ) {
    Uint8 ro, go, bo, ao;
    
    SDL_GetRenderDrawColor( _render, &ro, &go, &bo, &ao );
    SDL_SetRenderDrawColor( _render, red, green, blue, 0xff );
}

void set_color4u( Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha ) {
    Uint8 ro, go, bo, ao;
    
    SDL_GetRenderDrawColor( _render, &ro, &go, &bo, &ao );
    SDL_SetRenderDrawColor( _render, red, green, blue, alpha );
}

void draw_rectangle_param( int x, int y, int w, int h, bool fill ) {
    SDL_Rect rect = { x, y, w, h };

    if ( fill ) {
        SDL_RenderFillRect( _render, &rect );
    } else {
        SDL_RenderDrawRect( _render, &rect );
        SDL_RenderDrawPoint( _render, x + w - 1, y + h - 1 );
    }
}

void draw_pixel_size( point p, int size ) {
    draw_rectangle_param( p.first, p.second, size, size, true );
}
