
/*
 * #################################################
 *
 *      Description:
 * Contains generic functions:
 * initialization of structures,
 * date and time calculation,
 * tag analysis,
 * etc.
 *
 *      License:
 * This program is distributed under the terms of the GNU General Public License (GPL),
 * ensuring the freedom to redistribute and modify the software in accordance with open-source standards.
 *
 *  	Version:  1.0
 *		Created:  23/07/2025
 *
 *  	Author:
 * Catoni Mirko (IMprojtech)
 *
 * #################################################
 */

// --------------------------------------
/***** Initialize settings from setup file *****/
void init_setting( char *set_file, char *pass, AppGlobal *app ) {
    FILE *fp;
    const char *home = getenv( "HOME" );

    strcpy( set_file, home );
    strcat( set_file, DIR_SET );
    mkdir( set_file, 0755 );
    strcat( set_file, SET_FILE );

    strcpy( app->cfg.file_note, home );
    strcat( app->cfg.file_note, DEFAULT_FILE_NOTE );

    default_config( &app->cfg, &app->stl );

    if ( ( fp = fopen( set_file, "r" ) ) == NULL ) {
        write_key( pass, sizeof( pass ) );
        generate_SHA512( pass, app->cfg.hash_pass );

        write_config( set_file, &app->cfg, &app->stl );
        exit( 0 );
    }

    load_config( set_file, &app->cfg, &app->stl );
}

// --------------------------------------
/***** Initialize structure BlockInfo *****/
void init_blockinfo( BlockInfo *data ) {
    data->start = 0;
    data->end = 0;
    strcpy( data->hash, "." );
    strcpy( data->tag, "." );
    strcpy( data->date, "." );
}

// --------------------------------------
/***** Search for separators in the tag *****/
static bool check_tag( const char *tag ) {
    if ( !tag )
        return false;

    const char *sep = strchr( tag, '>' );
    if ( !sep )
        sep = strchr( tag, '<' );

    if ( !sep ) {
        if ( strlen( tag ) > 24 ) {
            return false;
        }
        return true;
    }

    size_t len1 = sep - tag;
    size_t len2 = strlen( sep + 1 );

    if ( len1 > 24 || len2 > 24 ) {
        return false;
    }

    return true;
}

// --------------------------------------
/***** Copies data from "Options" to "NotesData" *****/
void init_ndat_from_opts( NotesData *n, Options *o ) {

    if ( o->arg_tag ) {
        if ( !check_tag( o->arg_tag ) ) {
            fprintf( stderr, "Error: 'Tag' too long (max %zu characters)\n",
                     ( sizeof( n->Tag ) / 2 ) - 1 );
            exit( 1 );
        }
        strcpy( n->Tag, o->arg_tag );
    }

    if ( o->arg_comment ) {
        if ( strlen( o->arg_comment ) >= sizeof( n->Comment ) ) {
            fprintf( stderr, "Error: 'Comment' too long (max %zu characters)\n",
                     sizeof( n->Comment ) - 1 );
            exit( 1 );
        }
        strcpy( n->Comment, o->arg_comment );
    }

    if ( o->arg_keywords ) {
        if ( strlen( o->arg_keywords ) >= sizeof( n->Keywords ) ) {
            fprintf( stderr, "Error: 'Keywords' too long (max %zu characters)\n",
                     sizeof( n->Keywords ) - 1 );
            exit( 1 );
        }
        strcpy( n->Keywords, o->arg_keywords );
    }

    if ( o->arg_filepath ) {
        if ( strlen( o->arg_filepath ) >= sizeof( n->Link_File ) ) {
            fprintf( stderr, "Error: 'Link_File' too long (max %zu characters)\n",
                     sizeof( n->Link_File ) - 1 );
            exit( 1 );
        }
        strcpy( n->Link_File, o->arg_filepath );
    }

    if ( o->arg_date ) {
        if ( strlen( o->arg_date ) >= sizeof( n->Date ) ) {
            fprintf( stderr, "Error: 'Date' too long (max %zu characters)\n",
                     sizeof( n->Date ) - 1 );
            exit( 1 );
        }
        strcpy( n->Date, o->arg_date );
    }

    n->Protection = o->with_protection;
}

// --------------------------------------
/***** Copy data from "NotesData" to "Protect" *****/
void init_ctx_from_ndat( Protect *p, NotesData *n ) {
    p->comment = n->Comment;
    p->file = n->Link_File;
    p->body = n->Body;
    p->iv = n->Iv;
    p->protection = &n->Protection;
}

// --------------------------------------
/***** Split the tag at the separator *****/
static void tokenizer( char *_Tag, char *_InTag, AppGlobal *app ) {
    char *token = strtok( app->opts.arg_tag, "<>" );

    if ( token != NULL ) {
        strcpy( _Tag, token );

        token = strtok( NULL, "<>" );
        if ( token != NULL ) {
            strcpy( _InTag, token );
            return;
        }
    }
}

// --------------------------------------
/***** Check if tag contains separator characters *****/
void analyze_string_tag( char *_InTag, AppGlobal *app ) {

    if ( strchr( app->opts.arg_tag, '<' ) != NULL && strchr( app->opts.arg_tag, '>' ) != NULL ) {
        fprintf( stderr, "tag format error\n" );
        exit( 1 );
    }

    else if ( strchr( app->opts.arg_tag, '<' ) != NULL )
        tokenizer( _InTag, app->NDat.Tag, app );
    else if ( strchr( app->opts.arg_tag, '>' ) != NULL )
        tokenizer( app->NDat.Tag, _InTag, app );
    else
        strcpy( app->NDat.Tag, app->opts.arg_tag );
    return;
}

// --------------------------------------
/***** Calculate date and time *****/
void take_time( NotesData *NDat ) {
    time_t currentTime;
    struct tm *localTime;
    time( &currentTime );
    localTime = localtime( &currentTime );
    strftime( NDat->Date, sizeof( NDat->Date ), "%Y-%m-%d %H:%M:%S", localTime );
}

// --------------------------------------
/***** Generates a hash associated with the note *****/
void generate_sha1( const NotesData *data, AppGlobal *app ) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    unsigned char buffer[sizeof( NotesData )];
    memcpy( buffer, data, sizeof( NotesData ) );

    SHA1( buffer, sizeof( NotesData ), hash );
    for ( size_t i = 0; i < SHA_DIGEST_LENGTH; i++ ) {
        sprintf( app->data.hash + i * 2, "%02x", hash[i] );
    }
    app->data.hash[SHA_DIGEST_LENGTH * 2] = '\0';
}

// --------------------------------------
/***** Copy NotesData structure *****/
void copy_ndat( NotesData *dest, NotesData *src ) {

    strcpy( dest->Tag, src->Tag );
    strcpy( dest->Comment, src->Comment );
    strcpy( dest->Keywords, src->Keywords );
    strcpy( dest->Link_File, src->Link_File );
    strcpy( dest->Date, src->Date );
    strcpy( dest->Iv, src->Iv );
    dest->Protection = src->Protection;

    if ( src->Body ) {
        dest->Body = malloc( strlen( src->Body ) + 1 );
        if ( dest->Body ) {
            strcpy( dest->Body, src->Body );
            free( src->Body );
            src->Body = NULL;
        } else {
            fprintf( stderr, "[ERROR] memory allocation\n" );
            exit( EXIT_FAILURE );
        }
    } else
        dest->Body = NULL;
}
