/* Licensed under CC BY-NC-SA 4.0 © 2025 Pazu101 */

#ifndef quote_manager_fun_H
#define quote_manager_fun_H
#include "sqlite3.h"
#define MAX_TEXT 2000 // Defines to max length of a quote. 

void insert_quote(sqlite3* db);
int file_exists(const char* filename);

#endif