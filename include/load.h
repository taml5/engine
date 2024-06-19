#ifndef GAME
#define GAME
#include "game.h"
#endif

/**
 * Load the map of sectors from the given filepath.
 * 
 * @param filepath: The filepath to read the map data from.
 */
struct game *load_sectors(char *filepath);

/**
 * Deallocate the given game struct.
 * 
 * @param game: The game struct.
 */
void destroy_game(struct game *game);