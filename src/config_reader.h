#ifndef CONFIG_READER_H
#define CONFIG_READER_H

typedef struct {
    char mysql_host[256];
    char mysql_user[256];
    char mysql_pass[256];
    char mysql_db[256];
    char mysql_tabela[256];
    int records_per_page;
    int total_records;
} Config;

int load_config(Config* config);

#endif // CONFIG_READER_H 