
/*
 * #################################################
 *
 *      Description:
 * This module contains useful functions for manipulating tree nodes,
 * including saving and loading the data structure.
 *
 *      License:
 * This program is distributed under the terms of the GNU General Public License (GPL),
 * ensuring the freedom to redistribute and modify the software in accordance with open-source standards.
 *
 *  	Version:  1.0
 *		Created:  11/07/2025
 *
 *  	Author:
 * Catoni Mirko (IMprojtech)
 *
 * #################################################
 */

#include "Tree_Structure.h"

// --------------------------------------
/* Handler declarations */
static void error_tree( const char *msg );
static void swap_nodes( TreeNode *previous, TreeNode *current, TreeNode *next );
static void write_tree( FILE *fp, TreeNode *root );
static TreeNode *read_tree( FILE *fp );
static int find_delimiter( FILE *fp );

// --------------------------------------
/***** Error reporting function *****/
static void error_tree( const char *msg ) {
    fprintf( stderr, "%s\n", msg );
    exit( EXIT_FAILURE );
}

// --------------------------------------
/***** Free memory *****/
void free_tree( TreeNode *root ) {

    if ( root == NULL ) {
        return;
    }

    free_tree( root->firstChild );
    free_tree( root->nextSibling );
    free( root );
    root = NULL;
}

//----- Node Checks -----

// --------------------------------------
/***** Auxiliary function to check for duplicate tags in the tree *****/
void check_duplicate( TreeNode *root, char *key, find_function find, int *cont ) {
    if ( root == NULL ) {
        return;
    }

    if ( find( root, key ) == 0 ) {
        ( *cont )++;
    }

    check_duplicate( root->firstChild, key, find, cont );
    check_duplicate( root->nextSibling, key, find, cont );
}

// --------------------------------------
/***** Auxiliary function to check if a node is a descendant of another node *****/
int is_descendant( TreeNode *potentialDescendant, TreeNode *ancestor ) {
    if ( ancestor == NULL )
        return 0;

    if ( ancestor == potentialDescendant )
        return 1;

    TreeNode *child = ancestor->firstChild;
    while ( child != NULL ) {
        if ( is_descendant( potentialDescendant, child ) )
            return 1;
        child = child->nextSibling;
    }

    return 0;
}

//----- Research -----

// --------------------------------------
/***** Search tags *****/
int find_tag_node( TreeNode *node, char *tag ) {
    return strncasecmp( node->data.tag, tag, strlen( tag ) );
}

// --------------------------------------
/***** Search hash *****/
int find_hash_node( TreeNode *node, char *hash ) {
    return strncasecmp( node->data.hash, hash, strlen( hash ) );
}

// --------------------------------------
/***** Search date *****/
int find_date_node( TreeNode *node, char *date ) {
    return strncasecmp( node->data.date, date, strlen( date ) );
}

// --------------------------------------
/***** Search node *****/
TreeNode *find_node( TreeNode *root, char *key, find_function find ) {
    if ( root == NULL ) {
        return NULL;
    }

    if ( find( root, key ) == 0 ) {
        return root;
    }

    TreeNode *result = find_node( root->firstChild, key, find );
    if ( result == NULL ) {
        result = find_node( root->nextSibling, key, find );
    }

    return result;
}

// --------------------------------------
/***** Search previous sibling *****/
TreeNode *find_previous_sibling( TreeNode *root, TreeNode *node ) {

    if ( root == NULL ) {
        return NULL;
    }

    if ( root->nextSibling == node || root->firstChild == node ) {
        return root;
    }

    TreeNode *result = find_previous_sibling( root->firstChild, node );
    if ( result == NULL ) {
        result = find_previous_sibling( root->nextSibling, node );
    }

    return result;
}

//----- Tree management -----

// --------------------------------------
/***** Insert new node *****/
TreeNode *insert_node( TreeNode *currentNode, const BlockInfo *data ) {

    TreeNode *newNode = (TreeNode *)malloc( sizeof( TreeNode ) );

    newNode->data = *data;
    newNode->firstChild = NULL;
    newNode->nextSibling = NULL;

    if ( currentNode == NULL ) {
        return newNode;
    }

    if ( currentNode->firstChild == NULL ) {
        currentNode->firstChild = newNode;
    }

    else {
        TreeNode *sibling = currentNode->firstChild;
        while ( sibling->nextSibling != NULL ) {
            sibling = sibling->nextSibling;
        }
        sibling->nextSibling = newNode;
    }
    return currentNode;
}

// --------------------------------------
/***** Remove node and its descendants *****/
TreeNode *remove_node( TreeNode *root, char *hash ) {

    if ( root == NULL ) {
        return NULL;
    }

    if ( strncasecmp( root->data.hash, hash, strlen( hash ) ) == 0 ) {
        TreeNode *newRoot = root->nextSibling;
        free( root );
        return newRoot;
    }

    TreeNode *currentSibling = root;
    TreeNode *previousSibling = NULL;

    while ( currentSibling != NULL &&
            strncasecmp( currentSibling->data.hash, hash, strlen( hash ) ) != 0 ) {
        previousSibling = currentSibling;
        currentSibling = currentSibling->nextSibling;
    }

    if ( currentSibling != NULL ) {
        if ( previousSibling != NULL ) {
            previousSibling->nextSibling = currentSibling->nextSibling;
        }
        free( currentSibling );
    }

    root->firstChild = remove_node( root->firstChild, hash );
    root->nextSibling = remove_node( root->nextSibling, hash );

    return root;
}

// --------------------------------------
/***** Copy sub tree *****/
TreeNode *copy_subtree( TreeNode *subtree ) {
    if ( subtree == NULL ) {
        return NULL;
    }

    TreeNode *copy = malloc( sizeof( TreeNode ) );
    copy->data = subtree->data;
    copy->firstChild = copy_subtree( subtree->firstChild );
    copy->nextSibling = copy_subtree( subtree->nextSibling );

    return copy;
}

// --------------------------------------
/***** Exchange node *****/
static void swap_nodes( TreeNode *previous, TreeNode *current, TreeNode *next ) {
    if ( current == NULL || next == NULL )
        error_tree( "unable to move the node" );

    if ( previous->firstChild == current ) {
        current->nextSibling = next->nextSibling;
        next->nextSibling = current;
        previous->firstChild = next;
    } else {
        current->nextSibling = next->nextSibling;
        next->nextSibling = current;
        previous->nextSibling = next;
    }
}

// --------------------------------------
/***** Move node *****/
TreeNode *move_node( TreeNode *root, char *keyDestination, char *keySource, find_function find ) {

    TreeNode *sourceNode = find_node( root, keySource, find );

    if ( sourceNode == NULL )
        error_tree( "source node no found" );

    if ( strncasecmp( keyDestination, "up", strlen( keyDestination ) ) == 0 ) { // Move Up
        TreeNode *previusNode = find_previous_sibling( root, sourceNode );
        TreeNode *previuspreviusNode = find_previous_sibling( root, previusNode );

        if ( previusNode == NULL || previusNode->firstChild == sourceNode )
            error_tree( "unable to move the node" );

        swap_nodes( previuspreviusNode, previusNode, sourceNode );

    } else if ( strncasecmp( keyDestination, "down",
                             strlen( keyDestination ) ) == 0 ) { // Move Down
        TreeNode *previusNode = find_previous_sibling( root, sourceNode );

        if ( sourceNode->nextSibling == NULL )
            error_tree( "unable to move the node" );

        swap_nodes( previusNode, sourceNode, sourceNode->nextSibling );
    } else {

        TreeNode *nodeDestination = find_node( root, keyDestination, find );

        if ( nodeDestination == NULL )
            error_tree( "destination node no found destination" );

        if ( is_descendant( nodeDestination, sourceNode ) ) {
            error_tree( "unable to move the node" );
        }

        TreeNode *newNode = malloc( sizeof( TreeNode ) );
        newNode->data = sourceNode->data;
        newNode->firstChild = copy_subtree( sourceNode->firstChild );
        newNode->nextSibling = NULL;

        root = remove_node( root, keySource );

        if ( nodeDestination->firstChild == NULL ) {
            nodeDestination->firstChild = newNode;
        } else {
            TreeNode *sibling = nodeDestination->firstChild;
            while ( sibling->nextSibling != NULL ) {
                sibling = sibling->nextSibling;
            }
            sibling->nextSibling = newNode;
        }
    }

    return root;
}

//----- Save and load tree -----

// --------------------------------------
/***** Write structure *****/
static void write_tree( FILE *fp, TreeNode *root ) {
    if ( root == NULL ) {
        fprintf( fp, "%s\n", STRNODENULL );
        return;
    }

    fprintf( fp, "%s\n%ld %ld %s %s %s\n", STRNODE, root->data.start, root->data.end,
             root->data.hash, root->data.tag, root->data.date );
    write_tree( fp, root->firstChild );
    write_tree( fp, root->nextSibling );
}

// --------------------------------------
/***** Read_structure *****/
static TreeNode *read_tree( FILE *fp ) {
    BlockInfo data;
    char opt[8];

    if ( fscanf( fp, "%7s", opt ) != 1 ) {
        return NULL;
    }

    if ( strcmp( opt, STRNODENULL ) == 0 ) {
        return NULL;
    }

    if ( strcmp( opt, STRNODE ) == 0 ) {
        char buff[64];
        if ( fscanf( fp, "%ld %ld %40s %[^\n]", &data.start, &data.end, data.hash, buff ) != 4 ) {
            return NULL;
        }

        if ( strlen( buff ) == 3 ) // root
            sscanf( buff, "%23s %19[^\n]", data.tag, data.date );

        else {
            size_t bl = strlen( buff );
            memcpy( data.date, buff + bl - 19, 19 );
            data.date[19] = '\0';

            size_t taglen = bl - 19 - 1;
            char temp_tag[24];
            memcpy( temp_tag, buff, taglen );
            temp_tag[taglen] = '\0';

            char *start = temp_tag;
            while ( *start == ' ' || *start == '\t' )
                start++;

            char *end = start + strlen( start ) - 1;
            while ( end >= start && ( *end == ' ' || *end == '\t' ) ) {
                *end = '\0';
                end--;
            }

            strncpy( data.tag, start, sizeof( data.tag ) - 1 );
            data.tag[sizeof( data.tag ) - 1] = '\0';
        }
        TreeNode *root = insert_node( NULL, &data );

        if ( root == NULL ) {
            error_tree( "node not inserted" );
        }

        root->firstChild = read_tree( fp );
        root->nextSibling = read_tree( fp );
        return root;
    }

    return NULL;
}

// --------------------------------------
/***** Write structure at the end of the file *****/
void save_to_file( TreeNode *root, const char *file_note ) {
    FILE *fp;
    if ( ( fp = fopen( file_note, "rb+" ) ) == NULL )
        error_tree( "file opening failed" );

    fseek( fp, 0, SEEK_END );
    fprintf( fp, "\n %s \n", DELIMITER );
    write_tree( fp, root );
    fclose( fp );
}

// --------------------------------------
/***** Find the delimiter that divides the structure from the data *****/
static int find_delimiter( FILE *fp ) {
    fseek( fp, 0, SEEK_END );

    long position = ftell( fp );

    while ( position >= 0 ) {
        fseek( fp, position, SEEK_SET );
        char ch = fgetc( fp );

        if ( ch == ' ' || ch == '\n' ) {
            char word[100];
            fscanf( fp, "%s", word );
            if ( strcmp( word, DELIMITER ) == 0 )
                return position;
        }
        position--;
    }
    error_tree( "tree not found" );
    return -1;
}

// --------------------------------------
/***** Load data from file *****/
TreeNode *load_from_file( TreeNode *root, BlockInfo *data, const char *file_note ) {
    ;
    FILE *fp;
    if ( ( fp = fopen( file_note, "rb" ) ) == NULL )
        error_tree( "file opening failed" );

    fseek( fp, 0, SEEK_END );
    int check = ftell( fp );

    if ( check > 0 ) {
        find_delimiter( fp );
        root = read_tree( fp );
        fclose( fp );
    } else {
        fclose( fp );

        strcpy( data->tag, INITIAL_TAG );

        root = insert_node( root, data );
        root->data.end = -1;
    }
    return root;
}
