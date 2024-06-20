#include "list.h"
#include <assert.h>
#include <stdlib.h>

typedef struct list {
  void **data;
  size_t length;
  size_t size;
  free_func_t freer;
} list_t;

list_t *list_init(size_t initial_capacity, free_func_t freer) {
  list_t *arr = malloc(sizeof(list_t));
  assert(arr != NULL);
  arr->length = initial_capacity;
  arr->data = malloc(sizeof(void *) * (initial_capacity + 1));
  assert(arr->data != NULL);
  arr->size = 0;
  arr->freer = freer;
  return arr;
}

void list_free(list_t *list) {
  if (list->freer != NULL) {
    for (size_t i = 0; i < list_size(list); i++) {
      list->freer(list->data[i]);
    }
  }
  free(list->data);
  free(list);
}

size_t list_size(list_t *list) { return list->size; }

void *list_get(list_t *list, size_t index) {
  assert(index < list->size);
  assert(index >= 0);
  return list->data[index];
}

void list_add(list_t *list, void *value) {
  assert(value != NULL);
  if (list->size >= list->length) {
    void **newData = malloc(sizeof(void *) * 2 * (list->length));
    assert(newData != NULL);
    for (size_t i = 0; i < list->length; i++) {
      newData[i] = list->data[i];
    }
    newData[list->size] = value;
    free(list->data);
    list->data = newData;
    list->length = 2 * (list->length);
    list->size++;
    for (size_t i = list->size; i < list->length; i++) {
      list->data[i] = NULL;
    }
  } else {
    list->data[list->size] = value;
    list->size++;
  }
}

void *list_remove(list_t *list, size_t index) {
  assert(list->size != 0 && index < list->size);
  void *to_return = list->data[index];
  if (index == list_size(list) - 1) {
    list->size--;
    list->data[index] = NULL;
    return to_return;
  }
  for (size_t i = index; i < list->length - 1; i++) {
    list->data[i] = list->data[i + 1];
  }
  list->data[list->size - 1] = NULL;
  list->size--;
  return to_return;
}
