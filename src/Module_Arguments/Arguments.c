
/*
 * #################################################
 *
 *      Description:
 * Implements the command line parser.
 * Parses main commands, subcommands and options,
 * populates the Options structure and handles errors and usage messages.
 *
 *      License:
 * This program is distributed under the terms of the GNU General Public License (GPL),
 * ensuring the freedom to redistribute and modify the software in accordance with open-source standards.
 *
 *      Version:  1.0
 *      Created:  03/07/2025
 *
 *      Author:
 * Catoni Mirko (IMprojtech)
 *
 * #################################################
 */

#include "Arguments.h"

// --------------------------------------
/* Command handler declarations */
static void cmd_add( int argc, char *argv[], Options *opts );
static void cmd_view( int argc, char *argv[], Options *opts );
static void cmd_find( int argc, char *argv[], Options *opts );
static void cmd_modify( int argc, char *argv[], Options *opts );
static void cmd_organize( int argc, char *argv[], Options *opts );
static void cmd_remove( int argc, char *argv[], Options *opts );
static void cmd_setting( int argc, char *argv[], Options *opts );
static void cmd_editor( int argc, char *argv[], Options *opts );
static void cmd_backup( int argc, char *argv[], Options *opts );
static void cmd_help( int argc, char *argv[], Options *opts );

/* handler declarations for 'add' subcommands */
static void add_note( int argc, char *argv[], Options *opts );
static void add_file( int argc, char *argv[], Options *opts );

/* handler declarations for 'view' subcommands */
static void view_note( int argc, char *argv[], Options *opts );
static void view_tag( int argc, char *argv[], Options *opts );
static void view_file( int argc, char *argv[], Options *opts );

/* Error handler declarations */
static void usage_error( const char *msg );
static void print_usage( const char *progname, const char *msg );

/* Debug handler declarations */
void print_opts( Options *opts );

// --------------------------------------
/* Dispatch tables */
typedef struct {
    const char *name;
    void ( *func )( int, char **, Options * );
} Commands;

/* Main funcions */
static Commands commands[] = { { "add", cmd_add },
                               { "view", cmd_view },
                               { "find", cmd_find },
                               { "modify", cmd_modify },
                               { "organize", cmd_organize },
                               { "remove", cmd_remove },
                               { "setting", cmd_setting },
                               { "editor", cmd_editor },
                               { "backup", cmd_backup },
                               { "help", cmd_help },
                               { NULL, NULL } };

/* Sub 'add' functions */
static Commands add_subs[] = { { "note", add_note }, { "file", add_file }, { NULL, NULL } };

/* Sub 'view' functions */
static Commands view_subs[] = {
    { "note", view_note }, { "tag", view_tag }, { "file", view_file }, { NULL, NULL } };

/* Macro for duplication errors and extra arguments */
#define SET_STRING_ONCE( field, value, optname )                                                   \
    do {                                                                                           \
        if ( ( field ) != NULL )                                                                   \
            usage_error( "Option --" optname " specified more than once" );                        \
        ( field ) = ( value );                                                                     \
    } while ( 0 )

#define SET_BOOL_ONCE( field, optname )                                                            \
    do {                                                                                           \
        if ( ( field ) )                                                                           \
            usage_error( "Option --" optname " specified more than once" );                        \
        ( field ) = true;                                                                          \
    } while ( 0 )

#define CHECK_EXTRA_ARGS()                                                                         \
    do {                                                                                           \
        if ( optind < argc )                                                                       \
            usage_error( "Too many arguments provided" );                                          \
    } while ( 0 )

// --------------------------------------
/* DEBUG: print contents */
void print_opts( Options *opts ) {
    printf( "\n[OPTIONS STRUCT CONTENT]\n" );
    printf( "Command         : %d\n", opts->cmd );
    printf( "Tag             : %s\n", opts->arg_tag ? opts->arg_tag : "(null)" );
    printf( "Comment         : %s\n", opts->arg_comment ? opts->arg_comment : "(null)" );
    printf( "Filepath        : %s\n", opts->arg_filepath ? opts->arg_filepath : "(null)" );
    printf( "Keywords        : %s\n", opts->arg_keywords ? opts->arg_keywords : "(null)" );
    printf( "Hash            : %s\n", opts->arg_hash ? opts->arg_hash : "(null)" );
    printf( "Generic         : %s\n", opts->arg_generic ? opts->arg_generic : "(null)" );
    printf( "Editor          : %s\n", opts->arg_editor ? opts->arg_editor : "(null)" );
    printf( "Index           : %d\n", opts->arg_index );
    printf( "with_body       : %s\n", opts->with_body ? "true" : "false" );
    printf( "with_IO_flag    : %s\n", opts->with_flag_IO ? "true" : "false" );
    printf( "with_protection : %s\n", opts->with_protection ? "true" : "false" );
    printf( "with_file_flag  : %s\n", opts->with_file_flag ? "true" : "false" );
    printf( "with_extended   : %s\n", opts->with_extended ? "true" : "false" );
    printf( "--------------------------------\n" );
}

// --------------------------------------
/* Helper for reporting usage errors and exiting */
static void usage_error( const char *msg ) {
    fprintf( stderr, "%s\n", msg );
    exit( EXIT_FAILURE );
}

static void print_usage( const char *progname, const char *msg ) {
    fprintf( stderr, "%s \n\n", msg );
    fprintf( stderr,
             "Usage : \n"
             " %s [COMMAND] [SUB COMMAND] [OPTIONS] \n"
             " %s [COMMAND] [OPTIONS] \n\n"
             "COMMAND list : \n"
             "   add        Add a note or note file. \n"
             "   view       Show notes or tags or note file list. \n"
             "   find       Search and print specific notes.\n"
             "   modify     Edit existing note. \n"
             "   organize   Change the position of the note. \n"
             "   remove     Remove a note. \n"
             "   setting    Change the note file in use. \n"
             "   editor     Change editor used. \n"
             "   backup     Run backup. \n"
             "   help       Complete guide. \n",
             progname, progname );

    exit( EXIT_FAILURE );
}

// --------------------------------------
/***** Implementation of add *****/

static void add_note( int argc, char *argv[], Options *opts ) {
    if ( argc < 4 )
        usage_error( "Usage: add note --tag <text> --comment <text> [--file "
                     "<text> | --keywords <text> | --body | --input | --protection]" );

    const char *x = "t:c:f:k:bip";
    struct option long_opts[] = {
        { "tag", required_argument, 0, 't' },  { "comment", required_argument, 0, 'c' },
        { "file", required_argument, 0, 'f' }, { "keywords", required_argument, 0, 'k' },
        { "body", no_argument, 0, 'b' },       { "input", no_argument, 0, 'i' },
        { "protection", no_argument, 0, 'p' }, { 0, 0, 0, 0 } };

    int opt;
    opterr = 0;
    optind = 0;
    while ( ( opt = getopt_long( argc, argv, x, long_opts, NULL ) ) != -1 ) {
        switch ( opt ) {
        case 't':
            SET_STRING_ONCE( opts->arg_tag, optarg, "tag" );
            break;
        case 'c':
            SET_STRING_ONCE( opts->arg_comment, optarg, "comment" );
            break;
        case 'f':
            SET_STRING_ONCE( opts->arg_filepath, optarg, "file" );
            break;
        case 'k':
            SET_STRING_ONCE( opts->arg_keywords, optarg, "keywords" );
            break;
        case 'b':
            SET_BOOL_ONCE( opts->with_body, "body" );
            break;
        case 'i':
            SET_BOOL_ONCE( opts->with_flag_IO, "input" );
            break;
        case 'p':
            SET_BOOL_ONCE( opts->with_protection, "protection" );
            break;
        default:
            usage_error( "add note: illegal option" );
        }
    }

    CHECK_EXTRA_ARGS();

    if ( !opts->arg_tag || !opts->arg_comment )
        usage_error( "add note: --tag and --comment are required" );

    if ( !opts->with_body && opts->with_flag_IO )
        usage_error( "add note: --input only valid with --body" );

    opts->cmd = CMD_ADD_NOTE;
}

static void add_file( int argc, char *argv[], Options *opts ) {
    if ( argc < 2 )
        usage_error( "Usage: add file <path> [--comment <text>]" );

    opts->arg_filepath = argv[1];

    const char *x = "c:";
    struct option long_opts[] = { { "comment", required_argument, 0, 'c' }, { 0, 0, 0, 0 } };
    argc--;
    argv++;

    int opt;
    opterr = 0;
    optind = 0;
    while ( ( opt = getopt_long( argc, argv, x, long_opts, NULL ) ) != -1 ) {
        if ( opt == 'c' )
            opts->arg_comment = optarg;
        else
            usage_error( "add file: illegal option" );
    }

    CHECK_EXTRA_ARGS();

    opts->cmd = CMD_ADD_FILE;
}

static void cmd_add( int argc, char *argv[], Options *opts ) {
    if ( argc < 2 )
        usage_error( "Usage: add <note|file> [options]" );

    const char *sub = argv[1];
    for ( int i = 0; add_subs[i].name; i++ ) {
        if ( !strcmp( sub, add_subs[i].name ) ) {
            add_subs[i].func( argc - 1, argv + 1, opts );
            return;
        }
    }

    usage_error( "Unknown 'add' subcommand" );
}

// --------------------------------------
/***** Implementation of view *****/

static void view_note( int argc, char *argv[], Options *opts ) {
    if ( argc < 1 )
        usage_error( "Usage: view note --body | --extended | --protection]" );

    const char *x = "bep";
    struct option long_opts[] = { { "body", no_argument, 0, 'b' },
                                  { "extended", no_argument, 0, 'e' },
                                  { "protection", no_argument, 0, 'p' },
                                  { 0, 0, 0, 0 } };

    int opt;
    opterr = 0;
    optind = 0;
    while ( ( opt = getopt_long( argc, argv, x, long_opts, NULL ) ) != -1 ) {
        switch ( opt ) {
        case 'b':
            SET_BOOL_ONCE( opts->with_body, "body" );
            break;
        case 'e':
            SET_BOOL_ONCE( opts->with_extended, "extended" );
            break;
        case 'p':
            SET_BOOL_ONCE( opts->with_protection, "protection" );
            break;
        default:
            usage_error( "view note: illegal option" );
        }
    }

    CHECK_EXTRA_ARGS();

    opts->cmd = CMD_VIEW_NOTE;
}

static void view_tag( int argc, char *argv[], Options *opts ) {
    if ( argc > 2 )
        usage_error( "Usage: view tag [--extended]" );

    const char *x = "e";
    struct option long_opts[] = { { "extended", no_argument, 0, 'e' }, { 0, 0, 0, 0 } };

    int opt;
    opterr = 0;
    optind = 0;
    while ( ( opt = getopt_long( argc, argv, x, long_opts, NULL ) ) != -1 ) {
        if ( opt == 'e' )
            SET_BOOL_ONCE( opts->with_extended, "extended" );
        else
            usage_error( "view tag: illegal option" );
    }

    CHECK_EXTRA_ARGS();

    opts->cmd = CMD_VIEW_TAG;
}

static void view_file( int argc, char *argv[], Options *opts ) {
    if ( argc != 1 )
        usage_error( "Usage: view file" );

    opts->cmd = CMD_VIEW_FILE;
}

static void cmd_view( int argc, char *argv[], Options *opts ) {
    if ( argc < 2 )
        usage_error( "Usage: view <note|tag|file> [options]" );

    const char *sub = argv[1];
    for ( int i = 0; view_subs[i].name; i++ ) {
        if ( !strcmp( sub, view_subs[i].name ) ) {
            view_subs[i].func( argc - 1, argv + 1, opts );
            return;
        }
    }

    usage_error( "Unknown 'view' subcommand" );
}

// --------------------------------------
/***** Implementation of find  *****/

static void cmd_find( int argc, char *argv[], Options *opts ) {
    if ( argc < 2 )
        usage_error( "Usage: find [--tag <text> | --hash <text>  | --date <text> | --keywords "
                     "<text> | --file "
                     "| --body | --output | --extended | --protection]" );

    const char *x = "t:h:d:k:fboep";
    struct option long_opts[] = {
        { "tag", required_argument, 0, 't' },  { "hash", required_argument, 0, 'h' },
        { "date", required_argument, 0, 'd' }, { "keywords", required_argument, 0, 'k' },
        { "file", no_argument, 0, 'f' },       { "body", no_argument, 0, 'b' },
        { "output", no_argument, 0, 'o' },     { "extended", no_argument, 0, 'e' },
        { "protection", no_argument, 0, 'p' }, { 0, 0, 0, 0 } };

    int opt;
    opterr = 0;
    optind = 0;
    while ( ( opt = getopt_long( argc, argv, x, long_opts, NULL ) ) != -1 ) {
        switch ( opt ) {
        case 't':
            SET_STRING_ONCE( opts->arg_tag, optarg, "tag" );
            break;
        case 'h':
            SET_STRING_ONCE( opts->arg_hash, optarg, "hash" );
            break;
        case 'd':
            SET_STRING_ONCE( opts->arg_date, optarg, "date" );
            break;
        case 'k':
            SET_STRING_ONCE( opts->arg_keywords, optarg, "keywords" );
            break;
        case 'f':
            SET_BOOL_ONCE( opts->with_file_flag, "file flag" );
            break;
        case 'b':
            SET_BOOL_ONCE( opts->with_body, "body" );
            break;
        case 'o':
            SET_BOOL_ONCE( opts->with_flag_IO, "output" );
            break;
        case 'e':
            SET_BOOL_ONCE( opts->with_extended, "extended" );
            break;
        case 'p':
            SET_BOOL_ONCE( opts->with_protection, "protection" );
            break;
        default:
            usage_error( "find: illegal option" );
        }
    }

    CHECK_EXTRA_ARGS();

    int count = 0;
    if ( opts->arg_tag )
        count++;
    if ( opts->arg_hash )
        count++;
    if ( opts->arg_date )
        count++;
    if ( opts->arg_keywords )
        count++;

    if ( count > 1 )
        usage_error( "view note: --tag, --hash, --keywords and --date are incompatible" );

    if ( opts->with_file_flag &&
         ( opts->with_extended || opts->with_body || opts->with_protection ) )
        usage_error( "find: --file incompatible with --extended, --body, "
                     "--protection" );

    if ( !opts->with_file_flag && opts->with_flag_IO )
        usage_error( "find: --output only valid with --file" );

    opts->cmd = CMD_FIND;
}

// --------------------------------------
/***** Implementation of modify  *****/

static void sub_modify( int argc, char *argv[], Options *opts ) {
    if ( argc < 2 )
        usage_error( "Usage: modify <hash> [--tag <text> | --comment <text> | "
                     "--file <text> | --keywords <text> | --body | --input | --protection]" );

    const char *x = "t:c:f:k:bip";
    struct option long_opts[] = {
        { "tag", required_argument, 0, 't' },  { "comment", required_argument, 0, 'c' },
        { "file", required_argument, 0, 'f' }, { "keywords", required_argument, 0, 'k' },
        { "body", no_argument, 0, 'b' },       { "input", no_argument, 0, 'i' },
        { "protection", no_argument, 0, 'p' }, { 0, 0, 0, 0 } };

    int opt;
    opterr = 0;
    optind = 0;
    while ( ( opt = getopt_long( argc, argv, x, long_opts, NULL ) ) != -1 ) {
        switch ( opt ) {
        case 't':
            SET_STRING_ONCE( opts->arg_tag, optarg, "tag" );
            break;
        case 'c':
            SET_STRING_ONCE( opts->arg_comment, optarg, "comment" );
            break;
        case 'f':
            SET_STRING_ONCE( opts->arg_filepath, optarg, "file" );
            break;
        case 'k':
            SET_STRING_ONCE( opts->arg_keywords, optarg, "keywords" );
            break;
        case 'b':
            SET_BOOL_ONCE( opts->with_body, "body" );
            break;
        case 'i':
            SET_BOOL_ONCE( opts->with_flag_IO, "input" );
            break;
        case 'p':
            SET_BOOL_ONCE( opts->with_protection, "protection" );
            break;
        default:
            usage_error( "modify: illegal option" );
        }
    }

    CHECK_EXTRA_ARGS();

    if ( !opts->with_body && opts->with_flag_IO )
        usage_error( "modify: --input only valid with --body" );

    opts->cmd = CMD_MODIFY;
}

static void cmd_modify( int argc, char *argv[], Options *opts ) {
    opts->arg_hash = argv[1];
    sub_modify( argc - 1, argv + 1, opts );
}

// --------------------------------------
/***** Implementation of (Organize, remove, setting, editor, backup, help) *****/

static void cmd_organize( int argc, char *argv[], Options *opts ) {
    if ( argc != 3 )
        usage_error( "Usage: organize <hash> [<hash> | \"up\" | \"down\" | \".\" ]" );

    opts->arg_hash = argv[1];
    opts->arg_generic = argv[2];
    opts->with_protection = true;
    opts->cmd = CMD_ORGANIZE;
}

static void cmd_remove( int argc, char *argv[], Options *opts ) {
    if ( argc != 2 )
        usage_error( "Usage: remove <hash>" );

    opts->arg_hash = argv[1];
    opts->with_protection = true;
    opts->cmd = CMD_REMOVE;
}

static void cmd_setting( int argc, char *argv[], Options *opts ) {
    if ( argc != 2 )
        usage_error( "Usage: setting <index>" );

    char *end;
    long result = strtol( argv[1], &end, 10 );

    if ( *end != '\0' || result < 0 )
        usage_error( "setting: invalid numeric index" );

    opts->arg_index = (int)result;
    opts->cmd = CMD_SETTING;
}

static void cmd_editor( int argc, char *argv[], Options *opts ) {
    if ( argc != 2 )
        usage_error( "Usage: editor <vim|nano|nul>" );

    if ( strcmp( argv[1], "vim" ) && strcmp( argv[1], "nano" ) && strcmp( argv[1], "nul" ) )
        usage_error( "editor: valid options are vim, nano, or nul" );

    opts->arg_editor = argv[1];
    opts->cmd = CMD_EDITOR;
}

static void cmd_backup( int argc, char *argv[], Options *opts ) {
    if ( argc != 1 )
        usage_error( "Usage: backup" );

    opts->cmd = CMD_BACKUP;
}

static void cmd_help( int argc, char *argv[], Options *opts ) {
    if ( argc != 1 )
        usage_error( "Usage: help" );

    opts->cmd = CMD_HELP;
}

// --------------------------------------
/*****  Extracts and validates arguments, populates `opts` *****/

int parse_arguments( int argc, char *argv[], Options *opts ) {

    if ( argc < 2 )
        print_usage( argv[0], "No command specified" );

    memset( opts, 0, sizeof( *opts ) );
    opts->cmd = CMD_NONE;
    opts->arg_index = -1;

    const char *cmd = argv[1];
    for ( int i = 0; commands[i].name; i++ ) {
        if ( !strcmp( cmd, commands[i].name ) ) {
            commands[i].func( argc - 1, argv + 1, opts );
            return 0;
        }
    }

    print_usage( argv[0], "Unknown command" );
    return 1;
}
