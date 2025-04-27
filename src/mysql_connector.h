#ifndef MYSQL_CONNECTOR_H
#define MYSQL_CONNECTOR_H

#include <mysql/mysql.h>
#include "config_reader.h"

MYSQL* init_mysql_connection(Config* config);
long get_total_records(MYSQL* conn, const char* table_name);
int fetch_records(MYSQL* conn, const char* table_name, long offset, long limit, MYSQL_RES** result);

#endif // MYSQL_CONNECTOR_H 