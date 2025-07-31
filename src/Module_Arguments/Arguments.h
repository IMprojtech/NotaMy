
/*
 * #################################################
 *
 *      Description:
 * Header associated with Arguments.c.
 *
 *      License:
 * This program is distributed under the terms of the GNU General Public License (GPL),
 * ensuring the freedom to redistribute and modify the software in accordance with open-source standards.
 *
 *  	Author:
 * Catoni Mirko (IMprojtech)
 *
 * #################################################
 */

#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>

// Commands main
typedef enum {
    CMD_NONE,
    CMD_ADD_NOTE,
    CMD_ADD_FILE,
    CMD_VIEW_NOTE,
    CMD_VIEW_TAG,
    CMD_VIEW_FILE,
    CMD_FIND,
    CMD_MODIFY,
    CMD_ORGANIZE,
    CMD_REMOVE,
    CMD_SETTING,
    CMD_EDITOR,
    CMD_BACKUP,
    CMD_HELP
} Command;

// Struct for options
typedef struct {
    Command cmd;

    char *arg_tag;
    char *arg_hash;
    char *arg_comment;
    char *arg_filepath;
    char *arg_editor;
    char *arg_date;
    char *arg_keywords;
    char *arg_generic;

    int arg_index;

    bool with_body;
    bool with_protection;
    bool with_extended;
    bool with_keywords;
    bool with_file_flag;
    bool with_flag_IO;

} Options;

// Extracts and validates arguments, populates `opts`.
int parse_arguments( int argc, char *argv[], Options *opts );

// DEBUG: print contents
void print_opts( Options *opts );

#endif // ARGUMENTS_H
