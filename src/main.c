#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <mysql/mysql.h>
#include <json-c/json.h>
#include <sys/resource.h>
#include <unistd.h>

#include "mysql_connector.h"
#include "csv_writer.h"
#include "memory_manager.h"
#include "config_reader.h"

#define MAX_THREADS 4
#define MAX_MEMORY_USAGE 0.7 // 70% of total RAM

typedef struct {
    int thread_id;
    long start_offset;
    long end_offset;
    Config* config;
} ThreadData;

void* process_data_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    // Initialize MySQL connection for this thread
    MYSQL* conn = init_mysql_connection(data->config);
    if (!conn) {
        fprintf(stderr, "Thread %d: Failed to connect to MySQL\n", data->thread_id);
        return NULL;
    }
    
    // Process in smaller chunks to manage memory
    const long CHUNK_SIZE = data->config->records_per_page;
    for (long offset = data->start_offset; offset < data->end_offset; offset += CHUNK_SIZE) {
        // Check memory usage before processing
        if (!check_memory_limits()) {
            fprintf(stderr, "Thread %d: Memory limit reached, waiting...\n", data->thread_id);
            sleep(1);
            continue;
        }

        long current_chunk = (offset + CHUNK_SIZE > data->end_offset) ? 
                            (data->end_offset - offset) : CHUNK_SIZE;

        MYSQL_RES* result = NULL;
        if (!fetch_records(conn, data->config->mysql_tabela, offset, current_chunk, &result)) {
            fprintf(stderr, "Thread %d: Failed to fetch records\n", data->thread_id);
            continue;
        }

        // Generate filename for this chunk
        char filename[256];
        long page_number = (offset / CHUNK_SIZE) + 1;
        snprintf(filename, sizeof(filename), "pagina_%04ld.csv", page_number);

        // Write to CSV
        if (!write_records_to_csv(result, filename)) {
            fprintf(stderr, "Thread %d: Failed to write CSV file\n", data->thread_id);
        }

        mysql_free_result(result);
    }

    mysql_close(conn);
    return NULL;
}

int main() {
    // Initialize configuration
    Config config;
    if (!load_config(&config)) {
        fprintf(stderr, "Failed to load configuration\n");
        return 1;
    }

    // Set memory limits
    size_t total_mem = get_total_memory();
    if (!set_memory_limits(total_mem * MAX_MEMORY_USAGE)) {
        fprintf(stderr, "Failed to set memory limits\n");
        return 1;
    }

    // Initialize MySQL connection
    MYSQL* conn = init_mysql_connection(&config);
    if (!conn) {
        fprintf(stderr, "Failed to connect to MySQL\n");
        return 1;
    }

    // Calculate number of pages
    long total_pages = (config.total_records + config.records_per_page - 1) / config.records_per_page;

    // Create threads
    pthread_t threads[MAX_THREADS];
    ThreadData thread_data[MAX_THREADS];
    int thread_count = 0;

    for (long page = 0; page < total_pages; page++) {
        if (thread_count >= MAX_THREADS) {
            // Wait for a thread to finish
            pthread_join(threads[thread_count % MAX_THREADS], NULL);
        }

        thread_data[thread_count % MAX_THREADS].thread_id = thread_count;
        thread_data[thread_count % MAX_THREADS].start_offset = page * config.records_per_page;
        thread_data[thread_count % MAX_THREADS].end_offset = (page + 1) * config.records_per_page;
        thread_data[thread_count % MAX_THREADS].config = &config;

        if (pthread_create(&threads[thread_count % MAX_THREADS], NULL, 
                          process_data_thread, &thread_data[thread_count % MAX_THREADS]) != 0) {
            fprintf(stderr, "Failed to create thread %d\n", thread_count);
            continue;
        }

        thread_count++;
    }

    // Wait for remaining threads
    for (int i = 0; i < MAX_THREADS; i++) {
        if (threads[i]) {
            pthread_join(threads[i], NULL);
        }
    }

    mysql_close(conn);
    return 0;
}
