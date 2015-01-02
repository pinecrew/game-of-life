#include <cstdio>
#include <cstdlib>
#include <SDL2/SDL.h>
#include "draw.hpp"

const char * game_name = "Conway's Game of Life";
const int screen_width = 640;
const int screen_height = 480;
bool quit_flag = false;

SDL_Window * window = NULL;
SDL_Renderer * render = NULL;
SDL_Event event;

void game_send_error( int code ) {
    printf( "[error]: %s\n", SDL_GetError() );
    exit( code );
}

void game_event( SDL_Event *event ) {
    SDL_PollEvent( event );
    switch ( event->type ) {
        case SDL_QUIT:
            quit_flag = true;
            break;
        default:
            break;
    }
}

void game_loop( void ) {
    // insert code
}

void game_render( void ) {
    SDL_RenderClear( render );
    // render code
    SDL_RenderPresent( render );
}

void game_destroy( void ) {
    // insert code
    SDL_DestroyRenderer( render );
    SDL_DestroyWindow( window );
    SDL_Quit();
}

void game_init( void ) {
    window = SDL_CreateWindow( game_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                               screen_width, screen_height, SDL_WINDOW_SHOWN );
    if ( window == NULL ) {
        game_send_error( EXIT_FAILURE );
    }
    render = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if ( render == NULL ) {
        game_send_error( EXIT_FAILURE );
    }
    draw_init( render );
}

int main( int argc, char * argv[] ) {
    Uint32 FPS_MAX = 1000 / 63; // ~ 60 fps

    game_init();
    while ( quit_flag == false ) {
        game_event( &event );
        game_loop();
        game_render();
        SDL_Delay( FPS_MAX );
    }
    game_destroy();
    return EXIT_SUCCESS;
}