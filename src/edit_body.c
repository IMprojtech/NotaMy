
/*
 * #################################################
 *
 *      Description:
 * Contains the logic for writing Body
 *
 *      License:
 * This program is distributed under the terms of the GNU General Public License (GPL),
 * ensuring the freedom to redistribute and modify the software in accordance with open-source standards.
 *
 *  	Version:  1.0
 *		Created:  24/07/2025
 *
 *  	Author:
 * Catoni Mirko (IMprojtech)
 *
 * #################################################
 */

static void dinamic_write( char **Body ) {

    int max_length = 160;
    int length = 0;
    int ch;

    *Body = (char *)malloc( max_length * sizeof( char ) );

    if ( *Body == NULL ) {
        fprintf( stderr, "[ERROR] memory allocation\n" );
        exit( EXIT_FAILURE );
    }

    while ( ( ch = getchar() ) != EOF ) {
        ( *Body )[length] = ch;
        length++;

        if ( length >= max_length ) {
            max_length *= 2;
            *Body = (char *)realloc( *Body, max_length * sizeof( char ) );

            if ( *Body == NULL ) {

                fprintf( stderr, "[ERROR] memory allocation\n" );
                exit( EXIT_FAILURE );
            }
        }
    }

    ( *Body )[length] = '\0';
}

void edit( AppGlobal *app ) {

    if ( app->opts.with_flag_IO || ( strcasecmp( app->cfg.editor, "Nul" ) == 0 ) ) {
        dinamic_write( &app->NDat.Body );
        return;
    }

    FILE *fp;

    if ( ( fp = fopen( "Edit.tmp", "wb" ) ) == NULL ) {
        fprintf( stderr, "[ERROR] file Edit.tmp creation failed\n" );
        exit( EXIT_FAILURE );
    }

    if ( app->NDat.Body != NULL ) {
        int check = fwrite( app->NDat.Body, sizeof( char ), strlen( app->NDat.Body ) + 1, fp );
        if ( check == 0 ) {
            fprintf( stderr, "[ERROR] file Edit.tmp write failed\n" );
            exit( EXIT_FAILURE );
        }
    }

    fclose( fp );
    pid_t pid = fork();

    if ( pid == 0 ) {
        if ( strcasecmp( app->cfg.editor, "Vim" ) == 0 ) {
            execl( "/bin/vim", "vim", "Edit.tmp", NULL );
        } else if ( strcasecmp( app->cfg.editor, "Nano" ) == 0 ) {
            execl( "/bin/nano", "nano", "Edit.tmp", NULL );
        }
    }

    else if ( pid > 0 ) {
        wait( NULL );
    } else {
        fprintf( stderr, "[ERROR] process\n" );
        exit( EXIT_FAILURE );
    }

    if ( ( fp = fopen( "Edit.tmp", "rb" ) ) == NULL ) {
        fprintf( stderr, "[ERROR] file Edit.tmp opening failed\n" );
        exit( EXIT_FAILURE );
    }

    size_t newSize = 0;
    int ch;

    while ( ( ch = fgetc( fp ) ) != EOF ) {
        newSize++;
    }

    app->NDat.Body = (char *)malloc( ( newSize + 2 ) * sizeof( char ) );

    if ( app->NDat.Body == NULL ) {
        fprintf( stderr, "[ERROR] memory allocation\n" );
        exit( EXIT_FAILURE );
    }

    rewind( fp );

    int i;

    for ( i = 0; i < newSize; ++i ) {
        ch = fgetc( fp );
        app->NDat.Body[i] = ch;
    }

    app->NDat.Body[i] = '\0';

    fclose( fp );
    remove( "Edit.tmp" );
}
