#define WALL_LIMIT 66
#define WINDOW_HEIGHT 700 // also window width, because the game is a square
#define CORRIDOR_SIZE 25
#define TILES_HEIGHT WINDOW_HEIGHT/CORRIDOR_SIZE + 1

typedef enum { right , up , left , down } Direction;

typedef struct {
    uint32_t x;
    uint32_t y;
} Point;

typedef struct {
    uint32_t x; // these are the centre of pacman
    uint32_t y;
    uint32_t size; // pacman is a square, this means width and height
    Direction direction;
} Dude;

/* don't initialize a Wall except by calling build_wall */
typedef struct {
    Point start;
    Point end;
} Wall;

typedef enum { vacant , food , blocked , special } Tile;

typedef struct {
    int16_t wall_count; // 255 walls ought to suffice
    Wall walls[WALL_LIMIT];
    Tile tiles[TILES_HEIGHT][TILES_HEIGHT]; // FIXME use a bitfield instead.
} Maze;

