#include <cstdio>
#include <cstdlib>
#include <locale>
#include <set>
#include <SDL2/SDL.h>
#include "draw.hpp"
#include "logics.hpp"
#include "font.hpp"

const char * game_name = "Conway's Game of Life";
const int screen_width = 640;
const int screen_height = 480;
const int border_size = 24;
const int help_box_width = 200;
const int help_box_height = 80;
int pixel_size = 8, mx = -1, my = -1, px = 0, py = 0;
bool quit_flag = false;
bool button_set = false;
bool help_flag = false;

const wchar_t help_info[] = 
    L"help menu:\n\n"
    L"  F1 -- this menu\n"
    L" ESC -- quit\n"
    L"LBLK -- set/clear\n"
    L" WUP -- zoom in\n"
    L"WDWN -- zoom out\n"
    L"   R -- clear gamepole\n"
    L"   > -- next generation";

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

SDL_Texture * generate_wireframe_texture( bool wireframe = true ) {
    SDL_Texture * texture = SDL_CreateTexture( render, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 
                                               screen_width, screen_height );
    SDL_SetRenderTarget( render, texture );
    SDL_RenderClear( render );
    if ( wireframe ) {
        set_color4u( 0xff, 0xff, 0xff, 0x64 );
        for ( size_t i = 0; i < screen_width; i += pixel_size ) {
            SDL_RenderDrawLine( render, i, 0, i, screen_height - border_size );
        }
        for ( size_t j = 0; j < screen_height - border_size; j += pixel_size ) {
            SDL_RenderDrawLine( render, 0, j, screen_width, j );
        }
    }
    set_color4u( 0x00, 0x00, 0xff, 0x32 );
    draw_rectangle_param( 0, 0, screen_width, border_size, true );
    draw_rectangle_param( 0, screen_height - border_size, screen_width, border_size, true );
    draw_rectangle_param( 0, border_size, border_size, screen_height - 2 * border_size, true );
    draw_rectangle_param( screen_width - border_size, border_size, border_size, screen_height - 2 * border_size, true );
    set_color4u( 0xff, 0xff, 0xff, 0xff );
    SDL_SetRenderTarget( render, NULL );
    return texture;
}

void set_point( int x, int y ) {
    auto obj = std::pair< int, int >( x / pixel_size - px / pixel_size, y / pixel_size - py / pixel_size );
    auto it = draw.find( obj );

    if ( it != draw.end() ) {
        draw.erase( it );
    } else {
        draw.insert( obj );
    }
}

void gamepole_resize( int resize ) {
    if ( resize < 0 && pixel_size == 1 ) {
        return;
    }
    SDL_DestroyTexture( texture );
    pixel_size += resize;
    if ( pixel_size > 3 ) {
        texture = generate_wireframe_texture();
    } else {
        texture = generate_wireframe_texture( false );
    }
}

bool intersect( int param, int p1, int p2 ) {
    if ( param >= p1 && param <= p2 ) {
        return true;
    }
    return false;
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
                case SDLK_F1:
                    help_flag = !help_flag;
                    break;
                default:
                    break;
            }
            event->key.keysym.sym = 0;
            break;
        case SDL_MOUSEMOTION:
            mx = ( event->motion.x ) / pixel_size * pixel_size;
            my = ( event->motion.y ) / pixel_size * pixel_size;
            if ( button_set ) {
                draw.insert( std::pair< int, int >( ( event->motion.x ) / pixel_size - px / pixel_size,
                                                    ( event->motion.y ) / pixel_size - py / pixel_size ) );
            }
            if ( intersect( event->motion.x, 0, border_size ) ) {
                px += pixel_size;
            } 
            if ( intersect( event->motion.x, screen_width - border_size, screen_width ) ) {
                px -= pixel_size;
            }
            if ( intersect( event->motion.y, 0, border_size ) ) {
                py += pixel_size;
            } 
            if ( intersect( event->motion.y, screen_height - border_size, screen_height ) ) {
                py -= pixel_size;
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

float get_fps( void ) {
    static float NewCount = 0.0f, LastCount = 0.0f, FpsRate = 0.0f;
    static int FrameCount = 0;

    NewCount = (float) SDL_GetTicks();
    if ( ( NewCount - LastCount ) > 1000.0f ) {
        FpsRate = ( FrameCount * 1000.0f ) / ( NewCount - LastCount );
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
    const wchar_t tmp[] = L"FPS: %.2f; count = %d; mouse = (%d, %d); (px, py) = (%d, %d); F1 -- help";
    wchar_t buffer[BUFFER_SIZE];

    SDL_RenderClear( render );
    SDL_RenderCopy( render, texture, NULL, NULL );
    set_coloru( COLOR_WHITE );
    for ( auto p = draw.begin(); p != draw.end(); p++ ) {
        int xp = p->first * pixel_size + px;
        int yp = p->second * pixel_size + py;
        if ( !( intersect( xp, 0, border_size ) || intersect( xp, screen_width - border_size, screen_width ) ||
             intersect( yp, 0, border_size ) || intersect( yp, screen_height - border_size, screen_height ) ) ) {
            draw_pixel_size( xp, yp, pixel_size );
        }
    }
    set_coloru( COLOR_RED );
    draw_pixel_size( mx, my, pixel_size );
    swprintf( buffer, BUFFER_SIZE, tmp, get_fps(), draw.size(), mx, my, px, py );
    font_draw( render, ft, buffer, 5, screen_height - 16 );
    if ( help_flag ) {
        set_color4u( 0x00, 0x00, 0xff, 0x96 );
        draw_rectangle_param( ( screen_width - help_box_width ) / 2, ( screen_height - help_box_height ) / 2, help_box_width, help_box_height, true );
        font_draw( render, ft, help_info, ( screen_width - help_box_width ) / 2, ( screen_height - help_box_height ) / 2 + 4 );
    }
    set_coloru( COLOR_BLACK );
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
