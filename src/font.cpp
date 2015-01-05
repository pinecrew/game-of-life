#include "font.hpp"

const char NULL_STR = '\0';

void string_copy_n( char *src, char *dst, size_t start, size_t length ) {
    size_t i = 0;

    while ( ( dst[i] = src[i+start] ) != NULL_STR && ++i <= length );
    dst[i] = NULL_STR;
}

void tokenize( token_t ** a, char * data, const char * delimeters ) {
    size_t start = 0, counter = 0, length = 0, n = 0;
    size_t del_length = strlen( delimeters ) + 1;
    size_t dat_length = strlen( data ) + 1;
    token_t *obj = NULL;
    size_t prev, next;
    size_t i, j;

    obj = (token_t *) malloc( sizeof( token_t ) );
    obj->size = next = 0;
    prev = -1;
    for ( i = 0; i < dat_length; i++ ) {
        for ( j = 0; j < del_length; j++ ) {
            if ( data[i] == delimeters[j] ) {
                if ( next - prev > 1 ) {
                    obj->size++;
                }
                prev = next;
            }
        }
        next++;
    }
    obj->name = (char **) malloc( sizeof( obj->size ) * sizeof( char * ) );
    do {
        for ( i = 0; i < del_length; i++ ) {
            if ( data[counter] == delimeters[i] ) {
                length = counter - start;
                if ( length > 0 ) {
                    obj->name[n] = (char *) malloc( ( length + 1 ) * sizeof( char ) );
                    string_copy_n( data, obj->name[n], start, length-1 );
                    start = counter + 1;
                    n++;
                    break;
                }
                start = counter + 1;
            }
        }
    } while ( data[counter++] != NULL_STR ); 
    *a = obj;
}

void free_token( token_t *data ) {
    size_t i;

    if ( data != NULL ) {
        for ( i = 0; i < data->size; i++ ) {
            free( data->name[i] );
        }
        free( data->name );
        free( data );
    }
}

int font_load( SDL_Renderer * r, font_table_t ** t, const char * font ) {
    SDL_Texture *tex = NULL;
    font_table_t *a = NULL;
    char tex_name[64];
    char buffer[128];
    wint_t current = 0;
    size_t load = 1, i = 0;
    FILE *f;
    token_t *token;
    int id = 0;

    a = (font_table_t *) calloc( 1, sizeof( font_table_t ) );
    *t = a;
    f = fopen( font, "rb" );
    if ( f == NULL ) {
        return A_ERROR_OPEN_FILE;
    }
    do {
        load = fread( &buffer[i], 1, 1, f );
    } while ( buffer[i++] != '\0' );
    tokenize( &token, buffer, " " );
    strcpy( tex_name, token->name[0] );
    a->t_width = atoi( token->name[1] );
    a->t_height = atoi( token->name[2] );
    free_token( token );
    tex = IMG_LoadTexture( r, tex_name );
    a->font = tex;
    if ( tex == NULL ) {
        fclose( f );
        return A_ERROR_LOAD_TEXTURE;
    }
    SDL_QueryTexture( tex, NULL, NULL, &( a->f_width ), &( a->f_height ) );
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
    free( t );
}

void font_coloru( font_table_t * t, Uint32 color ) {
    SDL_SetTextureColorMod( t->font, color >> 16, ( color >> 8 ) & 0xFF, color & 0xFF );
}