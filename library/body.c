#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "body.h"

struct body {
  polygon_t *poly;

  double mass;

  vector_t force;
  vector_t impulse;
  bool removed;

  void *info;
  free_func_t info_freer;
};

void body_reset(body_t *body) {
  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;
}
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "body.h"

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
  body_t *body = body_init_with_info(shape, mass, color, NULL, NULL);
  return body;
}

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
                            void *info, free_func_t info_freer) {
  body_t *body = malloc(sizeof(body_t));
  assert(body);
  body->poly = polygon_init(shape, VEC_ZERO, 0, color.r, color.g, color.b);
  body->mass = mass;
  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;
  body->removed = false;
  body->info = info;
  body->info_freer = info_freer;
  return body;
}

polygon_t *body_get_polygon(body_t *body) { return body->poly; }

void *body_get_info(body_t *body) { return body->info; }

void body_free(body_t *body) {
  if (body->info_freer != NULL) {
    body->info_freer(body->info);
  }
  polygon_free(body->poly);
  free(body);
}

list_t *body_get_shape(body_t *body) {
  size_t len = list_size(polygon_get_points(body->poly));
  list_t *copy = list_init(len, free);
  for (size_t i = 0; i < len; i++) {
    vector_t *oldVec = list_get(polygon_get_points(body->poly), i);
    vector_t *new = malloc(sizeof(vector_t));
    assert(new);
    new->x = oldVec->x;
    new->y = oldVec->y;
    list_add(copy, new);
  }
  return copy;
}

vector_t body_get_centroid(body_t *body) {
  return polygon_get_center(body->poly);
}

vector_t body_get_velocity(body_t *body) {
  vector_t v = polygon_get_velocity(body->poly);
  return v;
}

rgb_color_t *body_get_color(body_t *body) {
  return polygon_get_color(body->poly);
}

void body_set_color(body_t *body, rgb_color_t *col) {
  polygon_set_color(body->poly, col);
}

void body_set_centroid(body_t *body, vector_t x) {
  polygon_set_center(body->poly, x);
}

void body_set_velocity(body_t *body, vector_t v) {
  polygon_set_velocity(body->poly, v);
}

double body_get_rotation(body_t *body) {
  return polygon_get_rotation(body->poly);
}

void body_set_rotation(body_t *body, double angle) {
  polygon_set_rotation(body->poly, angle);
}

void body_tick(body_t *body, double dt) {
  vector_t old_velocity = body_get_velocity(body);
  vector_t impulse_vel = vec_multiply((1.0 / body->mass), body->impulse);
  vector_t force_vel = vec_multiply(dt / body->mass, body->force);
  vector_t add_factor = vec_add(force_vel, impulse_vel);

  vector_t new_velocity = vec_add(add_factor, old_velocity);

  vector_t sum = vec_add(old_velocity, new_velocity);

  body_set_velocity(body, vec_multiply(0.5, sum));

  polygon_move(body->poly, dt);

  body_set_velocity(body, new_velocity);
  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;
}

double body_get_mass(body_t *body) { return body->mass; }

void body_add_force(body_t *body, vector_t force) {
  double new_x = body->force.x + force.x;
  double new_y = body->force.y + force.y;
  body->force.x = new_x;
  body->force.y = new_y;
}

void body_add_impulse(body_t *body, vector_t impulse) {
  double new_x = body->impulse.x + impulse.x;
  double new_y = body->impulse.y + impulse.y;
  body->impulse.x = new_x;
  body->impulse.y = new_y;
}

void body_remove(body_t *body) { body->removed = true; }

bool body_is_removed(body_t *body) { return body->removed; }
