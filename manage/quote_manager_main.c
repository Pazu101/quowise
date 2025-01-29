/* Licensed under CC BY-NC-SA 4.0 © 2025 Pazu101 */

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "quote_manager_fun.h" // personal dependencie not in repo (ask owner)

int main()
{
    sqlite3* db;
    int rc;
const char* db_name = "quowise/quotes_empty.db"; //this database is empty but ready for the code
if (!file_exists(db_name))
{
    exit_helper("Database not found");
}
rc = sqlite3_open(db_name, &db);
if (rc)
{
    exit_helper("SQLite opening error\n");
}
insert_quote(db);
sqlite3_close(db);
    return 0;
}