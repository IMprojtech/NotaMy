
/*
 * #################################################
 *
 *      Description:
 * Contains functions that interact directly with the file
 *
 *      License:
 * This program is distributed under the terms of the GNU General Public License (GPL),
 * ensuring the freedom to redistribute and modify the software in accordance with open-source standards.
 *
 *  	Version:  1.0
 *		Created:  25/07/2025
 *
 *  	Author:
 * Catoni Mirko (IMprojtech)
 *
 * #################################################
 */

// --------------------------------------
/***** Calculates the hash of the current notes file *****/
bool sha1_file( const char *filename, char out_hex[41] ) {
    FILE *fp = fopen( filename, "rb" );
    if ( !fp )
        return false;
    fseek( fp, 0, SEEK_END );
    long sz = ftell( fp );
    rewind( fp );

    unsigned char *buf = malloc( sz );
    if ( !buf ) {
        fclose( fp );
        return false;
    }
    fread( buf, 1, sz, fp );
    fclose( fp );

    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1( buf, sz, hash );
    free( buf );

    for ( int i = 0; i < SHA_DIGEST_LENGTH; i++ )
        sprintf( out_hex + i * 2, "%02x", hash[i] );
    out_hex[SHA_DIGEST_LENGTH * 2] = '\0';
    return true;
}

// --------------------------------------
/***** Returns the next field delimited by FIELD_DELIM *****/
static char *next_field( char **pptr ) {
    if ( *pptr == NULL )
        return NULL;

    char *start = *pptr;
    char *pos = strstr( start, FIELD_DELIM );
    if ( pos ) {
        *pos = '\0';
        *pptr = pos + strlen( FIELD_DELIM );

    } else {
        *pptr = NULL;
    }

    return start;
}

// --------------------------------------
/***** Read data from file (for printing) *****/
void read_dat( int start, int end, AppGlobal *app ) {
    FILE *In = fopen( app->cfg.file_note, "rb" );

    if ( !In ) {
        fprintf( stderr, "[ERROR] file \"%s\" opening failed\n", app->cfg.file_note );
        exit( EXIT_FAILURE );
    }

    if ( app->NDat.Body ) {
        free( app->NDat.Body );
        app->NDat.Body = NULL;
    }

    size_t size = (size_t)( end - start );
    char *buffer = malloc( size + 1 );
    if ( !buffer ) {
        fclose( In );
        fprintf( stderr, "[ERROR] memory allocation\n" );
        exit( EXIT_FAILURE );
    }

    fseek( In, start, SEEK_SET );
    fread( buffer, 1, size, In );
    buffer[size] = '\0';

    char *cursor = buffer;
    char *token;

    token = next_field( &cursor );
    if ( token )
        strncpy( app->NDat.Tag, token, sizeof( app->NDat.Tag ) - 1 );

    token = next_field( &cursor );
    if ( token )
        strncpy( app->NDat.Comment, token, sizeof( app->NDat.Comment ) - 1 );

    token = next_field( &cursor );
    if ( token )
        strncpy( app->NDat.Keywords, token, sizeof( app->NDat.Keywords ) - 1 );

    token = next_field( &cursor );
    if ( token )
        strncpy( app->NDat.Link_File, token, sizeof( app->NDat.Link_File ) - 1 );

    token = next_field( &cursor );
    if ( token )
        strncpy( app->NDat.Date, token, sizeof( app->NDat.Date ) - 1 );

    token = next_field( &cursor );
    if ( token )
        strncpy( app->NDat.Iv, token, sizeof( app->NDat.Iv ) - 1 );

    token = next_field( &cursor );
    if ( token )
        app->NDat.Protection = ( atoi( token ) != 0 );

    if ( cursor && *cursor ) {
        char *end_marker = strstr( cursor, RECORD_DELIM );
        if ( end_marker )
            *end_marker = '\0';

        app->NDat.Body = strdup( cursor );
        if ( !app->NDat.Body ) {
            free( buffer );
            fclose( In );
            fprintf( stderr, "[ERROR] memory allocation\n" );
            exit( EXIT_FAILURE );
        }
    }

    free( buffer );
    fclose( In );
}

// --------------------------------------
/***** Write notes on the file *****/
static void write_file( FILE *Out, TreeNode *root, NotesData *NDat ) {
    root->data.start = ftell( Out );

    if ( fprintf( Out,
                  "%s" FIELD_DELIM "%s" FIELD_DELIM "%s" FIELD_DELIM "%s" FIELD_DELIM
                  "%s" FIELD_DELIM "%s" FIELD_DELIM "%d" FIELD_DELIM,
                  NDat->Tag, NDat->Comment, NDat->Keywords, NDat->Link_File, NDat->Date, NDat->Iv,
                  NDat->Protection ? 1 : 0 ) < 0 ) {
        fprintf( stderr, "[ERROR] file write failed\n" );
        exit( EXIT_FAILURE );
    }

    if ( NDat->Body != NULL ) {
        if ( fprintf( Out, "%s", NDat->Body ) < 0 ) {
            fprintf( stderr, "[ERROR] file write failed\n" );
            exit( EXIT_FAILURE );
        }
    }

    if ( fprintf( Out, RECORD_DELIM ) < 0 ) {
        fprintf( stderr, "[ERROR] file write failed\n" );
        exit( EXIT_FAILURE );
    }

    root->data.end = ftell( Out );
}

// --------------------------------------
/***** Read notes from the file (for writing) *****/
static void read_file( FILE *In, int start, int end, NotesData *NDat ) {
    fseek( In, start, SEEK_SET );

    memset( NDat, 0, sizeof( NotesData ) );
    if ( NDat->Body != NULL ) {
        free( NDat->Body );
        NDat->Body = NULL;
    }

    size_t size = end - start;
    char *buffer = malloc( size + 1 );
    if ( !buffer ) {
        fprintf( stderr, "[ERROR] memory allocation\n" );
        exit( EXIT_FAILURE );
    }

    fread( buffer, 1, size, In );
    buffer[size] = '\0';

    char *cursor = buffer;
    char *token;

    token = next_field( &cursor );
    if ( token )
        strncpy( NDat->Tag, token, sizeof( NDat->Tag ) - 1 );

    token = next_field( &cursor );
    if ( token )
        strncpy( NDat->Comment, token, sizeof( NDat->Comment ) - 1 );

    token = next_field( &cursor );
    if ( token )
        strncpy( NDat->Keywords, token, sizeof( NDat->Keywords ) - 1 );

    token = next_field( &cursor );
    if ( token )
        strncpy( NDat->Link_File, token, sizeof( NDat->Link_File ) - 1 );

    token = next_field( &cursor );
    if ( token )
        strncpy( NDat->Date, token, sizeof( NDat->Date ) - 1 );

    token = next_field( &cursor );
    if ( token )
        strncpy( NDat->Iv, token, sizeof( NDat->Iv ) - 1 );

    token = next_field( &cursor );
    if ( token )
        NDat->Protection = ( atoi( token ) != 0 );

    if ( cursor && *cursor ) {
        char *end_marker = strstr( cursor, RECORD_DELIM );
        if ( end_marker )
            *end_marker = '\0';

        NDat->Body = strdup( cursor );
        if ( !NDat->Body ) {
            free( buffer );
            fprintf( stderr, "[ERROR] memory allocation\n" );
            exit( EXIT_FAILURE );
        }
    }

    free( buffer );
}

// --------------------------------------
/***** Traverses the tree structure to read and write data to the file *****/
static void scroll_tree( TreeNode *root, NotesData *tmpNDat, FILE *In, FILE *Out, AppGlobal *app ) {

    if ( root == NULL ) {
        return;
    }

    if ( root->data.end == 0 )
        copy_ndat( &app->NDat, tmpNDat );

    else
        read_file( In, root->data.start, root->data.end, &app->NDat );

    if ( root->data.end != -1 )
        write_file( Out, root, &app->NDat );

    scroll_tree( root->firstChild, tmpNDat, In, Out, app );
    scroll_tree( root->nextSibling, tmpNDat, In, Out, app );
}

// --------------------------------------
/***** Rewrites the notes file with the changes *****/
void save_note( NotesData *tmpNDat, AppGlobal *app ) {

    FILE *In;
    FILE *Out;

    char old[201];

    strcpy( old, app->cfg.file_note );
    strcat( old, "_old" );
    rename( app->cfg.file_note, old );

    if ( ( In = fopen( old, "rb" ) ) == NULL ) {
        fprintf( stderr, "[ERROR] file \"%s\" opening failed\n", old );
        exit( EXIT_FAILURE );
    }

    if ( ( Out = fopen( app->cfg.file_note, "wb" ) ) == NULL ) {
        fprintf( stderr, "[ERROR] file \"%s\" creation failed\n", app->cfg.file_note );
        exit( EXIT_FAILURE );
    }

    scroll_tree( app->root, tmpNDat, In, Out, app );

    fclose( In );
    fclose( Out );

    remove( old );
}

// --------------------------------------
/***** Copy the current note file *****/
void copy_file( char *original_file, char *new_file ) {

    FILE *In;
    FILE *Out;

    if ( ( In = fopen( original_file, "rb" ) ) == NULL ) {
        fprintf( stderr, "[ERROR] file \"%s\" opening failed\n", original_file );
        exit( EXIT_FAILURE );
    }

    if ( ( Out = fopen( new_file, "wb" ) ) == NULL ) {
        fprintf( stderr, "[ERROR] file \"%s\" creation failed\n", new_file );
        exit( EXIT_FAILURE );
    }

    char ch = fgetc( In );

    while ( !feof( In ) ) {
        fputc( ch, Out );
        ch = fgetc( In );
    }

    fclose( In );
    fclose( Out );
}
