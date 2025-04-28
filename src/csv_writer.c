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

// Função para criar o diretório de arquivos CSV
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

// Função para encontrar o índice de um campo pelo nome
unsigned int find_field_index(MYSQL_FIELD* fields, unsigned int num_fields, const char* field_name) {
    for (unsigned int i = 0; i < num_fields; i++) {
        if (strcmp(fields[i].name, field_name) == 0) {
            return i;
        }
    }
    return (unsigned int)-1;
}

// Função para escrever os registros no arquivo CSV
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

    // Find field indices
    unsigned int cbo_idx = find_field_index(fields, num_fields, "cbo");
    unsigned int mae_idx = find_field_index(fields, num_fields, "mae");
    unsigned int nota_idx = find_field_index(fields, num_fields, "nota");
    unsigned int data_atualizacao_idx = find_field_index(fields, num_fields, "data_atualizacao");

    // Write data
    MYSQL_ROW row;
    char cleaned_field[1024]; // Buffer para o campo limpo
    int row_count = 0;

    while ((row = mysql_fetch_row(result))) {
        for (unsigned int i = 0; i < num_fields; i++) {
            if (row[i]) {
                clean_field(cleaned_field, row[i], sizeof(cleaned_field));

                if (strcmp(fields[i].name, "sexo") == 0) {
                    if (cbo_idx != (unsigned int)-1 && row[cbo_idx]) {
                        clean_field(cleaned_field, row[cbo_idx], sizeof(cleaned_field));
                    }
                }
                if (strcmp(fields[i].name, "cbo") == 0) {
                    if (mae_idx != (unsigned int)-1 && row[mae_idx]) {
                        clean_field(cleaned_field, row[mae_idx], sizeof(cleaned_field));
                    }
                }
                if (strcmp(fields[i].name, "mae") == 0) {
                    if (data_atualizacao_idx != (unsigned int)-1 && row[data_atualizacao_idx]) {
                        clean_field(cleaned_field, row[data_atualizacao_idx], sizeof(cleaned_field));
                    }
                }
                if (strcmp(fields[i].name, "data_atualizacao") == 0) {
                    if (nota_idx != (unsigned int)-1 && row[nota_idx]) {
                        clean_field(cleaned_field, row[nota_idx], sizeof(cleaned_field));
                    }
                }

                fprintf(file, "\"%s\"", cleaned_field);
            }
            if (i < num_fields - 1) fprintf(file, ";");
        }
        fprintf(file, "\n");
        row_count++;
    }

    fclose(file);
    return 1;
} 