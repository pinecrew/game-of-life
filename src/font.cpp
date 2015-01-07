#include "font.hpp"

const char NULL_STR = '\0';

int font_load( SDL_Renderer * r, font_table_t ** t, const char * font ) {
    unsigned int text_size = 0, abc_size = 0;
    SDL_Texture *tex = NULL;
    font_table_t *a = NULL;
    wint_t current = 0;
    char * text_name;
    size_t load = 1;
    int id = 0;
    FILE * f;

    a = new font_table_t;
    *t = a;
    f = fopen( font, "rb" );
    if ( f == NULL ) {
        return A_ERROR_OPEN_FILE;
    }
    fread( &( text_size ), sizeof(int), 1, f );
    fread( &( abc_size ), sizeof(int), 1, f );
    text_name = new char [text_size];
    a->table = new int [abc_size];
    fread( text_name, text_size, 1, f );
    fread( &( a->t_width ), sizeof(int), 1, f );
    fread( &( a->t_height ), sizeof(int), 1, f );
    tex = IMG_LoadTexture( r, text_name );
    delete[] text_name;
    a->font = tex;
    if ( tex == NULL ) {
        fclose( f );
        return A_ERROR_LOAD_TEXTURE;
    }
    SDL_QueryTexture( tex, NULL, NULL, &( a->f_width ), &( a->f_height ) );
    fseek( f, sizeof(int) * 4 + text_size + 1, SEEK_SET );
    do {
        load = fread( &current, 2, 1, f );
        if ( current != L'\n' && current < 0xFFFF && load != 0 ) {
            a->table[current] = id++;
        }
    } while ( load != 0 );
    fclose( f );
    return A_SUCCESS;
}

void font_draw( SDL_Renderer * r, font_table_t * t, const wchar_t * text, int x, int y ) {
    SDL_Rect wnd = { 0, 0, t->t_width, t->t_height };
    SDL_Rect pos = { 0, 0, t->t_width, t->t_height };
    int dy = 0, i = 0, id = 0, old_x = x;
    wint_t current;

    pos.x = x; pos.y = y;
    while ( ( current = text[i++] ) != NULL_STR ) {
        switch ( current ) {
            case '\n':
                pos.y += t->t_height;
                pos.x = old_x;
                continue;
            case '\t':
                pos.x += 2 * t->t_width;
                continue;
            // to upper
            case 'a'...'z':
            case L'а'...L'я':
                current -= 0x20;
                break;
        }
        id = t->table[current];
        while ( id * t->t_width >= t->f_width ) {
            id -= t->f_width / t->t_width;
            dy++;
        }
        wnd.x = id * t->t_width; wnd.y = dy * t->t_height;
        pos.x += t->t_width;
        SDL_RenderCopy( r, t->font, &wnd, &pos );
        dy = 0;
    }
}

void font_destroy( font_table_t * t ) {
    SDL_DestroyTexture( t->font );
    delete[] t->table;
    delete t;
}

void font_coloru( font_table_t * t, Uint32 color ) {
    SDL_SetTextureColorMod( t->font, color >> 16, ( color >> 8 ) & 0xFF, color & 0xFF );
}