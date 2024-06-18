#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ROTSPD 3.0f * 0.016f  // camera rotating speed
#define MVTSPD 3.0f * 0.016f  // movement speed

#define MAX_SECTOR 256  // max number of sectors
#define MAX_WALL 128  // max number of walls in a sector

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
 * A wall of the map.
 * 
 * @param left: The left endpoint of the wall.
 * @param right: The right endpoint of the wall.
 * @param portal: Whether this wall is a portal or not.
 */
struct wall {
    struct vec2i left, right;
    int portal;
};

/**
 * A sector of the map. This is a section of the map enclosed by walls.
 * 
 * @param id: The ID of the sector.
 * @param walls: The starting index of the first wall in the walls array.
 * @param n_walls: The number of walls in this sector.
 * @param floor_z: The height of the floor.
 * @param ceil_z: The height of the ceiling.
 */
struct sector {
    int id;
    size_t walls, n_walls;
    float floor_z;
    float ceil_z;
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
    float angle, anglecos, anglesin;
};

/**
 * A struct to...
 */
struct game {
    struct sector sectors[MAX_SECTOR];
    struct wall walls[MAX_WALL];
};