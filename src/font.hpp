#pragma once
#include <cstdio>
#include <cstring>
#include <wchar.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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
    int * table;
    SDL_Texture * font;
};
typedef struct font_table font_table_t;

int font_load( SDL_Renderer * r, font_table_t ** t, const char * font );
void font_draw( SDL_Renderer * r, font_table_t * t, const wchar_t * text, int x, int y );
void font_destroy( font_table_t * t );
void font_coloru( font_table_t * t, Uint32 color );

// структура файла конфигурации
// заголовок:
//     unsigned int size;
//     unsigned int abc_size;
//     char * filename[size];
//     int width;
//     int height;
// размер заголовка = sizeof(int) * 4 + size + 1;
// далее алфавит до конца файла в формате UTF-8
