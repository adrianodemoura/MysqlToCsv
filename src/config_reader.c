#include "config_reader.h"
#include <stdio.h>
#include <string.h>
#include <json-c/json.h>

int load_config(Config* config) {
    struct json_object* json;
    json = json_object_from_file("config/config.json");
    
    if (!json) {
        fprintf(stderr, "Failed to parse config file\n");
        return 0;
    }

    struct json_object* host, *user, *pass, *db, *tabela, *records_per_page, *total_records;
    
    if (!json_object_object_get_ex(json, "mysql_host", &host) ||
        !json_object_object_get_ex(json, "mysql_user", &user) ||
        !json_object_object_get_ex(json, "mysql_pass", &pass) ||
        !json_object_object_get_ex(json, "mysql_db", &db) ||
        !json_object_object_get_ex(json, "mysql_tabela", &tabela) ||
        !json_object_object_get_ex(json, "records_per_page", &records_per_page) ||
        !json_object_object_get_ex(json, "total_records", &total_records)) {
        fprintf(stderr, "Missing required configuration fields\n");
        json_object_put(json);
        return 0;
    }

    strncpy(config->mysql_host, json_object_get_string(host), sizeof(config->mysql_host) - 1);
    strncpy(config->mysql_user, json_object_get_string(user), sizeof(config->mysql_user) - 1);
    strncpy(config->mysql_pass, json_object_get_string(pass), sizeof(config->mysql_pass) - 1);
    strncpy(config->mysql_db, json_object_get_string(db), sizeof(config->mysql_db) - 1);
    strncpy(config->mysql_tabela, json_object_get_string(tabela), sizeof(config->mysql_tabela) - 1);
    
    config->records_per_page = json_object_get_int(records_per_page);
    config->total_records = json_object_get_int(total_records);

    json_object_put(json);
    return 1;
} 