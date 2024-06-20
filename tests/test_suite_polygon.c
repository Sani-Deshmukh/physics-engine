#include "list.h"
#include "polygon.h"
#include "test_util.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>

// Make square at (+/-1, +/-1)
polygon_t *make_square() {
  list_t *sq = list_init(4, free);
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t){+1, +1};
  list_add(sq, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-1, +1};
  list_add(sq, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-1, -1};
  list_add(sq, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){+1, -1};
  list_add(sq, v);

  polygon_t *poly =
      polygon_init(sq, (vector_t){0, 0}, (vector_t){0, 0}, 0, 0, 0, 0);
  return poly;
}

void test_square_area_centroid() {
  polygon_t *sq = make_square();
  assert(isclose(polygon_area(sq), 4));
  assert(vec_isclose(polygon_centroid(sq), VEC_ZERO));
  polygon_free_me(sq);
}

void test_square_translate() {
  polygon_t *sq = make_square();
  polygon_translate(sq, (vector_t){2, 3});
  assert(vec_equal(*(vector_t *)list_get(polygon_get_points(sq), 0),
                   (vector_t){3, 4}));
  assert(vec_equal(*(vector_t *)list_get(polygon_get_points(sq), 1),
                   (vector_t){1, 4}));
  assert(vec_equal(*(vector_t *)list_get(polygon_get_points(sq), 2),
                   (vector_t){1, 2}));
  assert(vec_equal(*(vector_t *)list_get(polygon_get_points(sq), 3),
                   (vector_t){3, 2}));
  assert(isclose(polygon_area(sq), 4));
  assert(vec_isclose(polygon_centroid(sq), (vector_t){2, 3}));
  polygon_free_me(sq);
}

void test_square_rotate() {
  polygon_t *sq = make_square();
  polygon_rotate(sq, 0.25 * M_PI, VEC_ZERO);
  assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(sq), 0),
                     (vector_t){0, sqrt(2)}));
  assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(sq), 1),
                     (vector_t){-sqrt(2), 0}));
  assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(sq), 2),
                     (vector_t){0, -sqrt(2)}));
  assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(sq), 3),
                     (vector_t){sqrt(2), 0}));
  assert(isclose(polygon_area(sq), 4));
  assert(vec_isclose(polygon_centroid(sq), VEC_ZERO));
  polygon_free_me(sq);
}

// Make 3-4-5 triangle
polygon_t *make_triangle() {
  list_t *tri = list_init(3, free);
  vector_t *v = malloc(sizeof(*v));
  *v = VEC_ZERO;
  list_add(tri, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){4, 0};
  list_add(tri, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){4, 3};
  list_add(tri, v);

  polygon_t *poly =
      polygon_init(tri, (vector_t){0, 0}, (vector_t){0, 0}, 0, 0, 0, 0);
  return poly;
}

void test_triangle_area_centroid() {
  polygon_t *tri = make_triangle();
  assert(isclose(polygon_area(tri), 6));
  assert(vec_isclose(polygon_centroid(tri), (vector_t){8.0 / 3.0, 1}));
  polygon_free_me(tri);
}

void test_triangle_translate() {
  polygon_t *tri = make_triangle();
  polygon_translate(tri, (vector_t){-4, -3});
  assert(vec_equal(*(vector_t *)list_get(polygon_get_points(tri), 0),
                   (vector_t){-4, -3}));
  assert(vec_equal(*(vector_t *)list_get(polygon_get_points(tri), 1),
                   (vector_t){0, -3}));
  assert(vec_equal(*(vector_t *)list_get(polygon_get_points(tri), 2),
                   (vector_t){0, 0}));
  assert(isclose(polygon_area(tri), 6));
  assert(vec_isclose(polygon_centroid(tri), (vector_t){-4.0 / 3.0, -2}));
  polygon_free_me(tri);
}

void test_triangle_rotate() {
  polygon_t *tri = make_triangle();

  // Rotate -acos(4/5) degrees around (4,3)
  polygon_rotate(tri, -acos(4.0 / 5.0), (vector_t){4, 3});
  assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(tri), 0),
                     (vector_t){-1, 3}));
  assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(tri), 1),
                     (vector_t){2.2, 0.6}));
  assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(tri), 2),
                     (vector_t){4, 3}));
  assert(isclose(polygon_area(tri), 6));
  assert(vec_isclose(polygon_centroid(tri), (vector_t){26.0 / 15.0, 2.2}));

  polygon_free_me(tri);
}

#define CIRC_NPOINTS 1000000
#define CIRC_AREA (CIRC_NPOINTS * sin(2 * M_PI / CIRC_NPOINTS) / 2)

// Circle with many points (stress test)
polygon_t *make_big_circ() {
  list_t *c = list_init(CIRC_NPOINTS, free);
  for (size_t i = 0; i < CIRC_NPOINTS; i++) {
    double angle = 2 * M_PI * i / CIRC_NPOINTS;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){cos(angle), sin(angle)};
    list_add(c, v);
  }

  polygon_t *poly =
      polygon_init(c, (vector_t){0, 0}, (vector_t){0, 0}, 0, 0, 0, 0);
  return poly;
}

void test_circ_area_centroid() {
  polygon_t *c = make_big_circ();
  assert(isclose(polygon_area(c), CIRC_AREA));
  assert(vec_isclose(polygon_centroid(c), VEC_ZERO));
  polygon_free_me(c);
}

void test_circ_translate() {
  polygon_t *c = make_big_circ();
  polygon_translate(c, (vector_t){100, 200});

  for (size_t i = 0; i < CIRC_NPOINTS; i++) {
    double angle = 2 * M_PI * i / CIRC_NPOINTS;
    assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(c), i),
                       (vector_t){100 + cos(angle), 200 + sin(angle)}));
  }
  assert(isclose(polygon_area(c), CIRC_AREA));
  assert(vec_isclose(polygon_centroid(c), (vector_t){100, 200}));

  polygon_free_me(c);
}

void test_circ_rotate() {
  // Rotate about the origin at an unusual angle
  const double ROT_ANGLE = 0.5;

  polygon_t *c = make_big_circ();
  polygon_rotate(c, ROT_ANGLE, VEC_ZERO);

  for (size_t i = 0; i < CIRC_NPOINTS; i++) {
    double angle = 2 * M_PI * i / CIRC_NPOINTS;
    assert(vec_isclose(
        *(vector_t *)list_get(polygon_get_points(c), i),
        (vector_t){cos(angle + ROT_ANGLE), sin(angle + ROT_ANGLE)}));
  }
  assert(isclose(polygon_area(c), CIRC_AREA));
  assert(vec_isclose(polygon_centroid(c), VEC_ZERO));

  polygon_free_me(c);
}

// Weird nonconvex polygon
polygon_t *make_weird() {
  list_t *w = list_init(5, free);
  vector_t *v = malloc(sizeof(*v));
  *v = VEC_ZERO;
  list_add(w, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){4, 1};
  list_add(w, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-2, 1};
  list_add(w, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-5, 5};
  list_add(w, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-1, -8};
  list_add(w, v);

  polygon_t *poly =
      polygon_init(w, (vector_t){0, 0}, (vector_t){0, 0}, 0, 0, 0, 0);
  return poly;
}

void test_weird_area_centroid() {
  polygon_t *w = make_weird();
  assert(isclose(polygon_area(w), 23));
  assert(vec_isclose(polygon_centroid(w),
                     (vector_t){-223.0 / 138.0, -51.0 / 46.0}));
  polygon_free_me(w);
}

void test_weird_translate() {
  polygon_t *w = make_weird();
  polygon_translate(w, (vector_t){-10, -20});

  assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(w), 0),
                     (vector_t){-10, -20}));
  assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(w), 1),
                     (vector_t){-6, -19}));
  assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(w), 2),
                     (vector_t){-12, -19}));
  assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(w), 3),
                     (vector_t){-15, -15}));
  assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(w), 4),
                     (vector_t){-11, -28}));
  assert(isclose(polygon_area(w), 23));
  assert(vec_isclose(polygon_centroid(w),
                     (vector_t){-1603.0 / 138.0, -971.0 / 46.0}));

  polygon_free_me(w);
}

void test_weird_rotate() {
  polygon_t *w = make_weird();
  // Rotate 90 degrees around (0, 2)
  polygon_rotate(w, M_PI / 2, (vector_t){0, 2});

  assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(w), 0),
                     (vector_t){2, 2}));
  assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(w), 1),
                     (vector_t){1, 6}));
  assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(w), 2),
                     (vector_t){1, 0}));
  assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(w), 3),
                     (vector_t){-3, -3}));
  assert(vec_isclose(*(vector_t *)list_get(polygon_get_points(w), 4),
                     (vector_t){10, 1}));
  assert(isclose(polygon_area(w), 23));
  assert(
      vec_isclose(polygon_centroid(w), (vector_t){143.0 / 46.0, 53.0 / 138.0}));

  polygon_free_me(w);
}

int main(int argc, char *argv[]) {
  // Run all tests? True if there are no command-line arguments
  bool all_tests = argc == 1;
  // Read test name from file
  char testname[100];
  if (!all_tests) {
    read_testname(argv[1], testname, sizeof(testname));
  }

  DO_TEST(test_square_area_centroid)
  DO_TEST(test_square_translate)
  DO_TEST(test_square_rotate)
  DO_TEST(test_triangle_area_centroid)
  DO_TEST(test_triangle_translate)
  DO_TEST(test_triangle_rotate)
  DO_TEST(test_circ_area_centroid)
  DO_TEST(test_circ_translate)
  DO_TEST(test_circ_rotate)
  DO_TEST(test_weird_area_centroid)
  DO_TEST(test_weird_translate)
  DO_TEST(test_weird_rotate)

  puts("polygon_test PASS");
}
