#include "csv_writer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

// Função para limpar o campo removendo quebras de linha e retornos de carro
void clean_field(char* dest, const char* src, size_t max_len) {
    size_t j = 0;
    for (size_t i = 0; src[i] && j < max_len - 1; i++) {
        if (src[i] != '\n' && src[i] != '\r') {
            dest[j++] = src[i];
        }
    }
    dest[j] = '\0';
}

int create_csv_directory() {
    struct stat st = {0};
    if (stat("files_csv", &st) == -1) {
        if (mkdir("files_csv", 0755) == -1) {
            fprintf(stderr, "Failed to create directory: %s\n", strerror(errno));
            return 0;
        }
    }
    return 1;
}

int write_records_to_csv(MYSQL_RES* result, const char* filename) {
    if (!create_csv_directory()) {
        return 0;
    }

    char full_path[512];
    snprintf(full_path, sizeof(full_path), "files_csv/%s", filename);

    FILE* file = fopen(full_path, "w");
    if (!file) {
        fprintf(stderr, "Failed to open file %s: %s\n", full_path, strerror(errno));
        return 0;
    }

    // Write header
    MYSQL_FIELD* fields = mysql_fetch_fields(result);
    unsigned int num_fields = mysql_num_fields(result);
    
    for (unsigned int i = 0; i < num_fields; i++) {
        fprintf(file, "\"%s\"", fields[i].name);
        if (i < num_fields - 1) fprintf(file, ";");
    }
    fprintf(file, "\n");

    // Write data
    MYSQL_ROW row;
    char cleaned_field[1024]; // Buffer para o campo limpo
    
    while ((row = mysql_fetch_row(result))) {
        for (unsigned int i = 0; i < num_fields; i++) {
            if (row[i]) {
                clean_field(cleaned_field, row[i], sizeof(cleaned_field));
                fprintf(file, "\"%s\"", cleaned_field);
            } else {
                fprintf(file, "\"\"");
            }
            if (i < num_fields - 1) fprintf(file, ";");
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 1;
} 