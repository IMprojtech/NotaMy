
/*
 * #################################################
 *
 *      Description:
 * Coordinates the modules and chooses the action based on the topic (core of the program)
 *
 *      License:
 * This program is distributed under the terms of the GNU General Public License (GPL),
 * ensuring the freedom to redistribute and modify the software in accordance with open-source standards.
 *
 *  	Version:  1.0
 *		Created:  27/07/2025
 *
 *  	Author:
 * Catoni Mirko (IMprojtech)
 *
 * #################################################
 */

void controller( char *SetFile, char *Passwd, char *Key, AppGlobal *app ) {

    if ( app->opts.with_protection ) {
        char TmpHashPass[HASH_PASS_MAX];

        write_key( Passwd, sizeof( Passwd ) );
        generate_SHA512( Passwd, TmpHashPass );

        if ( strcmp( TmpHashPass, app->cfg.hash_pass ) != 0 ) {
            fprintf( stderr, "[ERROR] invalid password \n" );
            exit( EXIT_FAILURE );
        }
    }

    init_ndat_from_opts( &app->NDat, &app->opts );
    find_function find;

    switch ( app->opts.cmd ) {

    case CMD_ADD_NOTE: { //! Add new note
        NotesData tmpNDat = { 0 };

        if ( app->opts.with_body == true )
            edit( app );

        char parent_tag[24] = { "\0" };

        if ( strlen( app->NDat.Tag ) == 0 )
            strcpy( app->NDat.Tag, DEFAULT_TAG );
        else
            analyze_string_tag( parent_tag, app );

        take_time( &app->NDat );

        generate_sha1( &app->NDat, app );

        strcpy( app->data.tag, app->NDat.Tag );
        strcpy( app->data.date, app->NDat.Date );

        if ( app->opts.with_protection == true ) {
            init_ctx_from_ndat( &app->ctx, &app->NDat );
            protect_encrypt( Passwd, &app->ctx, Key );
        }

        copy_ndat( &tmpNDat, &app->NDat );

        if ( strlen( parent_tag ) == 0 )
            app->root = insert_node( app->root, &app->data );

        else {
            int cont = 0;
            find = find_tag_node;

            check_duplicate( app->root, parent_tag, find, &cont );
            if ( cont > 1 ) {
                int cont = 0;

                printf( "[WARNING] ambiguous tag\ndo you want to specify a hash? [Y/n]\n" );
                char ch = getchar();

                if ( ch != 'y' && ch != 'Y' && ch != '\n' ) {
                    exit( 0 );
                }
                print_find_node( app->root, parent_tag, find, app );
                char hash[41] = { '\0' };
                printf( "Enter hash ->" );
                scanf( "%40s", hash );
                find = find_hash_node;
                check_duplicate( app->root, hash, find, &cont );

                if ( cont > 1 ) {
                    fprintf( stderr, "[ERROR] duplicate hash \n" );
                    exit( EXIT_FAILURE );
                }

                else {
                    TreeNode *parentNode = find_node( app->root, hash, find );
                    if ( parentNode == NULL ) {
                        fprintf( stderr, "[ERROR] %s not found \n", hash );
                        exit( EXIT_FAILURE );
                    }

                    else {
                        insert_node( parentNode, &app->data );
                    }
                }
            } else {
                TreeNode *parentNode = find_node( app->root, parent_tag, find );
                if ( parentNode == NULL ) {
                    fprintf( stderr, "[ERROR] %s not found \n", parent_tag );
                    exit( EXIT_FAILURE );
                }

                else {
                    insert_node( parentNode, &app->data );
                }
            }
        }

        save_note( &tmpNDat, app );
        save_to_file( app->root, app->cfg.file_note );
        break;
    }

    case CMD_ADD_FILE: { //! Add new note file
        char file[sizeof( app->opts.arg_filepath ) + sizeof( app->opts.arg_comment ) + 3];
        strcpy( file, app->opts.arg_filepath );
        if ( app->opts.arg_comment ) {
            strcat( file, " # " );
            strcat( file, app->opts.arg_comment );
        }
        update_config_value( "AddFile", 0, file, SetFile, &app->cfg, &app->stl );
        break;
    }

    case CMD_VIEW_NOTE: { //! View all note
        print_all( app->root, 0, app, Passwd, Key );
        break;
    }

    case CMD_VIEW_TAG: { //! View all tag note
        print_tree( app->root, 0, app );
        break;
    }

    case CMD_VIEW_FILE: { //! View note file list
        update_config_value( "ShowFile", 0, NULL, SetFile, &app->cfg, &app->stl );
        break;
    }

    case CMD_FIND: { //! Find node
        if ( strlen( app->NDat.Tag ) != 0 ) {
            int size = strlen( app->NDat.Tag ) - 1;
            find = find_tag_node;
            if ( app->opts.with_file_flag == true ) {
                int cont = 0;
                check_duplicate( app->root, app->NDat.Tag, find, &cont );

                if ( cont > 1 ) {
                    cont = 0;
                    printf( "[WARNING] ambiguous tag\ndo you want to specify a hash? [Y/n]\n" );
                    char ch = getchar();

                    if ( ch != 'y' && ch != 'Y' && ch != '\n' ) {
                        exit( 0 );
                    }
                    print_find_node( app->root, app->NDat.Tag, find, app );

                    char hash[41] = { '\0' };
                    printf( "Enter hash ->" );
                    scanf( "%40s", hash );
                    find = find_hash_node;
                    check_duplicate( app->root, hash, find, &cont );

                    if ( cont > 1 ) {
                        fprintf( stderr, "[ERROR] duplicate hash \n" );
                        exit( EXIT_FAILURE );
                    }

                    else
                        print_find( app->root, hash, find, app, Passwd, Key );
                } else
                    print_find( app->root, app->NDat.Tag, find, app, Passwd, Key );
            }

            else {
                if ( app->NDat.Tag[size] == '+' ) {
                    app->NDat.Tag[size] = '\0';
                    TreeNode *parentNode = find_node( app->root, app->NDat.Tag, find );
                    print_children( parentNode, app, Passwd, Key );

                } else if ( app->NDat.Tag[size] == '-' ) {
                    app->NDat.Tag[size] = '\0';
                    TreeNode *parentNode = find_node( app->root, app->NDat.Tag, find );

                    print_sibling( parentNode, app, Passwd, Key );
                } else {
                    print_find( app->root, app->NDat.Tag, find, app, Passwd, Key );
                }
            }
        } else if ( app->opts.arg_hash ) {
            int size = strlen( app->opts.arg_hash ) - 1;
            find = find_hash_node;
            if ( app->opts.arg_hash[size] == '+' ) {
                app->opts.arg_hash[size] = '\0';
                TreeNode *parentNode = find_node( app->root, app->opts.arg_hash, find );
                print_children( parentNode, app, Passwd, Key );

            } else if ( app->opts.arg_hash[size] == '-' ) {
                app->opts.arg_hash[size] = '\0';
                TreeNode *parentNode = find_node( app->root, app->opts.arg_hash, find );
                print_sibling( parentNode, app, Passwd, Key );
            } else {
                print_find( app->root, app->opts.arg_hash, find, app, Passwd, Key );
            }
        } else if ( app->opts.arg_date )
            process_data( app->root, app );
        else if ( app->opts.arg_keywords )
            print_find_keywords( app->root, app->opts.arg_keywords, app );
        break;
    }

    case CMD_MODIFY: { //! Edit note
        NotesData tmpNDat = { 0 };

        if ( strlen( app->NDat.Tag ) != 0 )
            strcpy( tmpNDat.Tag, app->NDat.Tag );

        if ( strlen( app->NDat.Comment ) != 0 )
            strcpy( tmpNDat.Comment, app->NDat.Comment );

        if ( strlen( app->NDat.Link_File ) != 0 )
            strcpy( tmpNDat.Link_File, app->NDat.Link_File );

        if ( strlen( app->NDat.Keywords ) != 0 )
            strcpy( tmpNDat.Keywords, app->NDat.Keywords );

        int cont = 0;
        find = find_hash_node;
        check_duplicate( app->root, app->opts.arg_hash, find, &cont );
        if ( cont > 1 ) {
            fprintf( stderr, "[ERROR] duplicate hash \n" );
            exit( EXIT_FAILURE );
        }

        find = find_hash_node;
        TreeNode *node = find_node( app->root, app->opts.arg_hash, find );

        if ( node == NULL ) {
            fprintf( stderr, "[ERROR] not found \n" );
            exit( EXIT_FAILURE );
        }

        read_dat( node->data.start, node->data.end, app );
        if ( app->NDat.Protection ) {
            init_ctx_from_ndat( &app->ctx, &app->NDat );
            if ( app->opts.with_protection )
                protect_decrypt( Passwd, &app->ctx, Key );
            else {
                fprintf( stderr, "[ERROR] protected \n" );
                exit( EXIT_FAILURE );
            }
        }

        node->data.start = 0;
        node->data.end = 0;

        if ( strlen( tmpNDat.Comment ) != 0 )
            strcpy( app->NDat.Comment, tmpNDat.Comment );

        if ( strlen( tmpNDat.Link_File ) != 0 )
            strcpy( app->NDat.Link_File, tmpNDat.Link_File );

        if ( strlen( tmpNDat.Keywords ) != 0 )
            strcpy( app->NDat.Keywords, tmpNDat.Keywords );

        if ( strlen( tmpNDat.Tag ) != 0 ) {
            strcpy( app->NDat.Tag, tmpNDat.Tag );
            strcpy( node->data.tag, tmpNDat.Tag );
        }

        if ( app->opts.with_body == true )
            edit( app );

        if ( app->opts.with_protection == true ) {
            init_ctx_from_ndat( &app->ctx, &app->NDat );
            protect_encrypt( Passwd, &app->ctx, Key );
        }

        copy_ndat( &tmpNDat, &app->NDat );

        save_note( &tmpNDat, app );
        save_to_file( app->root, app->cfg.file_note );
        break;
    }

    case CMD_ORGANIZE: { //! Organize nodes

        if ( app->opts.arg_hash[0] == '.' ) {
            fprintf( stderr, "[ERROR] the root cannot be moved \n" );
            exit( EXIT_FAILURE );
        }

        int contD = 0, contS = 0;
        find = find_hash_node;

        check_duplicate( app->root, app->opts.arg_generic, find, &contD );
        if ( contD > 1 ) {
            fprintf( stderr, "[ERROR] duplicate destination \n" );
            exit( EXIT_FAILURE );
        }

        check_duplicate( app->root, app->opts.arg_hash, find, &contS );
        if ( contS > 1 ) {
            fprintf( stderr, "[ERROR] duplicate source \n" );
            exit( EXIT_FAILURE );
        }

        find = find_hash_node;
        app->root = move_node( app->root, app->opts.arg_generic, app->opts.arg_hash, find );

        save_note( NULL, app );
        save_to_file( app->root, app->cfg.file_note );
        break;
    }

    case CMD_REMOVE: { //! Remove note

        if ( strlen( app->opts.arg_hash ) != 0 ) {
            int cont = 0;
            find = find_hash_node;
            check_duplicate( app->root, app->opts.arg_hash, find, &cont );
            if ( cont > 1 ) {
                fprintf( stderr, "[ERROR] duplicate hash \n" );
                exit( EXIT_FAILURE );
            } else {
                app->root = remove_node( app->root, app->opts.arg_hash );
            }
        } else if ( strlen( app->NDat.Tag ) != 0 ) {
            int cont = 0;
            find = find_tag_node;
            check_duplicate( app->root, app->NDat.Tag, find, &cont );

            if ( cont > 1 ) {
                cont = 0;
                printf( "[WARNING] ambiguous tag\ndo you want to specify a hash? [Y/n]\n" );
                char ch = getchar();

                if ( ch != 'y' && ch != 'Y' && ch != '\n' ) {
                    exit( 0 );
                }
                print_find_node( app->root, app->NDat.Tag, find, app );

                char hash[41] = { '\0' };
                printf( "Enter hash ->" );
                scanf( "%40s", hash );
                find = find_hash_node;
                check_duplicate( app->root, hash, find, &cont );

                if ( cont > 1 ) {
                    fprintf( stderr, "[ERROR] duplicate hash \n" );
                    exit( EXIT_FAILURE );
                }

                else {
                    app->root = remove_node( app->root, hash );
                }
            } else {
                TreeNode *nodeToRemove = find_node( app->root, app->NDat.Tag, find );
                if ( nodeToRemove != NULL )
                    app->root = remove_node( app->root, nodeToRemove->data.hash );
            }
        } else {
            fprintf( stderr, "[ERROR] syntax error\n" );
            exit( EXIT_FAILURE );
        }

        save_note( NULL, app );
        save_to_file( app->root, app->cfg.file_note );
        break;
    }

    case CMD_SETTING: { //! Change note file
        remove( app->cfg.file_note );
        update_config_value( "SetFile", app->opts.arg_index, NULL, SetFile, &app->cfg, &app->stl );
        break;
    }

    case CMD_EDITOR: { //! Change editor
        update_config_value( "Editor=", 0, app->opts.arg_editor, SetFile, &app->cfg, &app->stl );
        break;
    }

    case CMD_BACKUP: { //! Create backup
        char backup[201];
        strcpy( backup, app->cfg.file_note );
        strcat( backup, "_Backup" );
        copy_file( app->cfg.file_note, backup );
        break;
    }

    case CMD_HELP: { //! Helper
        help();
        break;
    }

    default:
        fprintf( stderr, "[ERROR] ?\n" );
        exit( EXIT_FAILURE );
        break;
    }
}
