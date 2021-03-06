#include <cstdio>
#include <cstdlib>
#include <locale>
#include <cmath>
#include <SDL2/SDL.h>
#include "draw.hpp"
#include "logics.hpp"
#include "font.hpp"

#define MOD(x, y) (((x) % (y) + (y)) % (y))

pair< int, int > operator+(pair< int, int > lhs, pair< int, int > rhs)
{
    return { lhs.first + rhs.first, lhs.second + rhs.second };
}

pair< int, int > operator+(pair< int, int > lhs, int rhs)
{
    return { lhs.first + rhs, lhs.second + rhs };
}

pair< int, int > operator-(pair< int, int > lhs, pair< int, int > rhs)
{
    return { lhs.first - rhs.first, lhs.second - rhs.second };
}

pair< int, int > operator*(pair< int, int > lhs, int rhs)
{
    return { lhs.first * rhs, lhs.second * rhs };
}

pair< int, int > operator/(pair< int, int > lhs, int rhs)
{
    return { lhs.first / rhs, lhs.second / rhs };
}

pair< int, int > operator%(pair< int, int > lhs, int rhs)
{
    return { lhs.first % rhs, lhs.second % rhs };
}

const char * game_name = "Conway's Game of Life";
int screen_width = 640;
int screen_height = 480;
const int border_size = 24;
const int help_box_width = 210;
const int help_box_height = 130;
const int min_pixel_size = 3;
int pixel_size = 8;
point center = { screen_width / 2, screen_height / 2 }; // центр видимой области
point mouse = center;     // позиция курсора
point origin = center; // позиция начала координат относительно верххнего левого края
int game_counter = 0, MAX_COUNT = 5;
bool quit_flag = false;
bool button_set = false;
bool help_flag = false;
bool game_step = false;
SDL_Window * window = NULL;
SDL_Renderer * render = NULL;
SDL_Texture * texture = NULL;
SDL_Event event;
font_table_t * ft = NULL;
cells draw;

const wchar_t tmp[] = L"(%s) FPS: %.2f; count %d; id (%d, %d); shift (%d, %d); delay %d";
const wchar_t * game_status[] = {
    (const wchar_t *) "pause",
    (const wchar_t *) "play"
};
const wchar_t help_info[] =
    L"help menu:\n\n"
    L"   F1 -- this menu\n"
    L"  ESC -- quit\n"
    L" LBLK -- set/clear\n"
    L"  WUP -- zoom in\n"
    L" WDWN -- zoom out\n"
    L"    R -- clear gamepole\n"
    L"SPACE -- next generation\n"
    L" LEFT -- move left\n"
    L"RIGHT -- move right\n"
    L"   UP -- move up\n"
    L" DOWN -- move down\n"
    L"    > -- speed up\n"
    L"    < -- speed down";

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
    for ( size_t i = MOD( origin.first, pixel_size); i < screen_width; i += pixel_size ) {
        SDL_RenderDrawLine( render, i, 0, i, screen_height - border_size );
    }
    for ( size_t j = MOD( origin.second, pixel_size); j < screen_height - border_size; j += pixel_size ) {
        SDL_RenderDrawLine( render, 0, j, screen_width, j );
    }
    set_color4u( 0x00, 0x00, 0xff, 0x32 );
    draw_rectangle_param( 0, 0, screen_width, border_size, true );
    draw_rectangle_param( 0, screen_height - border_size, screen_width, border_size, true );
    draw_rectangle_param( 0, border_size, border_size, screen_height - 2 * border_size, true );
    draw_rectangle_param( screen_width - border_size, border_size, border_size,
                          screen_height - 2 * border_size, true );
    set_color4u( 0xff, 0xff, 0xff, 0xff );
    SDL_SetRenderTarget( render, NULL );
    return texture;
}

void set_point( point p, bool auto_erase ) {
    auto obj = p - origin;
    obj.first = floor( (double)( obj.first ) / pixel_size );
    obj.second = floor( (double)( obj.second ) / pixel_size );
    auto it = draw.find( obj );

    if ( it != draw.end() && auto_erase )
        draw.erase( it );
    else
        draw.insert( obj );
}

void gamepole_resize( int resize ) {
    if ( resize < 0 && pixel_size == min_pixel_size ) {
        return;
    }
    SDL_DestroyTexture( texture );

    origin = mouse + (origin - mouse) * (pixel_size + resize) / pixel_size;
    pixel_size += resize;
    texture = generate_wireframe_texture();
}

void gamepole_shift( int x, int y ) {
    origin = origin + point(x, y);
}

bool intersect( int param, int p1, int p2 ) {
    if ( param >= p1 && param <= p2 ) {
        return true;
    }
    return false;
}

void get_mouse_pos( point & mouse ) {
    int x, y;
    SDL_GetMouseState( &x, &y );
    mouse = point( x, y ) / pixel_size * pixel_size + MOD( origin, pixel_size );
}

void game_event( SDL_Event *event ) {
    SDL_PollEvent( event );
    switch ( event->type ) {
        case SDL_QUIT:
            quit_flag = true;
            break;
        case SDL_WINDOWEVENT:
            if ( event->window.event == SDL_WINDOWEVENT_RESIZED ) {
                screen_width  = event->window.data1;
                screen_height = event->window.data2;
                gamepole_resize( 0 );
            }
            break;
        case SDL_KEYDOWN:
            switch ( event->key.keysym.sym ) {
                case SDLK_SPACE:
                    game_step = !game_step;
                    break;
                case SDLK_LEFT:
                    gamepole_shift( -pixel_size, 0 );
                    break;
                case SDLK_RIGHT:
                    gamepole_shift( pixel_size, 0 );
                    break;
                case SDLK_UP:
                    gamepole_shift( 0, -pixel_size );
                    break;
                case SDLK_DOWN:
                    gamepole_shift( 0, pixel_size );
                    break;
                case SDLK_PERIOD:
                    if ( MAX_COUNT > 0 ) {
                        MAX_COUNT -= 1;
                    }
                    break;
                case SDLK_COMMA:
                    MAX_COUNT += 1;
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
            event->key.keysym.sym = 0; // dirty hack
            break;
        case SDL_MOUSEMOTION:
            get_mouse_pos( mouse );
            if ( button_set ) {
                set_point( point( event->motion.x, event->motion.y ), false );
            }
            /* сдвиг области отрисовки при попаданию в border мыши */
            if ( intersect( event->motion.x, 0, border_size ) ) {
                origin.first += pixel_size;
            }
            if ( intersect( event->motion.x, screen_width - border_size, screen_width ) ) {
                origin.first -= pixel_size;
            }
            if ( intersect( event->motion.y, 0, border_size ) ) {
                origin.second += pixel_size;
            }
            if ( intersect( event->motion.y, screen_height - border_size, screen_height ) ) {
                origin.second -= pixel_size;
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
                        set_point( point( event->button.x, event->button.y ), true );
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
    if ( game_step && game_counter >= MAX_COUNT ) {
        nextStep( draw );
        game_counter = 0;
    } else {
        game_counter++;
    }
}

void game_render( void ) {
    const int BUFFER_SIZE = 128;
    wchar_t buffer[BUFFER_SIZE];

    SDL_RenderClear( render );
    SDL_RenderCopy( render, texture, NULL, NULL );
    set_coloru( COLOR_WHITE );
    for ( auto p: draw ) {
        int xp = p.first * pixel_size + origin.first;
        int yp = p.second * pixel_size + origin.second;
        if ( !( intersect( yp, screen_height - border_size, screen_height ) ) ) {
            draw_pixel_size( point(xp, yp), pixel_size );
        }
    }
    set_coloru( COLOR_RED );
    get_mouse_pos( mouse );
    draw_pixel_size( mouse + point(1, 1), pixel_size-1 );
    swprintf( buffer, BUFFER_SIZE, tmp, game_status[int(game_step)], get_fps(), draw.size(),
              mouse.first, mouse.second, origin.first, origin.second, MAX_COUNT );
    font_draw( render, ft, buffer, 5, screen_height - 16 );
    if ( help_flag ) {
        set_color4u( 0x00, 0x00, 0xff, 0x96 );
        draw_rectangle_param( ( screen_width - help_box_width ) / 2,
                              ( screen_height - help_box_height ) / 2,
                              help_box_width, help_box_height, true );
        font_draw( render, ft, help_info, ( screen_width - help_box_width ) / 2,
                   ( screen_height - help_box_height ) / 2 + 4 );
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
    setlocale( LC_CTYPE, "" );
    SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS );
    window = SDL_CreateWindow( game_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               screen_width, screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
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
