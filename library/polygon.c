#include "polygon.h"
#include "color.h"
#include "list.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

typedef struct polygon {
  list_t *points;
  vector_t velocity;
  double rotation_speed;
  rgb_color_t *color;
  double angle;
} polygon_t;

polygon_t *polygon_init(list_t *points, vector_t initial_velocity,
                        double rotation_speed, double red, double green,
                        double blue) {
  polygon_t *polygon = malloc(sizeof(polygon_t));
  assert(polygon != NULL);
  polygon->points = points;
  polygon->velocity = initial_velocity;
  polygon->rotation_speed = rotation_speed;
  polygon->angle = 0.0;
  polygon->color = color_init(red, green, blue);
  return polygon;
}

list_t *polygon_get_points(polygon_t *polygon) {
  list_t *vertices = polygon->points;
  return vertices;
}

void polygon_move(polygon_t *polygon, double time_elapsed) {
  vector_t translate = vec_multiply(time_elapsed, polygon->velocity);
  polygon_translate(polygon, translate);
  vector_t centroid = polygon_centroid(polygon);
  double angle_change = polygon->rotation_speed * time_elapsed;
  polygon_rotate(polygon, angle_change, centroid);
}

void polygon_set_velocity(polygon_t *polygon, vector_t vel) {
  polygon->velocity = vel;
}

vector_t polygon_get_velocity(polygon_t *polygon) { return polygon->velocity; }

void polygon_free(polygon_t *polygon) {
  list_free(polygon->points);
  color_free(polygon->color);
  free(polygon);
}

double polygon_get_velocity_x(polygon_t *polygon) {
  return polygon->velocity.x;
}

double polygon_get_velocity_y(polygon_t *polygon) {
  return polygon->velocity.y;
}

double polygon_area(polygon_t *polygon) {
  size_t len = list_size(polygon->points);
  double term1 = 0;

  for (size_t i = 0; i < len; i++) {
    vector_t *point_i = (vector_t *)list_get(polygon->points, i);
    vector_t *point_i1 = (vector_t *)list_get(polygon->points, (i + 1) % len);

    double xi = point_i->x;
    double xi1 = point_i1->x;
    double yi = point_i->y;
    double yi1 = point_i1->y;

    term1 += (xi1 + xi) * (yi1 - yi);
  }

  double out = 0.5 * fabs(term1);
  return out;
}

vector_t polygon_centroid(polygon_t *polygon) {
  size_t len = list_size(polygon->points);
  double cx = 0;
  double cy = 0;
  for (size_t i = 0; i < len; i++) {
    vector_t *point_i = (vector_t *)list_get(polygon->points, i);
    vector_t *point_i1 = (vector_t *)list_get(polygon->points, (i + 1) % len);
    double xi = point_i->x;
    double xi1 = point_i1->x;
    double yi = point_i->y;
    double yi1 = point_i1->y;

    double xterm = (xi + xi1) * ((xi * yi1) - (xi1 * yi));
    cx += xterm;

    double yterm = (yi + yi1) * ((xi * yi1) - (xi1 * yi));
    cy += yterm;
  }
  double constant = 1 / (6 * polygon_area(polygon));
  vector_t out = (vector_t){.x = constant * cx, .y = constant * cy};
  return out;
}

void polygon_translate(polygon_t *polygon, vector_t translation) {
  size_t len = list_size(polygon->points);
  for (size_t i = 0; i < len; i++) {
    vector_t *point = list_get(polygon->points, i);
    point->x += translation.x;
    point->y += translation.y;
  }
}

void polygon_rotate(polygon_t *polygon, double angle, vector_t point) {
  polygon_translate(polygon, vec_subtract(VEC_ZERO, point));
  for (size_t i = 0; i < list_size(polygon->points); i++) {
    // pointer to modify teh existing value
    vector_t *p = list_get(polygon->points, i);
    vector_t rot = vec_rotate(*p, angle);
    p->x = rot.x;
    p->y = rot.y;
  }
  polygon_translate(polygon, vec_subtract(point, VEC_ZERO));
}

rgb_color_t *polygon_get_color(polygon_t *polygon) { return polygon->color; }

void polygon_set_color(polygon_t *polygon, rgb_color_t *color) {
  polygon->color = color;
}

void polygon_set_center(polygon_t *polygon, vector_t centroid) {
  // move the centroid to the new location
  polygon_translate(polygon, vec_subtract(centroid, polygon_centroid(polygon)));
  ;
}

vector_t polygon_get_center(polygon_t *polygon) {
  return polygon_centroid(polygon);
}

void polygon_set_rotation(polygon_t *polygon, double rot) {
  double toRotate = rot - polygon->angle;
  // rotating polygon to that angle
  polygon_rotate(polygon, toRotate, polygon_centroid(polygon));
}

double polygon_get_rotation(polygon_t *polygon) { return polygon->angle; }
