#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "forces.h"
#include "scene.h"

const size_t INITIAL_NUM_BODIES = 5;
const size_t INITIAL_NUM_FORCES = 5;

struct scene {
  size_t num_bodies;
  list_t *bodies;
  list_t *force_creators;
};

typedef struct force_instance {
  force_creator_t force_creator;
  void *aux;
  list_t *bodies;
} force_instance_t;

force_instance_t *force_instance_init(force_creator_t force_creator, void *aux,
                                      list_t *bodies) {
  force_instance_t *new = malloc(sizeof(force_instance_t));
  assert(new != NULL);
  new->force_creator = force_creator;
  new->aux = aux;
  new->bodies = bodies;
  return new;
}

void force_instance_free(force_instance_t *force_instance) {
  body_aux_free(force_instance->aux);
  list_free(force_instance->bodies);
  free(force_instance);
}

scene_t *scene_init(void) {
  scene_t *scene = malloc(sizeof(scene_t));
  assert(scene != NULL);
  scene->num_bodies = 0;
  scene->bodies = list_init(INITIAL_NUM_BODIES, (free_func_t)body_free);
  scene->force_creators =
      list_init(INITIAL_NUM_FORCES, (free_func_t)force_instance_free);
  return scene;
}

size_t scene_bodies(scene_t *screen) { return screen->num_bodies; }

void scene_add_body(scene_t *screen, body_t *body) {
  list_add(screen->bodies, body);
  screen->num_bodies++;
}
body_t *scene_get_body(scene_t *screen, size_t index) {
  assert(index < screen->num_bodies);
  return list_get(screen->bodies, index);
}

void scene_remove_body(scene_t *scene, size_t index) {
  assert(index < scene->num_bodies);
  body_t *body = list_get(scene->bodies, index);
  body_remove(body);
}

void scene_free(scene_t *screen) {
  list_free(screen->bodies);
  list_free(screen->force_creators);
  free(screen);
}

void scene_tick(scene_t *scene, double dt) {
  // calls all force creators in the scene
  for (size_t i = 0; i < list_size(scene->force_creators); i++) {
    force_instance_t *force = list_get(scene->force_creators, i);
    force->force_creator(force->aux);
  }
  for (ssize_t i = list_size(scene->bodies) - 1; i >= 0; i--) {
    body_t *body = scene_get_body(scene, i);
    if (body_is_removed(body)) {
      // iterate through scene's force creators
      for (ssize_t j = list_size(scene->force_creators) - 1; j >= 0; j--) {
        force_instance_t *force = list_get(scene->force_creators, j);
        // iterate through force creator's list of bodies
        for (ssize_t k = list_size(force->bodies) - 1; k >= 0; k--) {
          body_t *force_body = list_get(force->bodies, k);
          if (force_body == body) {
            list_remove(scene->force_creators, j);
            force_instance_free(force);
            break;
          }
        }
      }
      scene->num_bodies--;
      body_free(list_remove(scene->bodies, i));
    } else {
      body_tick(scene_get_body(scene, i), dt);
    }
  }
}

void scene_add_force_creator(scene_t *scene, force_creator_t force_creator,
                             void *aux) {
  scene_add_bodies_force_creator(scene, force_creator, aux, list_init(0, free));
}

/**
 * Adds a force creator to a scene,
 * to be invoked every time scene_tick() is called.
 * The auxiliary value is passed to the force creator each time it is called.
 * The force creator is registered with a list of bodies it applies to,
 * so it can be removed when any one of the bodies is removed.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param forcer a force creator function
 * @param aux an auxiliary value to pass to forcer when it is called
 * @param bodies the list of bodies affected by the force creator.
 *   The force creator will be removed if any of these bodies are removed.
 *   This list does not own the bodies, so its freer should be NULL.
 */
void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer,
                                    void *aux, list_t *bodies) {
  force_instance_t *new_creator = force_instance_init(forcer, aux, bodies);
  list_add(scene->force_creators, new_creator);
}
