#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// === Core I/O ===

// Print string (null-terminated)
void strict_print(const char *s) {
    puts(s);
}

// Input integer
int strict_input() {
    int v;
    if (scanf("%d", &v) != 1) return 0;
    return v;
}

// === Dynamic List Implementation ===

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} StrictList;

StrictList* __list_new() {
    StrictList *list = (StrictList*)malloc(sizeof(StrictList));
    list->size = 0;
    list->capacity = 4;
    list->data = (int*)malloc(list->capacity * sizeof(int));
    return list;
}

void __list_append(StrictList *list, int value) {
    if (list->size >= list->capacity) {
        list->capacity *= 2;
        list->data = (int*)realloc(list->data, list->capacity * sizeof(int));
    }
    list->data[list->size++] = value;
}

void __list_remove(StrictList *list, int value) {
    for (size_t i = 0; i < list->size; i++) {
        if (list->data[i] == value) {
            memmove(&list->data[i], &list->data[i+1], (list->size - i - 1) * sizeof(int));
            list->size--;
            return;
        }
    }
}

int __list_get(StrictList *list, size_t idx) {
    if (idx >= list->size) return 0;
    return list->data[idx];
}

size_t __list_size(StrictList *list) {
    return list->size;
}

// === Array Implementation ===

typedef struct {
    int *data;
    size_t length;
} StrictArray;

StrictArray* __array_new(size_t length) {
    StrictArray *arr = (StrictArray*)malloc(sizeof(StrictArray));
    arr->length = length;
    arr->data = (int*)calloc(length, sizeof(int));
    return arr;
}

void __array_store(StrictArray *arr, size_t idx, int value) {
    if (idx < arr->length) {
        arr->data[idx] = value;
    }
}

int __array_load(StrictArray *arr, size_t idx) {
    if (idx < arr->length) {
        return arr->data[idx];
    }
    return 0;
}

// === Match Helpers ===

int __match_int(int value, int pattern) {
    return value == pattern;
}

int __match_range(int value, int low, int high) {
    return (value >= low && value <= high);
}

int __match_lt(int value, int limit) {
    return value < limit;
}

int __match_gt(int value, int limit) {
    return value > limit;
}
