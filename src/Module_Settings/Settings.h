
/*
 * #################################################
 *
 *		Description:
 * Header associated with Settings.c.
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#define CONFIG_LINE_MAX 256
#define VALUE_MAX 128
#define HASH_PASS_MAX 129
#define NAME_MAX 64

#define MAX_LINES 1024
#define MAX_LINE_LEN 512

/* ANSI escape sequences */
#define CSI "\x1B["

// Holds processed ANSI codes for output
// Holds processed ANSI codes for output
// Holds processed ANSI codes for output
typedef struct {
    char color_tag[VALUE_MAX];
    char color_hash[VALUE_MAX];
    char color_comment[VALUE_MAX];
    char color_keywords[VALUE_MAX];
    char color_body[VALUE_MAX];
    char color_date[VALUE_MAX];
    char color_file[VALUE_MAX];
    char color_generic[VALUE_MAX];
} Style;

// Holds raw configuration values
typedef struct {
    char hash_pass[HASH_PASS_MAX];
    char editor[NAME_MAX];
    char file_note[VALUE_MAX];
} Config;

// Runtime commands: AddFile, ShowFile, SetFile, Editor
void update_config_value( const char *command, int index, const char *input,
                          const char *config_path, Config *cfg, Style *stl );

// Load config and style specs
void load_config( const char *path, Config *cfg, Style *stl );

// Write config and style specs
void write_config( const char *path, Config *cfg, Style *stl );

// Load structures with default settings
void default_config( Config *cfg, Style *stl );

// Load structures with nocolor settings
void default_config_nocolor( Style *stl );

#endif // SETTINGS_H
