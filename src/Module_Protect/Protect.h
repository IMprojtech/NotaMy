
/*
 * #################################################
 *
 *              Description:
 * Header associated with Protect.c.
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

#ifndef PROTECT_H
#define PROTECT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <termios.h>
#include <time.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

#define KEY_SIZE 129
#define IV_SIZE 15

/*** Context holding all encryption-related data ***/
typedef struct {
    char *iv;
    char *body;
    char *comment;
    char *file;
    bool *protection;
} Protect;

// Reads a password from stdin without echoing input
void write_key( char *password, size_t max_size );

// Generates a SHA-512 hash of input
void generate_SHA512( const char *input, char *output );

// Encrypts fields in the context
void protect_encrypt( const char *passwd, Protect *ctx, char *key );

// Decrypts fields in the context
void protect_decrypt( const char *passwd, Protect *ctx, char *key );

// Masks sensitive fields in the context with asterisks
void mask( Protect *ctx );

#endif // PROTECT_H
