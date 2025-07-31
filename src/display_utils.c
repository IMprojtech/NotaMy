
/*
 * #################################################
 *
 *      Description:
 * Contains functions for searching and printing data and nodes
 *
 *      License:
 * This program is distributed under the terms of the GNU General Public License (GPL),
 * ensuring the freedom to redistribute and modify the software in accordance with open-source standards.
 *
 *      Version:  1.0
 *      Created:  26/07/2025
 *
 *      Author:
 * Catoni Mirko (IMprojtech)
 *
 * #################################################
 */

#define MAX_KEYWORDS 300
#define MAX_WORDS 50
#define MAX_WORD_LEN 50

#define MAX_DEPTH 1000
static int __last_flags[MAX_DEPTH] = { 0 };

// --------------------------------------
/***** Print the graphics for the tree nodes *****/
#define BRANCH( depth, node, app )                                                                 \
    do {                                                                                           \
        printf( "%s%s", ( app )->stl.color_generic, CSI "0m" );                                    \
        for ( int lvl = 0; lvl < ( depth ) - 1; lvl++ ) {                                          \
            if ( __last_flags[lvl] )                                                               \
                printf( "    " );                                                                  \
            else                                                                                   \
                printf( "│   " );                                                                  \
        }                                                                                          \
        if ( ( depth ) > 0 ) {                                                                     \
            int is_last = ( ( node )->nextSibling == NULL );                                       \
            printf( is_last ? "└── " : "├── " );                                                   \
            __last_flags[( depth ) - 1] = is_last;                                                 \
        }                                                                                          \
    } while ( 0 )

#define BRANCH_SPACE( depth, app )                                                                 \
    printf( "%s%s", ( app )->stl.color_generic, CSI "0m" );                                        \
    for ( int lvl = 0; lvl < ( depth ) - 1; lvl++ ) {                                              \
        if ( __last_flags[lvl] )                                                                   \
            printf( "    " );                                                                      \
        else                                                                                       \
            printf( "│   " );                                                                      \
    }                                                                                              \
    printf( ( depth ) > 0 ? "│   " : "    " );

// --------------------------------------
/***** Print body *****/
static void print_body( char *str, int depth, AppGlobal *app ) {
    char *token = str;
    char *newline;
    while ( ( newline = strchr( token, '\n' ) ) != NULL ) {
        *newline = '\0';
        printf( "%s%s%s\n", app->stl.color_body, token, CSI "0m" );
        *newline = '\n';
        BRANCH_SPACE( depth, app );
        token = newline + 1;
    }
    printf( "%s%s%s", app->stl.color_body, token, CSI "0m" );
}

// --------------------------------------
/***** Print file *****/
void print_file( AppGlobal *app ) {
    NotesData *nd = &app->NDat;
    if ( app->opts.with_flag_IO || ( strcasecmp( app->cfg.editor, "Nul" ) == 0 ) ) {
        FILE *fp = fopen( nd->Link_File, "rb" );
        if ( !fp ) {
            fprintf( stderr, "[ERROR] file \"%s\" opening failed\n", nd->Link_File );
            exit( EXIT_FAILURE );
        }
        int ch;
        printf( "\n" );
        while ( ( ch = fgetc( fp ) ) != EOF )
            putchar( ch );
        printf( "\n" );
        fclose( fp );
    }

    else if ( strcasecmp( app->cfg.editor, "Vim" ) == 0 ) {
        execl( "/bin/vim", "vim", nd->Link_File, NULL );
    } else if ( strcasecmp( app->cfg.editor, "Nano" ) == 0 ) {
        execl( "/bin/nano", "nano", nd->Link_File, NULL );
    } else {
        fprintf( stderr, "[ERROR] illegal Editor\n" );
        exit( EXIT_FAILURE );
    }
}

// --------------------------------------
/***** Print data from a node *****/
void print_node( AppGlobal *app, TreeNode *node, int depth ) {
    NotesData *nd = &app->NDat;
    if ( app->opts.with_file_flag ) {
        print_file( app );
        return;
    }
    if ( !app->opts.with_body ) {
        BRANCH( depth, node, app );
        printf( "%s%s %s%s%s ", app->stl.color_tag, node->data.tag, app->stl.color_comment,
                nd->Comment, CSI "0m" );
        if ( !app->opts.with_extended ) {
            if ( strlen( nd->Link_File ) )
                printf( "%s#%s ", app->stl.color_file, CSI "0m" );
            if ( nd->Body && strlen( nd->Body ) )
                printf( "%s#%s ", app->stl.color_body, CSI "0m" );
            if ( strlen( nd->Keywords ) )
                printf( "%s#%s ", app->stl.color_keywords, CSI "0m" );
            printf( "\n" );
        } else {
            if ( strlen( nd->Link_File ) )
                printf( "%s%s%s ", app->stl.color_file, nd->Link_File, CSI "0m" );
            if ( nd->Body && strlen( nd->Body ) )
                printf( "%s#%s ", app->stl.color_body, CSI "0m" );
            if ( strlen( nd->Keywords ) )
                printf( "%s#%s ", app->stl.color_keywords, CSI "0m" );
            printf( "%s%s %s%s%s\n", app->stl.color_hash, node->data.hash, app->stl.color_date,
                    node->data.date, CSI "0m" );
        }
    } else {
        BRANCH_SPACE( depth, app );
        printf( "\n" );
        BRANCH( depth, node, app );
        printf( "%s[%s]%s\n", app->stl.color_tag, node->data.tag, CSI "0m" );
        if ( strlen( nd->Comment ) ) {
            BRANCH_SPACE( depth, app );
            printf( "%s%s%s\n", app->stl.color_comment, nd->Comment, CSI "0m" );
        }
        if ( nd->Body && strlen( nd->Body ) ) {
            BRANCH_SPACE( depth, app );
            print_body( nd->Body, depth, app );
            printf( "\n" );
        }
        if ( strlen( nd->Link_File ) ) {
            BRANCH_SPACE( depth, app );
            printf( "%s%s%s\n", app->stl.color_file, nd->Link_File, CSI "0m" );
        }
        if ( app->opts.with_extended ) {

            if ( strlen( nd->Keywords ) > 1 ) {
                BRANCH_SPACE( depth, app );
                printf( "%s%s%s\n", app->stl.color_keywords, nd->Keywords, CSI "0m" );
            }

            if ( strlen( node->data.hash ) > 1 ) {
                BRANCH_SPACE( depth, app );
                printf( "%s%s%s\n", app->stl.color_hash, node->data.hash, CSI "0m" );
            }

            if ( strlen( node->data.date ) > 1 ) {
                BRANCH_SPACE( depth, app );
                printf( "%s%s%s\n", app->stl.color_date, node->data.date, CSI "0m" );
            }
        }
    }
}

// --------------------------------------
/***** Print data from all nodes *****/
void print_all( TreeNode *root, int depth, AppGlobal *app, char *Passwd, char *Key ) {
    if ( !root )
        return;
    read_dat( root->data.start, root->data.end, app );
    if ( app->NDat.Protection ) {
        init_ctx_from_ndat( &app->ctx, &app->NDat );
        if ( app->opts.with_protection )
            decrypt( Passwd, &app->ctx, Key );
        else
            mask( &app->ctx );
    }
    print_node( app, root, depth );
    print_all( root->firstChild, depth + 1, app, Passwd, Key );
    print_all( root->nextSibling, depth, app, Passwd, Key );
}

void print_list( TreeNode *root, int depth, AppGlobal *app, char *Passwd, char *Key ) {
    if ( !root )
        return;
    read_dat( root->data.start, root->data.end, app );
    if ( app->NDat.Protection ) {
        init_ctx_from_ndat( &app->ctx, &app->NDat );
        if ( app->opts.with_protection )
            decrypt( Passwd, &app->ctx, Key );
        else
            mask( &app->ctx );
    }
    print_node( app, root, depth );
    print_list( root->nextSibling, depth, app, Passwd, Key );
}

// --------------------------------------
/***** Print the nodes found by the search (all data) *****/
void print_find( TreeNode *root, char *key, find_function fn, AppGlobal *app, char *Passwd,
                 char *Key ) {
    if ( !root )
        return;
    if ( fn( root, key ) == 0 ) {
        read_dat( root->data.start, root->data.end, app );
        if ( app->NDat.Protection ) {
            init_ctx_from_ndat( &app->ctx, &app->NDat );
            if ( app->opts.with_protection )
                decrypt( Passwd, &app->ctx, Key );
            else
                mask( &app->ctx );
        }
        print_node( app, root, 0 );
    }
    print_find( root->firstChild, key, fn, app, Passwd, Key );
    print_find( root->nextSibling, key, fn, app, Passwd, Key );
}

// --------------------------------------
/***** Print child nodes *****/
void print_children( TreeNode *parent, AppGlobal *app, char *Passwd, char *Key ) {
    if ( !parent || !parent->firstChild )
        return;
    read_dat( parent->data.start, parent->data.end, app );
    if ( app->NDat.Protection ) {
        init_ctx_from_ndat( &app->ctx, &app->NDat );
        if ( app->opts.with_protection )
            decrypt( Passwd, &app->ctx, Key );
        else
            mask( &app->ctx );
    }
    print_node( app, parent, 0 );
    print_all( parent->firstChild, 1, app, Passwd, Key );
}

// --------------------------------------
/***** Print sibling nodes *****/
void print_sibling( TreeNode *parent, AppGlobal *app, char *Passwd, char *Key ) {
    if ( !parent || !parent->firstChild )
        return;
    read_dat( parent->data.start, parent->data.end, app );
    if ( app->NDat.Protection ) {
        init_ctx_from_ndat( &app->ctx, &app->NDat );
        if ( app->opts.with_protection )
            decrypt( Passwd, &app->ctx, Key );
        else
            mask( &app->ctx );
    }
    print_node( app, parent, 0 );
    print_list( parent->firstChild, 1, app, Passwd, Key );
}

// --------------------------------------
/***** Print the nodes found by the search (structure only) *****/
void print_find_node( TreeNode *root, char *key, find_function fn, AppGlobal *app ) {
    if ( !root )
        return;

    if ( fn( root, key ) == 0 ) {
        read_dat( root->data.start, root->data.end, app );
        printf( "%s%s %s%s %s%s%s\n", app->stl.color_tag, root->data.tag, app->stl.color_hash,
                root->data.hash, app->stl.color_file, strlen( app->NDat.Link_File ) ? "#" : "",
                CSI "0m" );
    }
    print_find_node( root->firstChild, key, fn, app );
    print_find_node( root->nextSibling, key, fn, app );
}

// --------------------------------------
/***** Print structure without data *****/
void print_tree( TreeNode *root, int depth, AppGlobal *app ) {
    if ( !root )
        return;
    BRANCH( depth, root, app );
    printf( "%s%s %s%s%s \n", app->stl.color_tag, root->data.tag, app->stl.color_hash,
            app->opts.with_extended ? root->data.hash : "", CSI "0m" );
    print_tree( root->firstChild, depth + 1, app );
    print_tree( root->nextSibling, depth, app );
}

// --------------------------------------
/***** Search nodes by date and print them *****/
static void print_find_date( TreeNode *root, search_function searchFunc, long *start, long *end,
                             AppGlobal *app ) {
    if ( !root )
        return;

    Date_Time dt = string_to_date( root->data.date );
    long timestamp = date_to_seconds( dt );
    if ( searchFunc( timestamp, *end, *start ) ) {
        read_dat( root->data.start, root->data.end, app );
        print_node( app, root, 0 );
    }

    if ( root->firstChild )
        print_find_date( root->firstChild, searchFunc, start, end, app );
    if ( root->nextSibling )
        print_find_date( root->nextSibling, searchFunc, start, end, app );
}

// --------------------------------------
/***** Start search by date *****/
void process_data( TreeNode *root, AppGlobal *app ) {

    long start, end;
    search_function searchFunc = parse_search_date( app->opts.arg_date, &start, &end );

    if ( !root )
        return;

    if ( root->firstChild )
        print_find_date( root->firstChild, searchFunc, &start, &end, app );
    if ( root->nextSibling )
        print_find_date( root->nextSibling, searchFunc, &start, &end, app );
}

// --------------------------------------
/***** Tokenize a string into space-separated words *****/
static int tokenize( char *input, char tokens[MAX_WORDS][MAX_WORD_LEN] ) {
    int count = 0;
    char *token = strtok( input, " " );
    while ( token && count < MAX_WORDS ) {
        strncpy( tokens[count++], token, MAX_WORD_LEN - 1 );
        token = strtok( NULL, " " );
    }
    return count;
}

// --------------------------------------
/***** Check that all keywords are found *****/
static bool match_all_keywords( const char *keywords, const char *search ) {
    if ( !keywords )
        return false;

    char key_tokens[MAX_WORDS][MAX_WORD_LEN];
    char search_tokens[MAX_WORDS][MAX_WORD_LEN];

    char kw_copy[MAX_KEYWORDS];
    char search_copy[MAX_KEYWORDS];

    strncpy( kw_copy, keywords, MAX_KEYWORDS - 1 );
    strncpy( search_copy, search, MAX_KEYWORDS - 1 );

    int key_count = tokenize( kw_copy, key_tokens );
    int search_count = tokenize( search_copy, search_tokens );

    for ( int i = 0; i < search_count; ++i ) {
        bool found = false;
        for ( int j = 0; j < key_count; ++j ) {
            if ( strncasecmp( search_tokens[i], key_tokens[j], strlen( search_tokens[i] ) ) == 0 ) {
                found = true;
                break;
            }
        }
        if ( !found )
            return false;
    }
    return true;
}

// --------------------------------------
/***** Search for nodes by keywords and print them *****/
void print_find_keywords( TreeNode *root, const char *search, AppGlobal *app ) {

    if ( !root )
        return;

    read_dat( root->data.start, root->data.end, app );
    if ( match_all_keywords( app->NDat.Keywords, search ) )
        print_node( app, root, 0 );

    if ( root->firstChild )
        print_find_keywords( root->firstChild, search, app );
    if ( root->nextSibling )
        print_find_keywords( root->nextSibling, search, app );
}

// --------------------------------------
/***** Helper *****/
void help( void ) {
    printf( "\nNAME :\n    %s - %s\n\n", NAME, ALIAS );

    printf( "SYNOPSIS :\n"
            "    %s [COMMAND] [SUBCOMMAND] [OPTIONS]\n"
            "    %s [COMMAND] [OPTIONS] \n\n",
            ALIAS, ALIAS );

    printf( "DESCRIPTION :\n"
            "    Terminal-based note manager with hierarchical tagging and file linking\n\n" );

    printf( "COMMAND list : \n"
            "   add        Add a note or note file. \n"
            "   view       Show notes or tags or note file list. \n"
            "   find        Search and print specific notes.\n"
            "   modify     Edit existing note. \n"
            "   organize   Change the position of the note. \n"
            "   remove     Remove a note. \n"
            "   setting    Change the note file in use. \n"
            "   editor     Change editor used. \n"
            "   backup     Run backup. \n"
            "   help       Complete guide. \n\n" );

    printf( "COMMON OPTIONS :\n"
            "  -t, --tag            Specifies the tag\n"
            "  -c, --comment        Adds a comment to the note\n"
            "  -k, --keywords       Keywords separated by spaces\n"
            "  -f, --file           Attaches or opens a file \n"
            "  -b, --body           Writes or displays the body of the note\n"
            "  -i, --input          Reads the body of the note from stdin\n"
            "  -p, --protection     Protects the note with encryption\n"
            "  -e, --extended       Extended view \n"
            "  -o, --output         Prints file contents to stdout\n\n" );

    printf( "EXAMPLE :\n"
            "  %s add note -t \"log\" -c \"Backup\" -k \"cron\" -b\n"
            "  %s find -t \"projects\" -k \"API\"\n"
            "  %s view tag -e\n"
            "  %s organize <hash> up\n"
            "  %s remove -h <hash>\n\n",
            ALIAS, ALIAS, ALIAS, ALIAS, ALIAS );

    printf( "NOTE :\n"
            "  - For configuration: ~/.config/NotaMy.conf\n"
            "  - Running the program as `ntm_nc` enables **no color** mode\n\n" );

    printf( "!! For complete details on each command, refer to the documentation "
            "available on GitHub.\n\n" );

    printf( "AUTHOR :\n    Written by Catoni Mirko (IMprojtech)\n\n" );

    printf( "VERSION\n    %s\n\n", VER );
}
