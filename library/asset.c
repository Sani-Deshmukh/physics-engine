#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>

#include "asset.h"
#include "asset_cache.h"
#include "color.h"
#include "sdl_wrapper.h"

const Uint8 OPAQUE_ALPHA_VALUE = 255;

typedef struct asset {
  asset_type_t type;
  SDL_Rect bounding_box;
} asset_t;

typedef struct text_asset {
  asset_t base;
  TTF_Font *font;
  const char *text;
  rgb_color_t color;
} text_asset_t;

typedef struct image_asset {
  asset_t base;
  SDL_Texture *texture;
  body_t *body;
} image_asset_t;

typedef struct button_asset {
  asset_t base;
  image_asset_t *image_asset;
  text_asset_t *text_asset;
  button_handler_t handler;
  bool is_rendered;
} button_asset_t;

/**
 * Allocates memory for an asset with the given parameters.
 *
 * @param ty the type of the asset
 * @param bounding_box the bounding box containing the location and dimensions
 * of the asset when it is rendered
 * @return a pointer to the newly allocated asset
 */
static asset_t *asset_init(asset_type_t ty, SDL_Rect bounding_box) {
  asset_t *new;
  switch (ty) {
  case ASSET_IMAGE: {
    new = malloc(sizeof(image_asset_t));
    break;
  }
  case ASSET_FONT: {
    new = malloc(sizeof(text_asset_t));
    break;
  }
  case ASSET_BUTTON: {
    new = malloc(sizeof(button_asset_t));
    break;
  }
  default: {
    assert(false && "Unknown asset type");
  }
  }
  assert(new);
  new->type = ty;
  new->bounding_box = bounding_box;
  return new;
}

asset_type_t asset_get_type(asset_t *asset) { return asset->type; }

asset_t *asset_make_image(const char *filepath, SDL_Rect bounding_box) {
  asset_t *img = asset_make_image_with_body(filepath, bounding_box, NULL);
  return img;
}

asset_t *asset_make_image_with_body(const char *filepath, SDL_Rect bounding,
                                    body_t *body) {
  image_asset_t *img = malloc(sizeof(image_asset_t));
  assert(img);
  img->base = *asset_init(ASSET_IMAGE, bounding);
  img->texture = asset_cache_obj_get_or_create(ASSET_IMAGE, filepath);
  img->body = body;
  return (asset_t *)img;
}

asset_t *asset_make_text(const char *filepath, SDL_Rect bounding_box,
                         const char *text, rgb_color_t color) {
  text_asset_t *t = malloc(sizeof(text_asset_t));
  assert(t);
  t->base = *asset_init(ASSET_FONT, bounding_box);
  t->font = asset_cache_obj_get_or_create(ASSET_FONT, filepath);
  t->text = text;
  t->color = color;
  return (asset_t *)t;
}

asset_t *asset_make_button(SDL_Rect bounding_box, asset_t *image_asset,
                           asset_t *text_asset, button_handler_t handler) {
  assert(image_asset == NULL || image_asset->type == ASSET_IMAGE);
  assert(text_asset == NULL || text_asset->type == ASSET_FONT);
  button_asset_t *button = malloc(sizeof(text_asset_t));
  assert(button);
  button->base = *asset_init(ASSET_BUTTON, bounding_box);
  button->image_asset = (image_asset_t *)image_asset;
  button->text_asset = (text_asset_t *)text_asset;
  button->handler = handler;

  return (asset_t *)button;
}

/**
 * Checks if the given x, y coordinate is within the bounding box of the asset
 * @param button the button asset which we are checking for the click
 * @param x x coordinate of click
 * @param y y coordinate of click
 * @return true if click is within bounding box
 * @return false if click is not within bounding box
 */
static bool click_in_button(asset_t *button, double x, double y) {
  SDL_Rect box = button->bounding_box;
  if (x < (box.x + box.w) && x > (box.x) && y < (box.y + box.h) &&
      y > (box.y)) {
    return true;
  }
  return false;
}

void asset_on_button_click(asset_t *button, state_t *state, double x,
                           double y) {
  button_asset_t *button_asset = (button_asset_t *)button;
  if (button_asset->is_rendered) {
    if (click_in_button(button, x, y) && button_asset->is_rendered) {
      button_asset->handler(state);
      button_asset->is_rendered = false;
    }
  }
  return;
}

void asset_render(asset_t *asset) {
  switch (asset->type) {
  case ASSET_IMAGE: {
    image_asset_t *img = (image_asset_t *)asset;
    if (img->body != NULL) {
      asset->bounding_box = *sdl_get_bounding_box(img->body);
    }
    sdl_render_image(img->texture, asset->bounding_box.w, asset->bounding_box.h,
                     asset->bounding_box.x, asset->bounding_box.y);
    break;
  }
  case ASSET_FONT: {
    text_asset_t *text = (text_asset_t *)asset;
    SDL_Color c = (SDL_Color){.r = text->color.r,
                              .g = text->color.g,
                              .b = text->color.b,
                              .a = OPAQUE_ALPHA_VALUE};
    sdl_render_text(text->text, text->font,
                    (vector_t){.x = text->base.bounding_box.x,
                               .y = text->base.bounding_box.y},
                    c);
    break;
  }
  case ASSET_BUTTON: {
    button_asset_t *button = (button_asset_t *)asset;
    if (button->image_asset != NULL) {
      asset_render((asset_t *)button->image_asset);
    }
    if (button->text_asset != NULL) {
      asset_render((asset_t *)button->text_asset);
    }
    button->is_rendered = true;
    break;
  }
  default: {
    assert(false && "Invalid Asset Type");
  }
  }
}

void asset_destroy(asset_t *asset) { free(asset); }
