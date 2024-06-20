#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>

#include "asset.h"
#include "asset_cache.h"
#include "list.h"
#include "sdl_wrapper.h"

static list_t *ASSET_CACHE;

const size_t FONT_SIZE = 18;
const size_t INITIAL_CAPACITY = 5;

typedef struct {
  asset_type_t type;
  const char *filepath;
  void *obj;
} entry_t;

static void asset_cache_free_entry(entry_t *entry) {
  assert(entry);
  if (entry->type == ASSET_IMAGE) {
    SDL_DestroyTexture(entry->obj);
  } else if (entry->type == ASSET_FONT) {
    TTF_CloseFont(entry->obj);
  } else if (entry->type == ASSET_BUTTON) {
    free(entry->obj);
  }
  free(entry);
}

void asset_cache_init() {
  ASSET_CACHE =
      list_init(INITIAL_CAPACITY, (free_func_t)asset_cache_free_entry);
}

void asset_cache_destroy() { list_free(ASSET_CACHE); }

/**
 * Checks whether the given entry already corresponds to a filepath
 * @param filepath filepath as a string
 * @return the entry corresponding to the filepath, if it exists
 * @return NULL otherwise
 */
entry_t *asset_found(const char *filepath) {
  for (size_t i = 0; i < list_size(ASSET_CACHE); i++) {
    entry_t *curr = list_get(ASSET_CACHE, i);
    if (curr->filepath != NULL && filepath != NULL) {
      if (strcmp(filepath, curr->filepath) == 0) {
        return curr;
      }
    }
  }
  return NULL;
}

void *asset_cache_obj_get_or_create(asset_type_t ty, const char *filepath) {
  entry_t *found_entry = asset_found(filepath);
  if (found_entry == NULL) {
    entry_t *new = malloc(sizeof(entry_t));
    assert(new);
    new->filepath = strdup(filepath);
    new->type = ty;
    if (ty == ASSET_IMAGE) {
      new->obj = sdl_display(filepath);
    } else if (ty == ASSET_FONT) {
      new->obj = TTF_OpenFont(filepath, FONT_SIZE);
    }
    list_add(ASSET_CACHE, new);
    return new->obj;
  }
  assert(found_entry->type == ty);
  return found_entry->obj;
}

void asset_cache_register_button(asset_t *button) {
  assert(button);
  entry_t *new_button = malloc(sizeof(entry_t));
  assert(new_button);
  new_button->filepath = NULL;
  new_button->obj = button;
  new_button->type = ASSET_BUTTON;
  list_add(ASSET_CACHE, new_button);
}

void asset_cache_handle_buttons(state_t *state, double x, double y) {
  for (size_t i = 0; i < list_size(ASSET_CACHE); i++) {
    entry_t *button_a = (entry_t *)list_get(ASSET_CACHE, i);
    if (button_a->type == ASSET_BUTTON) {
      asset_t *button = (asset_t *)button_a->obj;
      asset_on_button_click(button, state, x, y);
    }
  }
}
