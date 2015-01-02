#include <cstdio>
#include <cstdlib>
#include <set>
#include <SDL2/SDL.h>
#include "draw.hpp"
#include "logics.hpp"

const char * game_name = "Conway's Game of Life";
const int screen_width = 640;
const int screen_height = 480;
int pixel_size = 8;
bool quit_flag = false;
bool button_set = false;

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
        case SDL_MOUSEMOTION:
            if ( button_set ) {
                draw.insert( std::pair< int, int >( event->button.x / pixel_size, 
                                                    event->button.y / pixel_size ) );
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch ( event->button.button ) {
                case SDL_BUTTON_LEFT:
                    draw.insert( std::pair< int, int >( event->button.x / pixel_size, 
                                                        event->button.y / pixel_size ) );
                    break;
                case SDL_BUTTON_RIGHT:
                    setInitialCondition( draw );
                    break;
                default:
                    break;
            }
            button_set = true;
            break;
        case SDL_MOUSEBUTTONUP:
            button_set = false;
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
    for ( auto p = draw.begin(); p != draw.end(); p++ ) {
        draw_pixel_size( p->first * pixel_size, p->second * pixel_size, pixel_size, COLOR_WHITE );
    }
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