#ifndef __SDL_WRAPPER_H__
#define __SDL_WRAPPER_H__

#include "color.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "state.h"
#include "vector.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

// Values passed to a key handler when the given arrow key is pressed
typedef enum {
  LEFT_ARROW = 1,
  UP_ARROW = 2,
  RIGHT_ARROW = 3,
  DOWN_ARROW = 4,
  SPACE_BAR = 5,
} arrow_key_t;

/**
 * The possible types of key events.
 * Enum types in C are much more primitive than in Java; this is equivalent to:
 * typedef unsigned int KeyEventType;
 * #define KEY_PRESSED 0
 * #define KEY_RELEASED 1
 */
typedef enum { KEY_PRESSED, KEY_RELEASED } key_event_type_t;

typedef enum { MOUSE_PRESSED, MOUSE_RELEASED } mouse_event_type_t;

/**
 * Computes the center of the window in pixel coordinates.
 *
 * @return A vector representing the center of the window.
 */
vector_t get_window_center(void);
/**
 * Maps a scene coordinate to a window coordinate.
 *
 * @param scene_pos The position in scene coordinates.
 * @param window_center The center of the window in pixel coordinates.
 * @return A vector representing the mapped position in window coordinates.
 */
vector_t get_window_position(vector_t scene_pos, vector_t window_center);

/**
 * A keypress handler.
 * When a key is pressed or released, the handler is passed its char value.
 * Most keys are passed as their char value, e.g. 'a', '1', or '\r'.
 * Arrow keys have the special values listed above.
 *
 * @param key a character indicating which key was pressed
 * @param type the type of key event (KEY_PRESSED or KEY_RELEASED)
 * @param held_time if a press event, the time the key has been held in seconds
 */
typedef void (*key_handler_t)(char key, key_event_type_t type, double held_time,
                              void *state);

/**
 * A mousepress handler.
 * When a mouse is pressed or released, the handler is passed a mouse_event
 * (either pressed or unpressed), the x and y locations of the press,
 * and the state.
 * @param type the type of mouse event (MOUSE_PRESSED or MOUSE_RELEASED)
 * @param x_loc the x-coordinate of where the mouse was pressed
 * @param y_loc the y-coordinate of where the mouse was pressed
 * @param state the state that the mouse event is applied to
 */
typedef void (*mouse_handler_t)(void *state, double x_loc, double y_loc);

/**
 * Initializes the SDL window and renderer.
 * Must be called once before any of the other SDL functions.
 *
 * @param min the x and y coordinates of the bottom left of the scene
 * @param max the x and y coordinates of the top right of the scene
 */
void sdl_init(vector_t min, vector_t max);
/**
 * Displays the image that is passed in as the img parameter
 * The size of the image is defined by the img_width and img_height parameters
 * The location of the image is defined by the img_center_y and img_center_y
 * parameters
 */
void sdl_render_image(SDL_Texture *img, size_t img_width, size_t img_height,
                      size_t img_center_x, size_t image_center_y);
/**
 * Creates the texture of an image using the given stringPath that point to the
 * image file
 */
SDL_Texture *sdl_display(const char *stringPath);

/**
 * Gets the bounding box for the body, the smallest box
 * that will cover the body entirely
 * @param body body that the bounding box is calculated for
 * @return SDL_Rect of the bounding box for the body
 */
SDL_Rect *sdl_get_bounding_box(body_t *body);

/**
 * Displays the text that is passed in as the txt parameter
 * @param txt the string that is printed
 * @param font the font that the string is printed with
 * @param position a vector describing the position of the text when displayed
 */
void sdl_render_text(const char *txt, TTF_Font *font, const vector_t position,
                     SDL_Color color);

/**
 * Processes all SDL events and returns whether the window has been closed.
 * This function must be called in order to handle inputs.
 *
 * @return true if the window was closed, false otherwise
 */
bool sdl_is_done(void *state);

/**
 * Clears the screen. Should be called before drawing polygons in each frame.
 */
void sdl_clear(void);

/**
 * Draws a polygon from the given list of vertices and a color.
 *
 * @param poly a struct representing the polygon
 * @param color the color used to fill in the polygon
 */
void sdl_draw_polygon(polygon_t *poly, rgb_color_t color);

/**
 * Displays the rendered frame on the SDL window.
 * Must be called after drawing the polygons in order to show them.
 */
void sdl_show(void);

/**
 * Draws all bodies in a scene.
 * This internally calls sdl_clear(), sdl_draw_polygon(), and sdl_show(),
 * so those functions should not be called directly.
 *
 * @param scene the scene to draw
 * @param aux an additional body to draw (can be NULL if no additional bodies)
 */
void sdl_render_scene(scene_t *scene, void *aux);

/**
 * Registers a function to be called every time a key is pressed.
 * Overwrites any existing handler.
 *
 * Example:
 * ```
 * void on_key(char key, key_event_type_t type, double held_time) {
 *     if (type == KEY_PRESSED) {
 *         switch (key) {
 *             case 'a':
 *                 printf("A pressed\n");
 *                 break;
 *             case UP_ARROW:
 *                 printf("UP pressed\n");
 *                 break;
 *         }
 *     }
 * }
 * int main(void) {
 *     sdl_on_key(on_key);
 *     while (!sdl_is_done());
 * }
 * ```
 *
 * @param handler the function to call with each key press
 */
void sdl_on_key(key_handler_t handler);

void sdl_on_mouse(mouse_handler_t handler);

/**
 * Gets the amount of time that has passed since the last time
 * this function was called, in seconds.
 *
 * @return the number of seconds that have elapsed
 */
double time_since_last_tick(void);

#endif // #ifndef __SDL_WRAPPER_H__
