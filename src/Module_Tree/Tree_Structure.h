
/*
 * #################################################
 *
 *      Description:
 * Header associated with Tree_Structure.c.
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

#ifndef TREE_STRUCTURE_H
#define TREE_STRUCTURE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRNODE "*NODE*"
#define STRNODENULL "*NULL*"
#define DELIMITER "*====*"
#define INITIAL_TAG "/"

typedef struct {
    long start;
    long end;
    char hash[42];
    char tag[24];
    char date[20];
} BlockInfo;

typedef struct TreeNode {
    BlockInfo data;
    struct TreeNode *firstChild;
    struct TreeNode *nextSibling;
} TreeNode;

typedef int ( *find_function )( TreeNode *, char * );

// Free memory
void free_tree( TreeNode *root );

// Auxiliary function to check for duplicate tags in the tree
void check_duplicate( TreeNode *root, char *key, find_function find, int *cont );

// Auxiliary function to check if a node is a descendant of another node
int is_descendant( TreeNode *potentialDescendant, TreeNode *ancestor );

// Search tags
int find_tag_node( TreeNode *node, char *tag );

// Search hash
int find_hash_node( TreeNode *node, char *hash );

// Search date
int find_date_node( TreeNode *node, char *date );

// Search node
TreeNode *find_node( TreeNode *root, char *key, find_function find );

// Search previous sibling
TreeNode *find_previous_sibling( TreeNode *root, TreeNode *node );

// Insert new node
TreeNode *insert_node( TreeNode *currentNode, const BlockInfo *data );

// Remove node and its descendants
TreeNode *remove_node( TreeNode *root, char *hash );

// Copy sub tree
TreeNode *copy_subtree( TreeNode *subtree );

// Move node
TreeNode *move_node( TreeNode *root, char *keyDestination, char *keySource, find_function find );

// Write structure at the end of the file
void save_to_file( TreeNode *root, const char *file_note );

//  Load data from file
TreeNode *load_from_file( TreeNode *root, BlockInfo *data, const char *file_note );

#endif // TREE_STRUCTURE_H
