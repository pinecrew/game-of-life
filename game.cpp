#include <cstdio>
#include <cstdlib>
#include <set>
#include <SDL2/SDL.h>
#include "draw.hpp"
#include "logics.hpp"

const char * game_name = "Conway's Game of Life";
const int screen_width = 640;
const int screen_height = 480;
bool quit_flag = false;

SDL_Window * window = NULL;
SDL_Renderer * render = NULL;
SDL_Event event;

std::set< std::pair< int, int > > draw;

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
        case SDL_KEYDOWN:
            switch ( event->key.keysym.sym ) {
                case SDLK_RIGHT:
                    draw = getNextStep();
                    break;
                default:
                    break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch ( event->button.button ) {
                case SDL_BUTTON_LEFT:
                    draw.insert( std::pair< int, int >( event->button.x / 4, event->button.y / 4 ) );
                    break;
                case SDL_BUTTON_RIGHT:
                    setInitialCondition( draw );
                    break;
                default:
                    break;
            }
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
    for ( auto p = draw.begin(); p != draw.end(); p++ ) {
        draw_pixel_size( p->first * 4, p->second * 4, 4, COLOR_WHITE );
    }
    // draw_rectangle_fill( 100, 100, 10, 10, COLOR_RED );
    // draw_rectangle_outline( 200, 100, 10, 10, COLOR_BLUE );
    // draw_pixel_size( 100, 200, 5, COLOR_GREEN );
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