
/*
 * #################################################
 *
 *      Description:
 * This module handles compression and decompression of note files.
 *
 *      License:
 * This program is distributed under the terms of the GNU General Public License (GPL),
 * ensuring the freedom to redistribute and modify the software in accordance with open-source standards.
 *
 *      Version:  1.0
 *      Created:  18/07/2025
 *
 *      Author:
 * Catoni Mirko (IMprojtech)
 *
 * #################################################
 */

#include "Huffman_Coding.h"

// --------------------------------------
/* Support structures */
typedef struct {
    uint64_t bits;
    int len;
} HuffCode;

typedef struct {
    HuffNode **data;
    int size;
    int capacity;
} MinHeap;

// --------------------------------------
/* Handler declarations */
static MinHeap *heap_create( int capacity );
static void heap_push( MinHeap *heap, HuffNode *node );
static HuffNode *heap_pop( MinHeap *heap );
static void heap_destroy( MinHeap *heap );
static void build_code_table( HuffNode *node, HuffCode table[256], uint64_t bits, int len );

static MinHeap *heap_create( int capacity ) {
    MinHeap *heap = malloc( sizeof( MinHeap ) );
    heap->data = malloc( sizeof( HuffNode * ) * capacity );
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

static void heap_push( MinHeap *heap, HuffNode *node ) {
    int i = heap->size++;
    while ( i > 0 ) {
        int p = ( i - 1 ) / 2;
        if ( heap->data[p]->freq <= node->freq )
            break;
        heap->data[i] = heap->data[p];
        i = p;
    }
    heap->data[i] = node;
}

static HuffNode *heap_pop( MinHeap *heap ) {
    if ( heap->size == 0 )
        return NULL;
    HuffNode *min = heap->data[0];
    HuffNode *last = heap->data[--heap->size];
    int i = 0;
    while ( i * 2 + 1 < heap->size ) {
        int a = i * 2 + 1, b = i * 2 + 2;
        int min_i = ( b < heap->size && heap->data[b]->freq < heap->data[a]->freq ) ? b : a;
        if ( heap->data[min_i]->freq >= last->freq )
            break;
        heap->data[i] = heap->data[min_i];
        i = min_i;
    }
    heap->data[i] = last;
    return min;
}

static void heap_destroy( MinHeap *heap ) {
    free( heap->data );
    free( heap );
}

// --------------------------------------
/***** Tree construction *****/
HuffNode *huffman_build_tree( uint32_t freq[256] ) {
    MinHeap *heap = heap_create( 256 );
    for ( int i = 0; i < 256; ++i ) {
        if ( freq[i] > 0 ) {
            HuffNode *node = malloc( sizeof( HuffNode ) );
            node->symbol = (uint8_t)i;
            node->freq = freq[i];
            node->left = node->right = NULL;
            heap_push( heap, node );
        }
    }

    while ( heap->size > 1 ) {
        HuffNode *a = heap_pop( heap );
        HuffNode *b = heap_pop( heap );
        HuffNode *parent = malloc( sizeof( HuffNode ) );
        parent->symbol = 0;
        parent->freq = a->freq + b->freq;
        parent->left = a;
        parent->right = b;
        heap_push( heap, parent );
    }

    HuffNode *root = heap_pop( heap );
    heap_destroy( heap );
    return root;
}

void huffman_free_tree( HuffNode *node ) {
    if ( !node )
        return;
    huffman_free_tree( node->left );
    huffman_free_tree( node->right );
    free( node );
}

// --------------------------------------
/***** Code generation *****/
static void build_code_table( HuffNode *node, HuffCode table[256], uint64_t bits, int len ) {
    if ( !node->left && !node->right ) {
        table[node->symbol].bits = bits;
        table[node->symbol].len = len;
        return;
    }
    if ( node->left )
        build_code_table( node->left, table, ( bits << 1 ), len + 1 );
    if ( node->right )
        build_code_table( node->right, table, ( bits << 1 ) | 1, len + 1 );
}

// --------------------------------------
/***** Compression *****/
bool huffman_compress_file( const char *in_path, const char *out_path ) {
    FILE *in = fopen( in_path, "rb" );
    if ( !in ) {
        fprintf( stderr, "File \"%s\" opening failed \n", in_path );
        exit( EXIT_FAILURE );
    }

    uint32_t freq[256] = { 0 };
    int c;
    while ( ( c = fgetc( in ) ) != EOF )
        freq[(uint8_t)c]++;
    rewind( in );

    HuffNode *root = huffman_build_tree( freq );
    if ( !root ) {
        fclose( in );
        fprintf( stderr, "Error in Compression tree \n" );
        exit( EXIT_FAILURE );
    }

    HuffCode table[256] = { 0 };
    build_code_table( root, table, 0, 0 );

    FILE *out = fopen( out_path, "wb" );
    if ( !out ) {
        fclose( in );
        huffman_free_tree( root );
        fprintf( stderr, "File \"%s\" creation failed \n", out_path );
        exit( EXIT_FAILURE );
    }

    fwrite( "HUFF", 1, 4, out );
    fwrite( freq, sizeof( uint32_t ), 256, out );

    uint64_t buffer = 0;
    int bits = 0;
    while ( ( c = fgetc( in ) ) != EOF ) {
        HuffCode code = table[(uint8_t)c];
        buffer = ( buffer << code.len ) | code.bits;
        bits += code.len;

        while ( bits >= 8 ) {
            uint8_t b = ( buffer >> ( bits - 8 ) ) & 0xFF;
            fputc( b, out );
            bits -= 8;
        }
    }

    if ( bits > 0 ) {
        uint8_t b = ( buffer << ( 8 - bits ) ) & 0xFF;
        fputc( b, out );
    }

    fclose( in );
    fclose( out );
    huffman_free_tree( root );
    return true;
}

// --------------------------------------
/***** Decompression *****/
bool huffman_decompress_file( const char *in_path, const char *out_path ) {
    FILE *in = fopen( in_path, "rb" );
    if ( !in )
        return false;

    FILE *out = fopen( out_path, "wb" );
    if ( !out ) {
        fclose( in );
        fprintf( stderr, "File \"%s\" creation failed \n", out_path );
        exit( EXIT_FAILURE );
    }

    char magic[5] = { 0 };
    fread( magic, 1, 4, in );
    if ( strcmp( magic, "HUFF" ) != 0 ) {
        fclose( in );
        fclose( out );
        return false;
    }

    uint32_t freq[256] = { 0 };
    uint64_t total = 0;
    for ( int i = 0; i < 256; ++i ) {
        fread( &freq[i], sizeof( uint32_t ), 1, in );
        total += freq[i];
    }

    if ( total == 0 ) {
        fclose( in );
        fclose( out );
        return false;
    }

    HuffNode *root = huffman_build_tree( freq );
    if ( !root ) {
        fclose( in );
        fclose( out );
        fprintf( stderr, "Error in decompression tree \n" );
        exit( EXIT_FAILURE );
    }

    HuffNode *node = root;
    int c, bit;
    uint8_t byte = 0;
    int bits_left = 0;
    uint64_t written = 0;

    while ( written < total && ( c = fgetc( in ) ) != EOF ) {
        byte = (uint8_t)c;
        bits_left = 8;

        while ( bits_left-- && written < total ) {
            bit = ( byte >> bits_left ) & 1;
            node = bit ? node->right : node->left;
            if ( !node->left && !node->right ) {
                fputc( node->symbol, out );
                written++;
                node = root;
            }
        }
    }

    fclose( in );
    fclose( out );
    huffman_free_tree( root );
    return true;
}
