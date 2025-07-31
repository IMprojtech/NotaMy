
/*
 * #################################################
 *
 *      Description:
 * NotaMy alias "ntm" is a command-line Linux application
 * designed to streamline the management and organization of notes and crucial information.
 * It provides an intuitive interface for input, categorization, and display of notes,
 * allowing users to create a hierarchical structure using personalized tags.
 *
 *      License:
 * This program is distributed under the terms of the GNU General Public License (GPL),
 * ensuring the freedom to redistribute and modify the software in accordance with open-source standards.
 *
 *  	Version:  1.0
 *		Created:  30/07/2025
 *
 *  	Author:
 * Catoni Mirko (IMprojtech)
 *
 * #################################################
 */

//! Create your personal organizational structure with "NotaMy" and simplify the management of your information!

#include <stdio.h>

#include "ntm.h"

int main( int argc, char *argv[] ) {

    AppGlobal app = { 0 };

    char Key[129] = { '\0' };
    char Passwd[129] = { '\0' };

    char SetFile[300] = { '\0' };

    //! Initialize settings from setup file

    init_setting( SetFile, Passwd, &app );

    //! Process the arguments

    char *progname = basename( argv[0] );
    int no_color = strcmp( progname, ALIAS_NOCOLOR ) == 0;
    if ( no_color )
        default_config_nocolor( &app.stl );

    if ( parse_arguments( argc, argv, &app.opts ) != 0 )
        return 1;

    //! Unzip files

    char original_file[300];

    strcpy( original_file, app.cfg.file_note );
    strcat( app.cfg.file_note, "..." );

    if ( !huffman_decompress_file( original_file, app.cfg.file_note ) )
        copy_file( original_file, app.cfg.file_note );

    //! Calculate sha1 of the file at the beginning

    char hash_start[41];
    sha1_file( app.cfg.file_note, hash_start );

    //! Initialize "data" and "root"

    init_blockinfo( &app.data );
    app.root = load_from_file( app.root, &app.data, app.cfg.file_note );

    controller( SetFile, Passwd, Key, &app );

    free_tree( app.root );

    //! Calculate sha1 of the file at the end

    char hash_end[41];
    sha1_file( app.cfg.file_note, hash_end );

    //! If the file has changed, compress it.

    if ( strcmp( hash_start, hash_end ) )
        huffman_compress_file( app.cfg.file_note, original_file );

    remove( app.cfg.file_note );

    return 0;
}
