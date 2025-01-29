/* Licensed under CC BY-NC-SA 4.0 © 2025 Pazu101 */

#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "utils_functions.h" // personal dependencie not in repo (ask owner)
#include <sys/stat.h>
#include <string.h>
#include <curl/curl.h>
#include "pick_send_fun.h"

struct upload_status 
{
    const char* readptr;
    size_t bytes_left;
};

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
    struct upload_status *upload_ctx = (struct upload_status *)userp;
    size_t buffer_size = size * nmemb;

    if (upload_ctx->bytes_left == 0) {
        return 0;
    }

    size_t bytes_to_send = (upload_ctx->bytes_left < buffer_size) ? upload_ctx->bytes_left : buffer_size;
    memcpy(ptr, upload_ctx->readptr, bytes_to_send);

    upload_ctx->readptr  += bytes_to_send;
    upload_ctx->bytes_left -= bytes_to_send;

    return bytes_to_send;
}

int sum_quotes_weight(sqlite3* db) // sum and return weight of all quotes in database
{
    const char* sql_get_weight = "SELECT weight FROM quotes;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql_get_weight, -1, &stmt, NULL) != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        exit_helper("Failed to prepare statement\n");
    }
    int total_weight = 0; 
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int weight = sqlite3_column_int(stmt, 0);
        //printf("Log: current weight: %d\n", weight);
        total_weight += weight;
    }
    sqlite3_finalize(stmt);
    return total_weight;
}
int pick_quote(sqlite3* db, int total_shares) // pick and return ID of a quote based on ponderation calculation
{
    int id = -1;
    int cumulative_weight = 0;
    int random_number = rand() % total_shares;
    printf("Random number is : %d\n", random_number);
    const char* sql_get_weight_and_id = "SELECT id, weight FROM quotes;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql_get_weight_and_id, -1, &stmt, NULL) != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        exit_helper("Failed to prepare statement\n");
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) //sum each weight quote for cumulative weight and stops when greater than random number found 
    {
        int weight = sqlite3_column_int(stmt, 1);
        cumulative_weight += weight;
        if (random_number < cumulative_weight)
        {
            id = sqlite3_column_int(stmt, 0);
            printf("Cumulative weight : %d\n", cumulative_weight);
            break;            }
    }
    sqlite3_finalize(stmt);
    return id;
}

void get_and_update_weight(sqlite3* db, int id) // set the weight of ID picked to zero
{
    const char* sql_get_weight = "SELECT weight FROM quotes WHERE id = ?;";
    sqlite3_stmt* stmt_get;
    if (sqlite3_prepare_v2(db, sql_get_weight, -1, &stmt_get, NULL) != SQLITE_OK)
    {
        sqlite3_finalize(stmt_get);
        exit_helper("Failed to prepare statement\n");
    }
    if (sqlite3_bind_int(stmt_get, 1, id) != SQLITE_OK)
    {
        sqlite3_finalize(stmt_get);
        exit_helper("Failed to bind ID to fetch weight\n");
    }
    int current_weight = -1;
    if (sqlite3_step(stmt_get) == SQLITE_ROW)
    {
        current_weight = sqlite3_column_int(stmt_get, 0);
        sqlite3_finalize(stmt_get);
    }
    else
    {
        sqlite3_finalize(stmt_get);
        exit_helper("Failed to get weight");
    }
    const char* sql_reset_weight = "UPDATE quotes SET weight = 0 WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql_reset_weight, -1, &stmt, NULL) != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        exit_helper("Failed to prepare statement\n");
    }
    if (sqlite3_bind_int(stmt, 1, id) != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        exit_helper("Failed to bind ID\n");
    }

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        exit_helper("Reset of weight failed\n");
    }
    else
    {
        sqlite3_finalize(stmt);
        printf("Quote weight was %d\n", current_weight);
        printf("Now set to 1\n");
    }
}

void increment_weight(sqlite3* db, int id) // increment all weight under 10 of 1 (including the ID picked)
{
    const char* sql_increment_weights = "UPDATE quotes SET weight = weight + 1 WHERE weight < 10;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql_increment_weights, -1, &stmt, NULL) != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        exit_helper("Failed to prepare statement\n");
    }
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        exit_helper("Failed increment weights\n");
    }
    else
    {
        sqlite3_finalize(stmt);
        printf("Weights <10 incremented\n");
    }
}

void fetch_quote(sqlite3* db, int id) // get the text, book title and author of ID picked
{
    const char* sql_quote_from_id = "SELECT text, author, book FROM quotes WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql_quote_from_id, -1, &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    }
    else 
    {
        if (sqlite3_bind_int(stmt, 1, id) != SQLITE_OK) 
        {
            fprintf(stderr, "Failed to bind ID: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
        }
        else
        {
            if (sqlite3_step(stmt) == SQLITE_ROW) 
            {
                // Fetch the columns
                const char* quote_text = (const char*)sqlite3_column_text(stmt, 0); // First column: text
                const char* author = (const char*)sqlite3_column_text(stmt, 1);     // Second column: author
                const char* book = (const char*)sqlite3_column_text(stmt, 2);       // Third column: book

                printf("Quote %d fetched\n", id);
                send_email(db, id, quote_text, author, book);
            }
            else 
            {
                fprintf(stderr, "Quote not found for ID %d\n", id);
            }
        }
    sqlite3_finalize(stmt);
}
}
int file_exists(const char* filename) // function to check if database exists. Return 0 if no 1 if ok. 
{
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

int send_email(sqlite3* db, int id, const char* quote_text,const char* author,const char* book) // send the formatted mail with the quote of ID picked 
{
    
    CURL* curl;
    CURLcode res;
    struct curl_slist *recipients = NULL;
    const char* stmp_url = "your smtp port";
    const char* username = "your account email";
    const char* app_password = "your app password";
    const char* from_email = "<sender email>";
    const char* to_email = "<email to send>";

    char subject[MID_BUFFER];
    char body[MAX_TEXT];
    snprintf(subject, MID_BUFFER, "Aujourd'hui une citation de %s !", author);
    snprintf(body, MAX_TEXT, "Dans %s, %s a dit : <br><br>\"<i>%s</i>\"<br><br>À bientôt et continue à lire 🎈", book, author, quote_text);

    char payload[MAX_TEXT];
    int payload_len = snprintf(payload, sizeof(payload),
        "From: %s\r\n"
        "To: %s\r\n"
        "Subject: %s\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "\r\n" //end of header
        "%s\r\n", // mail body
        to_email, from_email, subject, body);
    if (payload_len < 0)
    {
        exit_helper("Error: Payload empty\n");
    }

    struct upload_status upload_ctx;
    upload_ctx.readptr   = payload;
    upload_ctx.bytes_left = payload_len;

    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "curl_easy_init() failed.\n");
        curl_global_cleanup();
        return 0;
    }
    printf("SMTP init OK\n");
    curl_easy_setopt(curl, CURLOPT_URL,stmp_url);
    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    curl_easy_setopt(curl, CURLOPT_USERNAME, username);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, app_password);
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from_email);
    recipients = curl_slist_append(recipients, to_email);
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) 
    {   
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
    else
    {
        printf("Email sent to %s\n", to_email);
    }
    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return (res == CURLE_OK) ? 1 : 0;
}