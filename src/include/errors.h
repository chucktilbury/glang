#ifndef __ERRORS_H__
#define __ERRORS_H__

#include <stdio.h>

void init_errors(FILE* fp);
void syntax(const char* str, ...);
void warning(const char* str, ...);
void fatal_error(const char* str, ...);
void command_error(const char* str, ...);

int get_num_errors();
int get_num_warnings();
void inc_error_count();
void inc_warning_count();
FILE* get_err_stream();

#endif