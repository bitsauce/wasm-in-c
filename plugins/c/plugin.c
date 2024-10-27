#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int sum(const int a, const int b) {
    return a + b;
}

char* get_heap_allocated_string() {
    const char* str = "Greetings from the C plugin!";
    char* heap_str = malloc(strlen(str) + 1);
    strcpy(heap_str, str);
    heap_str[strlen(str)] = '\0';
    printf("ptr: %lld\n", (intptr_t)heap_str);
    return heap_str;
}

void free_heap_allocated_string(char* str) {
    memset(str, 0, strlen(str));
    free(str);
}

void test_print() {
    printf("test_print(): Printing to stdout\n");
    fprintf(stderr, "test_print(): Printing to stderr\n");
}

void test_file_io() {
    const char* filename = "hello_c.txt";
    const char* contents = "Hello from C!";

    // Write file
    {
        FILE* file = fopen(filename, "w");
        if (!file) {
            fprintf(stderr, "test_file_io(): Failed to open file for writing\n");
            return;
        }
        fprintf(file, "%s", contents);
        fclose(file);
    }

    // Read file
    {
        FILE* file = fopen(filename, "r");
        if (!file) {
            fprintf(stderr, "test_file_io(): Failed to open file for reading\n");
            return;
        }
        char* read = malloc(strlen(contents) + 1);
        fgets(read, strlen(contents), file);
        read[strlen(contents)] = '\0';
        fclose(file);
        if (strcmp(read, contents) == 0) {
            fprintf(stdout, "test_file_io(): File content is as expected\n");
        }
        else {
            fprintf(stderr, "test_file_io(): File content is not as expected\n");
        }
    }
}

__attribute__((import_name("host_fn"))) extern int host_fn();

void test_host_fn() {
    printf("test_host_fn(): Got value: %d\n", host_fn());
}
