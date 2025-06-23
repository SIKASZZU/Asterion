/* map */


const int map_size = 100;
const int tile_size = 100; 
extern int map[map_size][map_size];

void generate_map();
void print_map(int map[map_size][map_size]);
void generate_random_map(int map[map_size][map_size], int min_val, int max_val);
void generate_maze_runner_map(int map[map_size][map_size]);
