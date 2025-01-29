/* Licensed under CC BY-NC-SA 4.0 © 2025 Pazu101 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <sys/stat.h>
#include "quote_manager_fun.h"
#include "utils_functions.h" // personal dependencie not in repo (ask owner)

int file_exists(const char* filename) // function to check if database exists. Return 0 if no 1 if ok. 
{
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

void insert_quote(sqlite3* db) //insertion of a quote with user input sanitized 
{
    char text[MAX_TEXT];
    char author[SMALL_BUFFER];
    char book[SMALL_BUFFER];

    printf("Enter your quote (max %d characters):\n", MAX_TEXT -1);
    clean_user_input(text, MAX_TEXT);
    printf("Enter author name:\n");
    clean_user_input(author, SMALL_BUFFER);
    printf("Enter book name:\n");
    clean_user_input(book, SMALL_BUFFER);

    const char* sql_insert = "INSERT INTO quotes (text, author, book) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql_insert, -1, &stmt, NULL) != SQLITE_OK)
    {
        exit_helper("Failed to prepare statement\n");
    }
    sqlite3_bind_text(stmt, 1, text, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, author, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, book, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        exit_helper("Failed to add quote\n");
    }
    else 
    {
        printf("Quote added successfully\n");
    }
    sqlite3_finalize(stmt);
}