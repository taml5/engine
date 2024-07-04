#ifndef GAME
#define GAME
#include "game.h"
#endif

/**
 * Load the map of sectors from the given filepath.
 * 
 * @param filepath: The filepath to read the map data from.
 * @param n_sectors: The number of sectors loaded.
 * @return A pointer to a heap allocated sector array.
 */
struct sector **load_sectors(const char *filepath, int *n_sectors);

/**
 * Load the texture from the given filepath.
 * 
 * @param filepath: The filepath to read the texture data from.
 * @return A pointer to a heap allocated texture array.
 */
texture load_texture(const char *filepath);

/**
 * Load the lights in the map from the given filepath.
 * 
 * @param file
 * @param filepath: The filepath to read the light data from.
 * @param n_sectors: The number of lights loaded.
 * @return A pointer to a heap allocated light array.
 */
struct light **load_lights(const char *filepath, int *n_lights);

/**
 * Deallocate the sector array.
 * 
 * @param sectors: A pointer to the sector array.
 * @param n_sectors: The number of sectors loaded.
 */
void destroy_sectors(struct sector **sectors, const int n_sectors);

/**
 * Deallocate the texture array.
 * 
 * @param textures: A pointer to the texture array.
 * @param n_textures: The number of textures loaded.
 */
void destroy_textures(texture *textures, const int n_textures);

/**
 * Deallocate the light array.
 * 
 * @param lights: A pointer to the light array.
 * @param n_lights: The number of lights loaded.
 */
void destroy_lights(struct light **lights, const int n_lights);