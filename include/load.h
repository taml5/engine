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
 * 
 */
struct sector **load_sectors(const char *filepath, int *n_sectors);

/**
 * Load the texture array from the given filepath.
 * 
 * @param filepath: The filepath to read the texture data from.
 * @param n_textures: The number of textures loaded.
 * @return A pointer to a heap allocated texture array.
 */
texture *load_textures(const char *filepath, int *n_textures);

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