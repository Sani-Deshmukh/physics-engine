#include "vector.h"
#include <math.h>
#include <stdlib.h>

const vector_t VEC_ZERO = (vector_t){.x = 0.0, .y = 0.0};

vector_t vec_add(vector_t v1, vector_t v2) {
  vector_t sum = (vector_t){.x = v1.x + v2.x, .y = v1.y + v2.y};
  return sum;
}

vector_t vec_subtract(vector_t v1, vector_t v2) {
  vector_t sub = (vector_t){.x = v1.x - v2.x, .y = v1.y - v2.y};
  return sub;
}

vector_t vec_negate(vector_t v) {
  vector_t neg = (vector_t){.x = v.x * -1, .y = v.y * -1};
  return neg;
}

vector_t vec_multiply(double scalar, vector_t v) {
  vector_t mul = (vector_t){.x = v.x * scalar, .y = v.y * scalar};
  return mul;
}

double vec_dot(vector_t v1, vector_t v2) {
  double dot = (v1.x * v2.x) + (v1.y * v2.y);
  return dot;
}

double vec_cross(vector_t v1, vector_t v2) {
  double cross = (v1.x * v2.y) - (v1.y * v2.x);
  return cross;
}

vector_t vec_rotate(vector_t v, double angle) {
  double xr = (v.x * cos(angle)) - (v.y * sin(angle));
  double yr = (v.x * sin(angle)) + (v.y * cos(angle));
  vector_t rot = (vector_t){.x = xr, .y = yr};
  return rot;
}

double vec_get_length(vector_t v) { return sqrt(pow(v.x, 2) + pow(v.y, 2)); }
