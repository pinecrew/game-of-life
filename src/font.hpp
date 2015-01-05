#pragma once
#include <string.h>
#include <wchar.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const int ABC_SIZE = 0x468;

enum font_error {
    A_NULL_OBJECT = 0,
    A_SUCCESS,
    A_ERROR_OPEN_FILE,
    A_ERROR_LOAD_TEXTURE,
};

struct font_table {
    int f_height;
    int f_width;
    int t_height;
    int t_width;
    int table[ABC_SIZE];
    SDL_Texture * font;
};
typedef struct font_table font_table_t;

struct token {
    char ** name;
    unsigned int size;
};
typedef struct token token_t;

int font_load( SDL_Renderer * r, font_table_t ** t, const char * font );
void font_draw( SDL_Renderer * r, font_table_t * t, const wchar_t * text, int x, int y );
void font_destroy( font_table_t * t );
void font_coloru( font_table_t * t, Uint32 color );