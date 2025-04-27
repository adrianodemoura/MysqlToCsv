#include "mysql_connector.h"
#include <stdio.h>
#include <string.h>

MYSQL* init_mysql_connection(Config* config) {
    MYSQL* conn = mysql_init(NULL);
    if (!conn) {
        fprintf(stderr, "mysql_init() failed\n");
        return NULL;
    }

    if (!mysql_real_connect(conn, config->mysql_host, config->mysql_user,
                           config->mysql_pass, config->mysql_db, 0, NULL, 0)) {
        fprintf(stderr, "mysql_real_connect() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    return conn;
}

long get_total_records(MYSQL* conn, const char* table_name) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM %s", table_name);

    if (mysql_query(conn, query) != 0) {
        fprintf(stderr, "mysql_query() failed: %s\n", mysql_error(conn));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (!result) {
        fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(conn));
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    long count = row ? atol(row[0]) : -1;

    mysql_free_result(result);
    return count;
}

int fetch_records(MYSQL* conn, const char* table_name, long offset, long limit, MYSQL_RES** result) {
    char query[512];
    snprintf(query, sizeof(query), 
             "SELECT * FROM %s LIMIT %ld OFFSET %ld", 
             table_name, limit, offset);

    if (mysql_query(conn, query) != 0) {
        fprintf(stderr, "mysql_query() failed: %s\n", mysql_error(conn));
        return 0;
    }

    *result = mysql_store_result(conn);
    if (!*result) {
        fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
} 