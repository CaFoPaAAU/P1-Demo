#pragma once
#include <stdbool.h>

typedef enum error_type_t {
	ACCESS_VIOLATION, NO_MEMORY, UNEXPECTED_VALUE, BAD_INPUT, OPEN_FILE, INVALID_ID
}error_type_t;


void print_groups(const dict_t *d);
void print_table(table_t *table, dict_t *d, int n, bool debug);
void hyphen_chain(int count);
void error_handler(error_type_t error_type);
void print_table_full(table_t *table, dict_t *d, int n);
