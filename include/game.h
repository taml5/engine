#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define FUDGE (1e-6)  // fudge factor to avoid floating point errors

#define SCR_WIDTH (640)  // screen width
#define SCR_HEIGHT (480)  // screen height
#define RATIO ((float) SCR_HEIGHT / (float) SCR_WIDTH)  // the aspect ratio

#define ROTSPD (2.0f * 0.016f)  // camera rotating speed
#define MVTSPD (1.5f * 0.016f)  // movement speed
#define CAM_Z (1.70)  // the default height of the camera

#define TEX_WIDTH (128)  // texture width
#define TEX_HEIGHT (128)  // texture height
#define TEX_WIDTH_DENSITY 1  // how much of the texture width is displayed per metre
#define TEX_HEIGHT_DENSITY 1  // how much of the texture height is displayed per metre

/**
 * A struct representing a 2D float vector.
 * 
 * @param x: The x coordinate of the vector.
 * @param y: The y coordinate of the vector.
 */
struct vec2 {
    float x;
    float y;
};

/**
 * A struct representing a 2D integer vector.
 * 
 * @param x: The x coordinate of the vector.
 * @param y: The y coordinate of the vector.
 */
struct vec2i {
    int x;
    int y;
};

/**
 * A struct representing an RGB colour value.
 * 
 * @param r The red value of the colour, between 0.0 and 1.0.
 * @param g The green value of the colour, between 0.0 and 1.0.
 * @param b The blue value of the colour, between 0.0 and 1.0.
 */
struct rgb {
    float r;
    float g;
    float b;
};

typedef struct rgb **texture;

/**
 * A wall of the map.
 * 
 * @param start: The starting endpoint of the wall.
 * @param end: The ending endpoint of the wall.
 * @param portal: The sector that this portal leads to. If this is not
 *                a portal, its value will be -1.
 * @param texture_id: The id of the wall texture.
 */
struct wall {
    struct vec2i *start, *end;
    int portal;
    int texture_id;
};

/**
 * A sector of the map. This is a section of the map enclosed by walls.
 * 
 * @param id: The ID of the sector.
 * @param n_walls: The number of walls in this sector.
 * @param walls: A pointer to an array of wall pointers that make up this sector.
 * @param floor_z: The height of the floor.
 * @param ceil_z: The height of the ceiling.
 * @param floor_colour: The colour of the sector floor.
 * @param ceil_colour: The colour of the sector ceiling.
 */
struct sector {
    int id;
    size_t n_walls;
    struct wall **walls;
    float floor_z;
    float ceil_z;
    struct rgb *floor_colour;
    struct rgb *ceil_colour;
};

/**
 * A struct representing a light in the world.
 * 
 * @param pos: The position of the light in world coordinates.
 * @param intensity: The intensity of the light, between 0.0 and 1.0.
 */
struct light {
    struct vec2 *pos;
    float intensity;
};

/**
 * The camera.
 * 
 * @param pos: The current position (x, y) of the camera.
 * @param sector: The current sector that the camera is in.
 * @param angle: The camera angle, measured in radians with respect to (1, 0) on the x-y plane.
 * @param anglecos: The sine of the camera angle.
 * @param anglesin: The cosine of the camera angle.
 * 
 */
struct camera {
    struct vec2 *pos;
    int sector;
    float angle, anglecos, anglesin, height;
};

/**
 * Process the inputs of the user and perform different actions based on them.
 * 
 * @param window: A pointer to the GLFW window struct.
 * @param camera: A pointer to the camera struct.
 */
void process_input(GLFWwindow *window, 
                   struct camera *camera, 
                   struct sector **sectors,
                   struct vec2 *new);

/**
 * Update the camera's location and sector if necessary. Returns whether the camera's position
 * has changed.
 * 
 * @param camera: The camera.
 * @param sectors: The array of sectors in the map.
 * @param new: The new position of the camera to be checked.
 * @param depth: The recursive depth of the function.
 */
bool update_location(struct camera *camera,
                     struct sector * const * const sectors,
                     struct vec2 *new,
                     const int depth);