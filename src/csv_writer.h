#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include <mysql/mysql.h>

int write_records_to_csv(MYSQL_RES* result, const char* filename);
int create_csv_directory();

#endif // CSV_WRITER_H 