#include "common.h"
#include <stdarg.h>

#include "scanner.h"

static struct {
    FILE* fp;
    int errors;
    int warnings;
} errors;

// messages longer than this will be truncated to this length.
static char msg_buff[132];

static void report() {
    fprintf(errors.fp, "    errors: %d warnings: %d\n", errors.errors, errors.warnings);
}

void init_errors(FILE* fp) {

    errors.fp = fp;   // If this is NULL, then stderr will be used.
    errors.errors = 0;
    errors.warnings = 0;
    atexit(report);
}

void syntax(const char* str, ...) {

    va_list args;
    const char* name = get_file_name();
    int lnum = get_line_no();
    int col = get_column_no();

    if(lnum > 0)
        snprintf(msg_buff, sizeof(msg_buff), "Syntax Error: %s: %d: %d: ", name, lnum, col);
    else
        snprintf(msg_buff, sizeof(msg_buff), "Syntax Error: ");

    int len = strlen(msg_buff);

    va_start(args, str);
    vsnprintf(&msg_buff[len], sizeof(msg_buff) - len, str, args);
    va_end(args);
    errors.errors++;
    fprintf(stderr, "%s\n", msg_buff);
}

void warning(const char* str, ...) {

    va_list args;
    const char* name = get_file_name();
    int lnum = get_line_no();
    int col = get_column_no();

    if(lnum > 0)
        snprintf(msg_buff, sizeof(msg_buff), "Warning: %s: %d: %d: ", name, lnum, col);
    else
        snprintf(msg_buff, sizeof(msg_buff), "Warning: ");

    int len = strlen(msg_buff);

    va_start(args, str);
    vsnprintf(&msg_buff[len], sizeof(msg_buff) - len, str, args);
    va_end(args);
    errors.warnings++;
    fprintf(stderr, "%s\n", msg_buff);
}

void fatal_error(const char* str, ...) {

    va_list args;

    snprintf(msg_buff, sizeof(msg_buff), "FATAL ERROR: ");

    int len = strlen(msg_buff);

    va_start(args, str);
    vsnprintf(&msg_buff[len], sizeof(msg_buff) - len, str, args);
    va_end(args);
    errors.errors++;
    fprintf(stderr, "%s\n", msg_buff);
    exit(1);
}

int get_num_errors() {

    return errors.errors;
}

int get_num_warnings() {

    return errors.warnings;
}

void inc_error_count() {

    errors.errors ++;
}

void inc_warning_count() {
    errors.warnings ++;
}

FILE* get_err_stream() {
    return errors.fp;
}