#include <cstdio>
#include <cstdlib>
#include <set>
#include <SDL2/SDL.h>
#include "draw.hpp"
#include "logics.hpp"

const char * game_name = "Conway's Game of Life";
const int screen_width = 640;
const int screen_height = 480;
int pixel_size = 8, mx = -1, my = -1;
bool quit_flag = false;
bool button_set = false;

SDL_Window * window = NULL;
SDL_Renderer * render = NULL;
SDL_Event event;
SDL_Texture * texture = NULL;

std::set< std::pair< int, int > > draw;

void game_send_error( int code ) {
    printf( "[error]: %s\n", SDL_GetError() );
    exit( code );
}

SDL_Texture * generate_wireframe_texture( SDL_Renderer * render ) {
    SDL_Texture * texture = SDL_CreateTexture( render, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, screen_width, screen_height );
    SDL_SetRenderTarget( render, texture );
    set_color4u( 0xff, 0xff, 0xff, 0x64 );
    for ( size_t i = 0; i < screen_width; i += pixel_size ) {
        SDL_RenderDrawLine( render, i, 0, i, screen_height );
    }
    for ( size_t j = 0; j < screen_height; j += pixel_size ) {
        SDL_RenderDrawLine( render, 0, j, screen_width, j );
    }
    set_color4u( 0xff, 0xff, 0xff, 0xff );
    SDL_SetRenderTarget( render, NULL );
    return texture;
}

void set_point( int x, int y ) {
    auto obj = std::pair< int, int >( x / pixel_size, y / pixel_size );
    auto it = draw.find( obj );

    if ( it != draw.end() ) {
        draw.erase( it );
    } else {
        draw.insert( obj );
    }
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
                    nextStep(draw);
                    break;
                case SDLK_r:
                    draw.clear();
                    break;
                case SDLK_ESCAPE:
                    quit_flag = true;
                    break;
                default:
                    break;
            }
            break;
        case SDL_MOUSEMOTION:
            mx = event->motion.x - pixel_size / 2;
            my = event->motion.y - pixel_size / 2;
            if ( button_set ) {
                draw.insert( std::pair< int, int >( event->motion.x / pixel_size,
                                                    event->motion.y / pixel_size ) );
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch ( event->button.button ) {
                case SDL_BUTTON_LEFT:
                    if ( button_set == false ) {
                        set_point( event->button.x, event->button.y );
                    }
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
    SDL_RenderCopy( render, texture, NULL, NULL );
    set_coloru( COLOR_WHITE );
    for ( auto p = draw.begin(); p != draw.end(); p++ ) {
        draw_pixel_size( p->first * pixel_size, p->second * pixel_size, pixel_size );
    }
    set_coloru( COLOR_RED );
    draw_pixel_size( mx, my, pixel_size );
    set_coloru( COLOR_BLACK );
    SDL_RenderPresent( render );
}

void game_destroy( void ) {
    draw.clear();
    SDL_DestroyTexture( texture );
    SDL_DestroyRenderer( render );
    SDL_DestroyWindow( window );
    SDL_Quit();
}

void game_init( void ) {
    SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS );
    window = SDL_CreateWindow( game_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               screen_width, screen_height, SDL_WINDOW_SHOWN );
    if ( window == NULL ) {
        game_send_error( EXIT_FAILURE );
    }
    render = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE );
    if ( render == NULL ) {
        game_send_error( EXIT_FAILURE );
    }
    SDL_SetRenderDrawBlendMode( render, SDL_BLENDMODE_BLEND ); 
    draw_init( render );
    texture = generate_wireframe_texture( render );
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
