/* Licensed under CC BY-NC-SA 4.0 © 2025 Pazu101 */

#ifndef PICK_SEND_FUN_H
#define PICK_SEND_FUN_H
#include "sqlite3.h"
#define MAX_TEXT 2000 // Defines to max length of a quote. 

int sum_quotes_weight(sqlite3* db);
void get_and_update_weight(sqlite3* db, int id);
void increment_weight(sqlite3* db, int id);
void fetch_quote(sqlite3* db, int id);
int pick_quote(sqlite3* db, int total_shares);
static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp);
int file_exists(const char* filename);
int send_email(sqlite3* db, int id, const char* quote_text,const char* author,const char* book);
#endif