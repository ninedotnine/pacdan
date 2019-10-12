#define WALL_LIMIT 66
#define WINDOW_HEIGHT 700 // also window width, because the game is a square
#define CORRIDOR_SIZE 25
#define TILES_HEIGHT WINDOW_HEIGHT/CORRIDOR_SIZE + 1

#define gcc_pure __attribute__((pure))

enum direction { right , up , left , down };

struct directions {
    bool right;
    bool up;
    bool left;
    bool down;
};

struct point {
    int x;
    int y;
};

struct dude {
    int x; // these are the centre of pacman
    int y;
    unsigned size; // pacman is a square, this means width and height
    enum direction direction;
    GC gc;
};

/* don't initialize a Wall except by calling build_wall */
struct wall {
    struct point start;
    struct point end;
};

enum tile { vacant , food , blocked , special };

struct maze {
    struct wall walls[WALL_LIMIT];
    enum tile tiles[TILES_HEIGHT][TILES_HEIGHT]; // FIXME use a bitfield instead.
    uint16_t food_count;
    uint16_t wall_count;
};

struct controls_thread_data {
    Display* const dpy;
    const Window win;
    bool game_over;
    bool paused;
    struct directions * dirs;
};
