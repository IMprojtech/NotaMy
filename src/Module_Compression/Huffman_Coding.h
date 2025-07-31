
/*
 * #################################################
 *
 *              Description:
 * Header associated with Huffman_Coding.c.
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

#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct HuffNode {
    uint8_t symbol;
    uint32_t freq;
    struct HuffNode *left, *right;
} HuffNode;

bool huffman_compress_file( const char *in_path, const char *out_path );
bool huffman_decompress_file( const char *in_path, const char *out_path );

// For internal use
HuffNode *huffman_build_tree( uint32_t freq[256] );
void huffman_free_tree( HuffNode *node );

#endif // HUFFMAN_H
