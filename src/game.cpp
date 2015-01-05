#include <cstdio>
#include <cstdlib>
#include <locale>
#include <set>
#include <sys/time.h>
#include <SDL2/SDL.h>
#include "draw.hpp"
#include "logics.hpp"
#include "font.hpp"

const char * game_name = "Conway's Game of Life";
const int screen_width = 640;
const int screen_height = 480;
const int border_size = 24;
int pixel_size = 8, mx = -1, my = -1;
bool quit_flag = false;
bool button_set = false;

SDL_Window * window = NULL;
SDL_Renderer * render = NULL;
SDL_Event event;
SDL_Texture * texture = NULL;

std::set< std::pair< int, int > > draw;

font_table_t * ft = NULL;

void game_send_error( int code ) {
    printf( "[error]: %s\n", SDL_GetError() );
    exit( code );
}

SDL_Texture * generate_wireframe_texture( void ) {
    SDL_Texture * texture = SDL_CreateTexture( render, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 
                                               screen_width, screen_height );
    SDL_SetRenderTarget( render, texture );
    SDL_RenderClear( render );
    set_color4u( 0xff, 0xff, 0xff, 0x64 );
    for ( size_t i = 0; i < screen_width; i += pixel_size ) {
        SDL_RenderDrawLine( render, i, 0, i, screen_height - border_size );
    }
    for ( size_t j = 0; j < screen_height - border_size; j += pixel_size ) {
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

void gamepole_resize( int resize ) {
    SDL_DestroyTexture( texture );
    if ( resize < 0 && pixel_size == 3 ) {
        return;
    }
    pixel_size += resize;
    if ( pixel_size > 3 ) {
        texture = generate_wireframe_texture();
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
            mx = ( event->motion.x ) / pixel_size * pixel_size;
            my = ( event->motion.y ) / pixel_size * pixel_size;
            if ( button_set ) {
                draw.insert( std::pair< int, int >( ( event->motion.x ) / pixel_size,
                                                    ( event->motion.y ) / pixel_size) );
            }
            break;
        case SDL_MOUSEWHEEL:
            switch ( event->wheel.type ) {
                case SDL_MOUSEWHEEL:
                    gamepole_resize( event->wheel.y );
                    event->wheel.y = 0; // dirty hack
                    break;
                default:
                    break;
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

unsigned int get_ticks( void ) {
    struct timeval tv;
    gettimeofday( &tv, 0 );
    return ( tv.tv_sec * 1000 + tv.tv_usec / 1000 );
}

float get_fps( void ) {
    static float NewCount = 0.0f, LastCount = 0.0f, FpsRate = 0.0f;
    static int FrameCount = 0;

    NewCount = (float) get_ticks();
    if ( ( NewCount - LastCount ) > 1000 ) {
        FpsRate = ( FrameCount * 1000 ) / ( NewCount - LastCount );
        LastCount = NewCount;
        FrameCount = 0;
    }
    FrameCount++;
    return FpsRate;
}

void game_loop( void ) {
    // insert code
}

void game_render( void ) {
    const int BUFFER_SIZE = 128;
    const wchar_t tmp[] = L"FPS: %.2f; count = %d; mouse = %d, %d";
    wchar_t buffer[BUFFER_SIZE];

    SDL_RenderClear( render );
    SDL_RenderCopy( render, texture, NULL, NULL );
    set_coloru( COLOR_WHITE );
    for ( auto p = draw.begin(); p != draw.end(); p++ ) {
        draw_pixel_size( p->first * pixel_size, p->second * pixel_size, pixel_size );
    }
    set_coloru( COLOR_RED );
    draw_pixel_size( mx, my, pixel_size );
    set_coloru( COLOR_BLACK );
    swprintf( buffer, BUFFER_SIZE, tmp, get_fps(), draw.size(), mx, my );
    font_draw( render, ft, buffer, 5, screen_height - 16 );
    SDL_RenderPresent( render );
}

void game_destroy( void ) {
    font_destroy( ft );
    draw.clear();
    SDL_DestroyTexture( texture );
    SDL_DestroyRenderer( render );
    SDL_DestroyWindow( window );
    SDL_Quit();
}

void game_init( void ) {
    setlocale( LC_ALL, "" );
    SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS );
    window = SDL_CreateWindow( game_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               screen_width, screen_height, SDL_WINDOW_SHOWN );
    if ( window == NULL ) {
        game_send_error( EXIT_FAILURE );
    }
    render = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | 
                                 SDL_RENDERER_TARGETTEXTURE );
    if ( render == NULL ) {
        game_send_error( EXIT_FAILURE );
    }
    SDL_SetRenderDrawBlendMode( render, SDL_BLENDMODE_BLEND ); 
    draw_init( render );
    texture = generate_wireframe_texture();
    font_load( render, &ft, "./data/font.cfg" );
    font_coloru( ft, 0xFF7F27 );
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
