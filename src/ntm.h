
/*
 * #################################################
 *
 *              Description:
 * Header of the "NoteMy" project
 *
 *      License:
 * This program is distributed under the terms of the GNU General Public License (GPL),
 * ensuring the freedom to redistribute and modify the software in accordance with open-source standards.
 *
 *      Author:
 * Catoni Mirko (IMprojtech)
 *
 * #################################################
 */

#ifndef NTM_H
#define NTM_H

// Modules
#include "Module_Settings/Settings.h"
#include "Module_Arguments/Arguments.h"
#include "Module_Protect/Protect.h"
#include "Module_Tree/Tree_Structure.h"
#include "Module_Date_Search/Date_Search.h"
#include "Module_Compression/Huffman_Coding.h"

#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <libgen.h>

// Program details
#define NAME "NotaMy"
#define ALIAS "ntm"
#define ALIAS_NOCOLOR "ntm_nc"
#define VER "1.0"

// Default file
#define DIR_SET "/.config"
#define SET_FILE "/NotaMy.conf"
#define DEFAULT_FILE_NOTE "/Notes_Map.X"

// Default tag
#define DEFAULT_TAG "?"

// Default delimiter file
#define FIELD_DELIM "<::>"
#define RECORD_DELIM "<::END::>\n"

typedef struct { //! Notes
    char Tag[50];
    char Comment[480];
    char Keywords[300];
    char Link_File[208];
    char Date[20];
    char Iv[16];
    char *Body;
    bool Protection;
} NotesData;

typedef struct { //! AppGlobal
    Config cfg;
    Style stl;
    Options opts;
    Protect ctx;
    BlockInfo data;
    TreeNode *root;
    NotesData NDat;
} AppGlobal;

#include "common_utils.c"
#include "edit_body.c"
#include "io_manager.c"
#include "display_utils.c"
#include "task_manager.c"

#endif // NTM_H
