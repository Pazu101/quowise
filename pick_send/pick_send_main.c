/* Licensed under CC BY-NC-SA 4.0 © 2025 Pazu101 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "pick_send_fun.h"
#include "sqlite3.h"
#include "utils_functions.h" // personal dependencie not in repo (ask owner)

int main()
{
    srand(time(NULL));
    sqlite3* db;
    int rc;
    const char* db_name = "quowise/quotes_empty.db"; //this database is empty but ready to work with code
    if (!file_exists(db_name))
    {
        exit_helper("Database not found");
    }
    else
    {
        printf("DB found OK\n");
    }
    rc = sqlite3_open(db_name, &db);
    if (rc)
    {
        exit_helper("SQLite opening error\n");
    }
    else
    {
        printf("DB open OK\n");
    }
    
    int total_shares = sum_quotes_weight(db);
    if (total_shares < 0)
    {
        exit_helper("Invalid total weight of quotes\n");
    }
    else 
    {
        printf("Total weight calculated : %d\n", total_shares);
    }
    int id_quote_picked = pick_quote(db, total_shares);
    if (id_quote_picked < 0)
    {
        exit_helper("Invalid ID picked\n");
    }
    else
    {
        printf("ID picked : %d\n", id_quote_picked);
    }
fetch_quote(db, id_quote_picked); // this function calls the send_email if quote found then return to main
get_and_update_weight(db, id_quote_picked);
increment_weight(db, id_quote_picked);
sqlite3_close(db);
    return 0;
}