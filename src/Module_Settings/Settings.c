
/*
 * #################################################
 *
 *      Description:
 * Handles application configuration management.
 * Includes loading, saving, and updating runtime settings,
 * as well as parsing visual styles (ANSI escape codes).
 *
 *      License:
 * This program is distributed under the terms of the GNU General Public License (GPL),
 * ensuring the freedom to redistribute and modify the software in accordance with open-source standards.
 *
 *  	Version:  1.0
 *		Created:  07/07/2025
 *
 *  	Author:
 * Catoni Mirko (IMprojtech)
 *
 * #################################################
 */

#include "Settings.h"

// --------------------------------------
/* Handler declarations */
static char *trim( char *s );
static const char *map_symbol( const char *symbol );
static void parse_style( const char *spec, char *outbuf );
static void validate_editor( const char *editor );

// --------------------------------------
/***** Trim whitespace; skip full-line or inline comments (# or //) *****/
static char *trim( char *s ) {
    char *end;

    while ( isspace( (unsigned char)*s ) )
        s++;
    if ( *s == '#' || ( *s == '/' && *( s + 1 ) == '/' ) )
        return NULL;

    end = s + strlen( s ) - 1;
    while ( end > s && ( isspace( (unsigned char)*end ) || *end == '\r' || *end == '\n' ) )
        *end-- = '\0';

    char *cmt = strstr( s, "//" );
    if ( !cmt )
        cmt = strchr( s, '#' );
    if ( cmt )
        *cmt = '\0';

    end = s + strlen( s ) - 1;
    while ( end > s && isspace( (unsigned char)*end ) )
        *end-- = '\0';
    return *s ? s : NULL;
}

// --------------------------------------
/***** Map a symbolic name to its ANSI code ******/
static const char *map_symbol( const char *symbol ) {
    if ( strcasecmp( symbol, "bold" ) == 0 )
        return CSI "1m";
    if ( strcasecmp( symbol, "underline" ) == 0 )
        return CSI "4m";
    if ( strcasecmp( symbol, "black" ) == 0 )
        return CSI "30m";
    if ( strcasecmp( symbol, "red" ) == 0 )
        return CSI "31m";
    if ( strcasecmp( symbol, "green" ) == 0 )
        return CSI "32m";
    if ( strcasecmp( symbol, "yellow" ) == 0 )
        return CSI "33m";
    if ( strcasecmp( symbol, "blue" ) == 0 )
        return CSI "34m";
    if ( strcasecmp( symbol, "magenta" ) == 0 )
        return CSI "35m";
    if ( strcasecmp( symbol, "cyan" ) == 0 )
        return CSI "36m";
    if ( strcasecmp( symbol, "white" ) == 0 )
        return CSI "37m";
    if ( strcasecmp( symbol, "reset" ) == 0 )
        return CSI "0m";
    fprintf( stderr, "Invalid style symbol: '%s'\n", symbol );
    exit( EXIT_FAILURE );
}

// --------------------------------------
/***** Parse comma-separated style spec into ANSI codes *****/
static void parse_style( const char *spec, char *outbuf ) {
    outbuf[0] = '\0';
    if ( !spec )
        return;

    char tmp[VALUE_MAX];
    strncpy( tmp, spec, VALUE_MAX - 1 );
    char *tok = strtok( tmp, "," );
    while ( tok ) {

        char *t = tok;
        while ( isspace( (unsigned char)*t ) )
            t++;

        char *end = t + strlen( t ) - 1;
        while ( end > t && isspace( (unsigned char)*end ) )
            *end-- = '\0';

        strncat( outbuf, map_symbol( t ), VALUE_MAX - strlen( outbuf ) - 1 );
        tok = strtok( NULL, "," );
    }
}

// --------------------------------------
/***** Validate editor field *****/
static void validate_editor( const char *editor ) {
    if ( strcasecmp( editor, "vim" ) && strcasecmp( editor, "nano" ) &&
         strcasecmp( editor, "nul" ) ) {
        fprintf( stderr, "Invalid editor: %s\n", editor );
        exit( EXIT_FAILURE );
    }
}

// --------------------------------------
/***** Write config and style specs *****/
void write_config( const char *path, Config *cfg, Style *stl ) {
    FILE *fp;

    if ( ( fp = fopen( path, "w" ) ) == NULL ) {
        fprintf( stderr, "File \"%s\" creation failed \n", path );
        exit( EXIT_FAILURE );
    }

    fprintf( fp, "# Configuration file!\n\n" );

    fprintf( fp, "SHA1= %s\n\n", cfg->hash_pass );

    fprintf( fp, "\n# Editor  (vim,nano)\n" );
    fprintf( fp, "Editor= %s\n\n", cfg->editor );

    fprintf( fp, "\n# Style (bold, underline, reset, black, red, green, yellow, blue, magenta, "
                 "cyan, white)\n" );
    fprintf( fp, "Style_Hash= %s\n", stl->color_hash );
    fprintf( fp, "Style_Tag= %s\n", stl->color_tag );
    fprintf( fp, "Style_Comment= %s\n", stl->color_comment );
    fprintf( fp, "Style_Keywords= %s\n", stl->color_keywords );
    fprintf( fp, "Style_Body= %s\n", stl->color_body );
    fprintf( fp, "Style_Date= %s\n", stl->color_date );
    fprintf( fp, "Style_File= %s\n", stl->color_file );
    fprintf( fp, "Style_Generic= %s\n\n", stl->color_generic );

    fprintf( fp, "FileNote= %s # General notes\n\n", cfg->file_note );

    fprintf( fp, "$= %s # Notes file in use\n\n", cfg->file_note );

    fclose( fp );
}

// --------------------------------------
/***** Load config and style specs *****/
void load_config( const char *path, Config *cfg, Style *stl ) {
    FILE *fp = fopen( path, "r" );
    char line[CONFIG_LINE_MAX], *val;

    while ( fgets( line, sizeof( line ), fp ) ) {

        char *ln = trim( line );
        if ( !ln )
            continue;

        char *eq = strchr( ln, '=' );
        if ( !eq )
            continue;

        *eq = '\0';
        val = trim( eq + 1 );
        if ( !val )
            continue;

        if ( !strcasecmp( ln, "SHA1" ) )
            strcpy( cfg->hash_pass, val );
        else if ( !strcasecmp( ln, "Editor" ) ) {
            validate_editor( val );
            strcpy( cfg->editor, val );
        } else if ( !strcasecmp( ln, "$" ) )
            strcpy( cfg->file_note, val );
        else if ( !strcasecmp( ln, "Style_Tag" ) )
            parse_style( val, stl->color_tag );
        else if ( !strcasecmp( ln, "Style_Hash" ) )
            parse_style( val, stl->color_hash );
        else if ( !strcasecmp( ln, "Style_Comment" ) )
            parse_style( val, stl->color_comment );
        else if ( !strcasecmp( ln, "Style_Keywords" ) )
            parse_style( val, stl->color_keywords );
        else if ( !strcasecmp( ln, "Style_Body" ) )
            parse_style( val, stl->color_body );
        else if ( !strcasecmp( ln, "Style_Date" ) )
            parse_style( val, stl->color_date );
        else if ( !strcasecmp( ln, "Style_File" ) )
            parse_style( val, stl->color_file );
        else if ( !strcasecmp( ln, "Style_Generic" ) )
            parse_style( val, stl->color_generic );
    }
    fclose( fp );

    if ( ( fp = fopen( cfg->file_note, "rb" ) ) == NULL ) {
        if ( ( fp = fopen( cfg->file_note, "wb" ) ) == NULL ) {
            fprintf( stderr, "file creation failed: %s\n", cfg->file_note );
            exit( EXIT_FAILURE );
        }
    }
    fclose( fp );
}

// --------------------------------------
/***** Runtime configuration commands *****/
void update_config_value( const char *command, int index, const char *input,
                          const char *config_path, Config *cfg, Style *stl ) {
    FILE *fp = fopen( config_path, "r" );
    if ( !fp ) {
        fprintf( stderr, "Error opening settings file: %s\n", config_path );
        exit( EXIT_FAILURE );
    }

    char *lines[MAX_LINES];
    int line_count = 0;
    char buffer[MAX_LINE_LEN];

    while ( fgets( buffer, sizeof( buffer ), fp ) && line_count < MAX_LINES ) {
        lines[line_count] = strdup( buffer );
        line_count++;
    }
    fclose( fp );

    int dollar_index = -1;
    for ( int i = 0; i < line_count; i++ ) {
        if ( lines[i][0] == '$' ) {
            dollar_index = i;
            break;
        }
    }

    if ( strcmp( command, "AddFile" ) == 0 ) {
        dollar_index -= 1;

        char *new_line = malloc( MAX_LINE_LEN );
        snprintf( new_line, MAX_LINE_LEN, "FileNote= %s \n", input );

        for ( int i = line_count; i > dollar_index; i-- ) {
            lines[i] = lines[i - 1];
        }
        lines[dollar_index] = new_line;
        line_count++;

    }

    else if ( strcmp( command, "SetFile" ) == 0 ) {
        int current = 0;
        char path_only[MAX_LINE_LEN] = { 0 };
        for ( int i = 0; i < line_count; i++ ) {
            if ( strncmp( lines[i], "FileNote=", 9 ) == 0 ) {
                if ( current == index ) {
                    char *start = lines[i] + 9;
                    while ( *start && isspace( (unsigned char)*start ) )
                        start++;
                    char *end = strstr( start, "#" );
                    size_t len = end ? (size_t)( end - start ) : strlen( start );
                    strncpy( path_only, start, len );
                    path_only[len] = '\0';
                    char *trim = path_only + len - 1;
                    while ( trim > path_only && isspace( (unsigned char)*trim ) )
                        *trim-- = '\0';
                    strcpy( cfg->file_note, path_only );
                    break;
                }
                current++;
            }
        }
        if ( current != index ) {
            fprintf( stderr, "SetFile: indice %d non trovato\n", index );
            exit( EXIT_FAILURE );
        }
        if ( dollar_index >= 0 ) {
            snprintf( buffer, sizeof( buffer ), "$= %.480s ## file in use\n", path_only );
            free( lines[dollar_index] );
            lines[dollar_index] = strdup( buffer );
        }
    }

    else if ( strcmp( command, "ShowFile" ) == 0 ) {
        printf( "\n%sNotes file in use: %s%s%s\n\n", stl->color_generic, stl->color_file,
                cfg->file_note, CSI "0m" );
        int n = 0;
        for ( int i = 0; i < line_count; i++ ) {
            if ( strncmp( lines[i], "FileNote=", 9 ) == 0 ) {
                char *start = lines[i] + 9;
                while ( *start == ' ' )
                    start++;
                char *comment = strstr( start, "#" );
                if ( comment ) {
                    *comment = '\0';
                    comment += 2;
                }
                printf( "%s%2d %s%s%s", stl->color_hash, n++, stl->color_file, start, CSI "0m" );
                if ( comment ) {
                    while ( isspace( (unsigned char)*comment ) )
                        comment++;
                    printf( "%s%s%s", stl->color_comment, comment, CSI "0m" );
                }
            }
        }
        printf( "\n" );
        return;
    }

    else if ( strncmp( command, "Editor=", 7 ) == 0 ) {
        validate_editor( input );
        int updated = 0;
        for ( int i = 0; i < line_count; i++ ) {
            if ( strncmp( lines[i], "Editor=", 7 ) == 0 ) {
                snprintf( buffer, sizeof( buffer ), "Editor= %s\n", input );
                free( lines[i] );
                lines[i] = strdup( buffer );
                updated = 1;
                break;
            }
        }
        if ( !updated ) {
            lines[line_count++] = strdup( buffer );
        }
    }

    else {
        fprintf( stderr, "Unknown command: %s\n", command );
        exit( EXIT_FAILURE );
    }

    fp = fopen( config_path, "w" );
    if ( !fp ) {
        fprintf( stderr, "file \"%s\" write failed\n", config_path );
        exit( EXIT_FAILURE );
    }
    for ( int i = 0; i < line_count; i++ ) {
        fputs( lines[i], fp );
        free( lines[i] );
    }
    fclose( fp );
    exit( 0 );
}

// --------------------------------------
/***** Load structures with default settings *****/
void default_config( Config *cfg, Style *stl ) {

    strncpy( cfg->editor, "vim", NAME_MAX - 1 );

    strncpy( stl->color_tag, "bold,yellow", NAME_MAX - 1 );
    strncpy( stl->color_hash, "bold,red", NAME_MAX - 1 );
    strncpy( stl->color_comment, "bold,white", NAME_MAX - 1 );
    strncpy( stl->color_keywords, "bold,magenta", NAME_MAX - 1 );
    strncpy( stl->color_body, "bold,cyan", NAME_MAX - 1 );
    strncpy( stl->color_date, "bold,green", NAME_MAX - 1 );
    strncpy( stl->color_file, "bold,blue", NAME_MAX - 1 );
    strncpy( stl->color_generic, "bold,white", NAME_MAX - 1 );
}

// --------------------------------------
/***** Load structures with nocolor settings *****/
void default_config_nocolor( Style *stl ) {

    strncpy( stl->color_tag, CSI "0m", NAME_MAX - 1 );
    strncpy( stl->color_hash, CSI "0m", NAME_MAX - 1 );
    strncpy( stl->color_comment, CSI "0m", NAME_MAX - 1 );
    strncpy( stl->color_keywords, CSI "0m", NAME_MAX - 1 );
    strncpy( stl->color_body, CSI "0m", NAME_MAX - 1 );
    strncpy( stl->color_date, CSI "0m", NAME_MAX - 1 );
    strncpy( stl->color_file, CSI "0m", NAME_MAX - 1 );
    strncpy( stl->color_generic, CSI "0m", NAME_MAX - 1 );
}
