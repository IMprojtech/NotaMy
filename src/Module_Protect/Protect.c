
/*
 * #################################################
 *
 *      Description:
 * This module handles the protection of sensitive data using symmetric encryption.
 * It includes functions to:
 * - Securely request and manage the user's password
 * - Generate encryption keys based on random passwords and IVs
 * - Encrypt and decrypt strings in memory
 * - Mask sensitive data with placeholders
 *
 * All functions work on a context structure (ProtectContext)
 * that encapsulates  data to be protected.
 *
 *      License:
 * This program is distributed under the terms of the GNU General Public License (GPL),
 * ensuring the freedom to redistribute and modify the software in accordance with open-source standards.
 *
 *  	Version:  1.0
 *		Created:  09/07/2025
 *
 *  	Author:
 * Catoni Mirko (IMprojtech)
 *
 * #################################################
 */

#include "Protect.h"

// --------------------------------------
/* Handler declarations */
static void generate_random_iv( char *iv, size_t size );
static void generate_key( const char *passwd, const char *iv, char *key_out );
static void crypt_data( char *str, size_t len, const char *key );
static void decrypt_data( char *str, size_t len, const char *key );

// --------------------------------------
/***** Reads a password from stdin without echoing input *****/
void write_key( char *password, size_t max_size ) {

    struct termios oldt, newt;

    printf( "Key: " );
    fflush( stdout );

    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~ECHO; // Disable input echo
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );

    fgets( password, max_size, stdin );

    size_t length = strlen( password );
    if ( length > 0 && password[length - 1] == '\n' ) {
        password[length - 1] = '\0';
    }

    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    printf( "\r\r" );
}

// --------------------------------------
/***** Generates a SHA-512 hash of input *****/
void generate_SHA512( const char *input, char *output ) {
    unsigned char hash[SHA512_DIGEST_LENGTH];

    SHA512( (const unsigned char *)input, strlen( input ), hash );

    for ( int i = 0; i < SHA512_DIGEST_LENGTH; i++ ) {
        sprintf( output + ( i * 2 ), "%02x", hash[i] );
    }

    output[SHA512_DIGEST_LENGTH * 2] = '\0';
}

// --------------------------------------
/***** Generates a random IV string *****/
static void generate_random_iv( char *iv, size_t size ) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyz"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "0123456789";
    size_t charset_len = sizeof( charset ) - 1;
    static bool seeded = false;
    if ( !seeded ) {
        srand( (unsigned int)time( NULL ) );
        seeded = true;
    }

    for ( size_t i = 0; i < size; i++ ) {
        int random_index = rand() % charset_len;
        iv[i] = charset[random_index];
    }
    iv[size] = '\0';
}

// --------------------------------------
/***** Recursive key derivation using SHA-512 *****/
static void generate_key( const char *passwd, const char *iv, char *key_out ) {

    char keytmp[KEY_SIZE];
    char hash[KEY_SIZE];
    const char *ptrPasswd = passwd;
    const char *ptrIv = iv;

    int i = 0;

    bool PWend = false;
    bool IVend = false;

    while ( i < KEY_SIZE - 1 && ( !PWend || !IVend ) ) {
        PWend = false;
        IVend = false;

        int sum = (unsigned char)*passwd + (unsigned char)*iv;
        if ( sum > 0x7E )
            sum -= 0x7E;
        keytmp[i++] = (char)sum;

        passwd++;
        iv++;

        if ( *passwd == '\0' ) {
            passwd = ptrPasswd;
            PWend = true;
        }

        if ( *iv == '\0' ) {
            iv = ptrIv;
            IVend = true;
        }
    }

    keytmp[i] = '\0';

    if ( i < KEY_SIZE - 1 ) {
        generate_SHA512( keytmp, hash );
        generate_key( keytmp, hash, key_out );
    } else {
        strncpy( key_out, keytmp, KEY_SIZE );
        key_out[KEY_SIZE - 1] = '\0';
    }
}

// --------------------------------------
/*** Encrypts a data buffer with key *****/
static void crypt_data( char *str, size_t len, const char *key ) {
    size_t keylen = strlen( key );
    if ( keylen == 0 || str == NULL || len <= 0 )
        return;

    for ( size_t i = 0; i < len; i++ ) {
        int tmp = (unsigned char)str[i] + (unsigned char)key[i % keylen];
        if ( tmp > 0x7E )
            tmp -= 0x7E;
        str[i] = (char)tmp;
    }
}

// --------------------------------------
/*** Decrypts a data buffer with key *****/
static void decrypt_data( char *str, size_t len, const char *key ) {
    size_t keylen = strlen( key );
    if ( keylen == 0 || str == NULL || len <= 0 )
        return;

    for ( size_t i = 0; i < len; i++ ) {
        int tmp = (unsigned char)str[i] - (unsigned char)key[i % keylen];
        if ( tmp < 0 )
            tmp += 0x7E;
        str[i] = (char)tmp;
    }
}

// --------------------------------------
/***** Encrypts fields in the context *****/
void protect_encrypt( const char *passwd, Protect *ctx, char *key ) {
    *( ctx->protection ) = true;
    generate_random_iv( ctx->iv, IV_SIZE );
    generate_key( passwd, ctx->iv, key );

    crypt_data( ctx->comment, strlen( ctx->comment ), key );
    crypt_data( ctx->file, strlen( ctx->file ), key );
    if ( ctx->body )
        crypt_data( ctx->body, strlen( ctx->body ), key );
}

// --------------------------------------
/***** Decrypts fields in the context *****/
void protect_decrypt( const char *passwd, Protect *ctx, char *key ) {
    generate_key( passwd, ctx->iv, key );

    decrypt_data( ctx->comment, strlen( ctx->comment ), key );
    decrypt_data( ctx->file, strlen( ctx->file ), key );
    if ( ctx->body )
        decrypt_data( ctx->body, strlen( ctx->body ), key );
}

// --------------------------------------
/***** Masks sensitive fields in the context with asterisks *****/
void mask( Protect *ctx ) {
    strcpy( ctx->comment, "*****" );
    strcpy( ctx->file, "*****" );
    if ( ctx->body )
        strcpy( ctx->body, "*****" );
}
