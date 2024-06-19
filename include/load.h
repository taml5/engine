#ifndef GAME
#define GAME
#include "game.h"
#endif

/**
 * Load the map of sectors from the given filepath.
 * 
 * @param filepath: The filepath to read the map data from.
 * @param n_sectors: The number of sectors loaded.
 * @return A pointer to a heap allocated 
 * 
 */
struct sector *load_sectors(char *filepath, int *n_sectors);

/**
 * Deallocate the sector array.
 * 
 * @param sectors: A pointer to the sector array.
 */
void destroy_sectors(struct sector *sectors);