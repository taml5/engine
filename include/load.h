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
 * Deallocate the sector array.
 * 
 * @param sectors: A pointer to the sector array.
 * @param n_sectors: The number of sectors loaded.
 */
void destroy_sectors(struct sector **sectors, int *n_sectors);