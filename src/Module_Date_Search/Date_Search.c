
/*
 * #################################################
 *
 *      Description:
 * This module handles date searches, offering features such as:
 * - convert dates to Unix seconds and vice versa
 * - validate and automatically complete incomplete date inputs
 * - parse search queries for absolute and relative dates for date filtering.
 *
 *      Version:  1.0
 *      Created:  10/07/2025
 *
 *      Author:
 * Catoni Mirko (IMprojtech)
 *
 * #################################################
 */

#include "Date_Search.h"

/* ============================== Initializing global variables ============================== */

char *GLOBAL_DELIMITER = " to ";
char GLOBAL_UNIT_SECONDS = 's';
char GLOBAL_UNIT_MINUTES = 'm';
char GLOBAL_UNIT_HOURS = 'h';
char GLOBAL_UNIT_DAYS = 'D';
char GLOBAL_UNIT_MONTHS = 'M';
char GLOBAL_UNIT_YEARS = 'Y';
char *DATE_FORMAT =
    "%d%n-%d%n-%d%n %d%n:%d%n:%d%n"; // Default format: year-month-day hour:minute:second

/* ============================== Error reporting function ============================== */

void error_date( const char *msg ) {
    fprintf( stderr, "[ERROR] %s\n", msg );
    exit( EXIT_FAILURE );
}

/* ============================== Utility functions for dates ============================== */

// Checks whether a year is a leap year
int leap_year( int year ) {
    return ( ( year % 4 == 0 ) && ( year % 100 != 0 ) ) || ( year % 400 == 0 );
}

// Calculates the days of the month (considering leap years)
int days_of_the_month( int month, int year ) {
    if ( month == 4 || month == 6 || month == 9 || month == 11 )
        return 30;
    if ( month == 2 )
        return ( leap_year( year ) ) ? 29 : 28;
    return 31;
}

// Converts a number of seconds (from YEAR_START) to a Date_Time structure
Date_Time seconds_to_date( long seconds ) {
    Date_Time dt = { YEAR_START, 1, 1, 0, 0, 0 };
    while ( 1 ) {
        long secs_in_year = leap_year( dt.year ) ? 366L * 86400L : 365L * 86400L;
        if ( seconds < secs_in_year )
            break;
        seconds -= secs_in_year;
        dt.year++;
    }
    dt.month = 1;
    while ( 1 ) {
        long secs_in_month = days_of_the_month( dt.month, dt.year ) * 86400L;
        if ( seconds < secs_in_month )
            break;
        seconds -= secs_in_month;
        dt.month++;
    }
    dt.day = ( seconds / 86400L ) + 1;
    seconds %= 86400L;
    dt.hour = seconds / 3600L;
    seconds %= 3600L;
    dt.minute = seconds / 60L;
    dt.second = seconds % 60L;
    return dt;
}

// Converts a Date_Time structure to seconds (from YEAR_START)
long date_to_seconds( Date_Time dt ) {
    long seconds = 0;
    for ( int y = YEAR_START; y < dt.year; y++ ) {
        seconds += leap_year( y ) ? 366L * 86400L : 365L * 86400L;
    }
    for ( int m = 1; m < dt.month; m++ ) {
        seconds += days_of_the_month( m, dt.year ) * 86400L;
    }
    seconds += ( dt.day - 1 ) * 86400L;
    seconds += dt.hour * 3600L;
    seconds += dt.minute * 60L;
    seconds += dt.second;
    return seconds;
}

// Gets the current date in seconds (from YEAR_START)
long get_current_date( void ) {
    time_t t = time( NULL );
    struct tm *tm_info = localtime( &t );
    if ( !tm_info )
        error_date( ERROR_CURRENT_DATE );
    Date_Time dt;
    dt.year = tm_info->tm_year + 1900;
    dt.month = tm_info->tm_mon + 1;
    dt.day = tm_info->tm_mday;
    dt.hour = tm_info->tm_hour;
    dt.minute = tm_info->tm_min;
    dt.second = tm_info->tm_sec;
    return date_to_seconds( dt );
}

/* ============================== Functions to obtain "calendar" boundaries ============================== */

// Returns the date with the time at the beginning of the day (00:00:00)
Date_Time get_start_of_day( Date_Time dt ) {
    dt.hour = 0;
    dt.minute = 0;
    dt.second = 0;
    return dt;
}

// Returns the date with the time at the end of the day (23:59:59)
Date_Time get_end_of_day( Date_Time dt ) {
    dt.hour = 23;
    dt.minute = 59;
    dt.second = 59;
    return dt;
}

// Returns the date with the day set to the first of the month (start of the month)
Date_Time get_start_of_month( Date_Time dt ) {
    dt.day = 1;
    dt.hour = 0;
    dt.minute = 0;
    dt.second = 0;
    return dt;
}

// Returns the date with the day set to the last of the month (end of the month)
Date_Time get_end_of_month( Date_Time dt ) {
    dt.day = days_of_the_month( dt.month, dt.year );
    dt.hour = 23;
    dt.minute = 59;
    dt.second = 59;
    return dt;
}

// Returns the date set to the first day of the year (start of year)
Date_Time get_start_of_year( Date_Time dt ) {
    dt.month = 1;
    dt.day = 1;
    dt.hour = 0;
    dt.minute = 0;
    dt.second = 0;
    return dt;
}

// Returns the date set to the last day of the year (end of year)
Date_Time get_end_of_year( Date_Time dt ) {
    dt.month = 12;
    dt.day = 31;
    dt.hour = 23;
    dt.minute = 59;
    dt.second = 59;
    return dt;
}

/* ============================== Functions to "subtract" time ============================== */

// Subtracts months from a given timestamp (in seconds)
long subtract_months( long now, int months ) {
    Date_Time dt = seconds_to_date( now );
    int totalMonths = dt.year * 12 + dt.month - 1;
    totalMonths -= months;
    dt.year = totalMonths / 12;
    dt.month = ( totalMonths % 12 ) + 1;
    int dim = days_of_the_month( dt.month, dt.year );
    if ( dt.day > dim )
        dt.day = dim;
    return date_to_seconds( dt );
}

// Subtracts years from a given timestamp (in seconds)
long subtract_years( long now, int years ) {
    Date_Time dt = seconds_to_date( now );
    dt.year -= years;
    int dim = days_of_the_month( dt.month, dt.year );
    if ( dt.day > dim )
        dt.day = dim;
    return date_to_seconds( dt );
}

// Subtracts days from a given timestamp (in seconds)
long subtract_days( long now, int days ) { return now - days * 86400L; }

/* ============================== Generic Lookup Function ============================== */

// Returns 1 if target is between lowerBound and upperBound (inclusive), 0 otherwise.
int search_generic( long target, long upperBound, long lowerBound ) {
    return ( target >= lowerBound && target <= upperBound );
}

/* ============================== Date_Time Structure Validation Function ============================== */

// Returns 1 if the structure contains a valid date/time (e.g., consistent year, month, day), 0 otherwise.
Date_Time validate_datetime( const char *input, int *parsed ) {

    if ( !input )
        error_date( ERROR_NULL_INPUT );

    Date_Time dt = { 0, 1, 1, 0, 0, 0 };
    int pos[6] = { 0 };

    *parsed = sscanf( input, DATE_FORMAT, &dt.year, &pos[0], &dt.month, &pos[1], &dt.day, &pos[2],
                      &dt.hour, &pos[3], &dt.minute, &pos[4], &dt.second, &pos[5] );

    if ( *parsed < 1 || *parsed > 6 )
        error_date( ERROR_FORMAT_DATE );
    if ( *parsed == 1 && pos[0] != (int)strlen( input ) )
        error_date( ERROR_FORMAT_DATE );
    if ( *parsed == 2 && pos[1] != (int)strlen( input ) )
        error_date( ERROR_FORMAT_DATE );
    if ( *parsed == 3 && pos[2] != (int)strlen( input ) )
        error_date( ERROR_FORMAT_DATE );
    if ( *parsed == 4 && pos[3] != (int)strlen( input ) )
        error_date( ERROR_FORMAT_DATE );
    if ( *parsed == 5 && pos[4] != (int)strlen( input ) )
        error_date( ERROR_FORMAT_DATE );
    if ( *parsed == 6 && pos[5] != (int)strlen( input ) )
        error_date( ERROR_FORMAT_DATE );

    if ( dt.year < YEAR_START || dt.year > YEAR_END )
        error_date( ERROR_VALUE_DATE );
    if ( dt.month < 1 || dt.month > 12 )
        error_date( ERROR_VALUE_DATE );
    int dim = days_of_the_month( dt.month, dt.year );
    if ( dt.day < 1 || dt.day > dim )
        error_date( ERROR_VALUE_DATE );
    if ( dt.hour < 0 || dt.hour > 23 )
        error_date( ERROR_VALUE_DATE );
    if ( dt.minute < 0 || dt.minute > 59 )
        error_date( ERROR_VALUE_DATE );
    if ( dt.second < 0 || dt.second > 59 )
        error_date( ERROR_VALUE_DATE );
    return dt;
}

/* ============================== User Input Parsing ============================== */

// The input is interpreted as a range or a single date.
search_function parse_search_absolute( const char *input, long *start, long *end ) {
    if ( !input )
        error_date( ERROR_NULL_INPUT );

    char buffer1[256] = { 0 }, buffer2[256] = { 0 };
    const char *delimiter = strstr( input, GLOBAL_DELIMITER );
    int iterations = ( delimiter != NULL ) ? 2 : 1;

    if ( iterations == 2 ) {
        int len = delimiter - input;
        if ( len >= (int)sizeof( buffer1 ) )
            error_date( ERROR_INVALID_INPUT );
        strncpy( buffer1, input, len );
        buffer1[len] = '\0';

        const char *second_part = delimiter + strlen( GLOBAL_DELIMITER );
        if ( strlen( second_part ) >= sizeof( buffer2 ) )
            error_date( ERROR_INVALID_INPUT );
        strcpy( buffer2, second_part );
    } else {
        if ( strlen( input ) >= sizeof( buffer1 ) )
            error_date( ERROR_INVALID_INPUT );
        strcpy( buffer1, input );
    }

    Date_Time date_start = { YEAR_START, 1, 1, 0, 0, 0 };
    Date_Time date_end = { YEAR_END, 12, 31, 23, 59, 59 };
    Date_Time temp = { YEAR_START, 1, 1, 0, 0, 0 };
    char *current_str = buffer1;

    for ( int i = 0; i < iterations; i++ ) {
        int parsed = 0;

        temp = validate_datetime( current_str, &parsed );

        if ( parsed == 1 ) {
            if ( i == 0 ) {
                date_start = (Date_Time){ temp.year, 1, 1, 0, 0, 0 };
                date_end = (Date_Time){ temp.year, 12, 31, 23, 59, 59 };
            } else {
                date_end = (Date_Time){ temp.year, 12, 31, 23, 59, 59 };
            }
        } else if ( parsed == 2 ) {
            if ( i == 0 ) {
                date_start = (Date_Time){ temp.year, temp.month, 1, 0, 0, 0 };
                date_end = get_end_of_month( temp );
            } else {
                date_end = get_end_of_month( temp );
            }
        } else if ( parsed == 3 ) {
            if ( i == 0 ) {
                date_start = get_start_of_day( temp );
                date_end = get_end_of_day( temp );
            } else {
                date_end = get_end_of_day( temp );
            }
        } else if ( parsed == 4 ) {
            if ( i == 0 ) {
                temp.minute = 0;
                temp.second = 0;
                date_start = temp;
                temp.minute = 59;
                temp.second = 59;
                date_end = temp;
            } else {
                temp.minute = 59;
                temp.second = 59;
                date_end = temp;
            }
        } else if ( parsed == 5 ) {
            if ( i == 0 ) {
                temp.second = 0;
                date_start = temp;
                temp.second = 59;
                date_end = temp;
            } else {
                temp.second = 59;
                date_end = temp;
            }
        } else if ( parsed == 6 ) {
            date_start = temp;
            date_end = temp;
        }

        if ( i == 0 ) {
            *start = date_to_seconds( date_start );
            *end = date_to_seconds( date_end );
        } else {
            *end = date_to_seconds( date_end );
        }
        current_str = buffer2;
    }
    return search_generic;
}

// input must be of the type "<value><unit>"
search_function parse_search_relative( const char *input, long *start, long *end ) {
    if ( !input )
        error_date( ERROR_NULL_INPUT );

    long value = 0;
    char unit = 0;
    if ( sscanf( input, "%ld%c", &value, &unit ) != 2 )
        return NULL;

    long now = get_current_date();
    Date_Time now_dt = seconds_to_date( now );

    if ( unit == GLOBAL_UNIT_SECONDS ) {
        *start = now - value;
        *end = now;
    } else if ( unit == GLOBAL_UNIT_MINUTES ) {
        *start = now - value * 60L;
        *end = now;
    } else if ( unit == GLOBAL_UNIT_HOURS ) {
        *start = now - value * 3600L;
        *end = now;
    } else if ( unit == GLOBAL_UNIT_DAYS ) {
        Date_Time start_dt = get_start_of_day( now_dt );
        Date_Time end_dt = get_end_of_day( now_dt );
        if ( value > 1 ) {
            long start_ts = date_to_seconds( start_dt ) - ( value - 1 ) * 86400L;
            start_dt = seconds_to_date( start_ts );
            start_dt = get_start_of_day( start_dt );
            *start = date_to_seconds( start_dt );
        } else {
            *start = date_to_seconds( start_dt );
        }
        *end = date_to_seconds( end_dt );
    } else if ( unit == GLOBAL_UNIT_MONTHS ) {
        Date_Time start_dt = get_start_of_month( now_dt );
        Date_Time end_dt = get_end_of_month( now_dt );
        if ( value > 1 ) {
            long start_ts = date_to_seconds( start_dt );
            start_ts = subtract_months( start_ts, value - 1 );
            start_dt = seconds_to_date( start_ts );
            start_dt = get_start_of_month( start_dt );
            *start = date_to_seconds( start_dt );
        } else {
            *start = date_to_seconds( start_dt );
        }
        *end = date_to_seconds( end_dt );
    } else if ( unit == GLOBAL_UNIT_YEARS ) {
        Date_Time start_dt = get_start_of_year( now_dt );
        Date_Time end_dt = get_end_of_year( now_dt );
        if ( value > 1 ) {
            long start_ts = date_to_seconds( start_dt );
            start_ts = subtract_years( start_ts, value - 1 );
            start_dt = seconds_to_date( start_ts );
            start_dt = get_start_of_year( start_dt );
            *start = date_to_seconds( start_dt );
        } else {
            *start = date_to_seconds( start_dt );
        }
        *end = date_to_seconds( end_dt );
    } else {
        return NULL;
    }
    if ( *start < date_to_seconds( (Date_Time){ YEAR_START, 1, 1, 0, 0, 0 } ) )
        error_date( ERROR_RANGE_DATE );
    return search_generic;
}

// Attempts to interpret the input as a relative interval; if that fails, it interprets it as an absolute interval.
search_function parse_search_date( const char *input, long *start, long *end ) {
    if ( !input )
        error_date( ERROR_NULL_INPUT );

    search_function search_func = parse_search_relative( input, start, end );
    if ( !search_func ) {
        search_func = parse_search_absolute( input, start, end );
    }

    return search_func;
}

/* ============================== Converting a String to Date_Time ============================== */

// Converts a string according to DATE_FORMAT.
Date_Time string_to_date( const char *date_str ) {
    if ( !date_str )
        error_date( ERROR_NULL_INPUT );

    int parsed = 0;

    Date_Time dt = validate_datetime( date_str, &parsed );

    if ( parsed < 3 ) {
        dt.month = 1;
        dt.day = 1;
    }
    if ( parsed < 4 )
        dt.hour = 0;
    if ( parsed < 5 )
        dt.minute = 0;
    if ( parsed < 6 )
        dt.second = 0;
    return dt;
}
