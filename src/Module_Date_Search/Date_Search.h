
/*
 * #################################################
 *
 *              Description:
 * Header associated with Date_Search.c.
 * Contains the declaration of the Date_Time structure and function prototypes.
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

#ifndef DATE_SEARCH_H
#define DATE_SEARCH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

/* ============================== Definizioni di errori ============================== */

#define ERROR_INVALID_INPUT "Invalid input."
#define ERROR_FORMAT_DATE "Invalid date format."
#define ERROR_VALUE_DATE "Incorrect value."
#define ERROR_RANGE_DATE "Date value out of range."
#define ERROR_NULL_INPUT "Null input."
#define ERROR_CURRENT_DATE "Retrieving current date failed."

#ifdef __cplusplus
extern "C" {
#endif

/* ============================== Variabili globali configurabili ============================== */

extern char *GLOBAL_DELIMITER;   // Delimitatore per intervalli assoluti
extern char GLOBAL_UNIT_SECONDS; // 's'
extern char GLOBAL_UNIT_MINUTES; // 'm'
extern char GLOBAL_UNIT_HOURS;   // 'h'
extern char GLOBAL_UNIT_DAYS;    // 'D'
extern char GLOBAL_UNIT_MONTHS;  // 'M'
extern char GLOBAL_UNIT_YEARS;   // 'Y'

extern char
    *DATE_FORMAT; // Esempio: "%d-%d-%d %d:%d:%d" (ordine: anno, mese, giorno, ora, minuto, secondo)

/* ============================== Definizioni e strutture base ============================== */

#define YEAR_START 1970
#define YEAR_END 2100

typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} Date_Time;

typedef int ( *search_function )( long target, long upperBound, long lowerBound );

/* ============================== Dichiarazioni delle funzioni pubbliche ============================== */

void error_date( const char *msg );

int leap_year( int year );
int days_of_the_month( int month, int year );
Date_Time seconds_to_date( long seconds );
long date_to_seconds( Date_Time dt );
long get_current_date( void );

Date_Time get_start_of_day( Date_Time dt );
Date_Time get_end_of_day( Date_Time dt );
Date_Time get_start_of_month( Date_Time dt );
Date_Time get_end_of_month( Date_Time dt );
Date_Time get_start_of_year( Date_Time dt );
Date_Time get_end_of_year( Date_Time dt );

long subtract_months( long now, int months );
long subtract_years( long now, int years );
long subtract_days( long now, int days );

int search_generic( long target, long upperBound, long lowerBound );

Date_Time validate_datetime( const char *input, int *parsed );

search_function parse_search_absolute( const char *input, long *start, long *end );
search_function parse_search_relative( const char *input, long *start, long *end );
search_function parse_search_date( const char *input, long *start, long *end );

Date_Time string_to_date( const char *date_str );

#ifdef __cplusplus
}
#endif

#endif // DATE_SEARCH_H
