#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/random.h>
#include <linux/slab.h>

#define MAX_ROWS 12
#define MAX_COLS 15
#define MAX_FRONTIER 1000  // Maximum number of frontier cells we can store
#define PROC_NAME "maze"
#define BUF_LEN 2048 // Buffer length for the maze

typedef struct {
    int x, y;
} Cell;

static char *maze_buffer;
static struct proc_dir_entry *maze_proc;

static void print_grid(char grid[][MAX_COLS], int rows, int cols) {
    int pos = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (pos < BUF_LEN - 1) {
                maze_buffer[pos++] = grid[i][j];
            }
        }
        if (pos < BUF_LEN - 1) {
            maze_buffer[pos++] = '\n';
        }
    }
    maze_buffer[pos] = '\0';
}

static void make_grid(char grid[][MAX_COLS], int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j] = '#';
        }
    }
}

static int is_within_bounds(int x, int y, int rows, int cols) {
    return (x >= 0 && x < rows && y >= 0 && y < cols);
}

static void random_cell(char grid[][MAX_COLS], int rows, int cols, int *x, int *y) {
    int random_value;
    get_random_bytes(&random_value, sizeof(random_value));
    *x = abs(random_value) % rows;  // Random row
    get_random_bytes(&random_value, sizeof(random_value));
    *y = abs(random_value) % cols;  // Random column
    grid[*x][*y] = ' ';  // Set random cell as passage
}

static void add_frontier_cells(char grid[][MAX_COLS], Cell frontier[], int *frontier_count, int x, int y, int rows, int cols) {
    int directions[4][2] = { {-2, 0}, {2, 0}, {0, -2}, {0, 2} };

    for (int i = 0; i < 4; i++) {
        int nx = x + directions[i][0];
        int ny = y + directions[i][1];

        if (is_within_bounds(nx, ny, rows, cols) && grid[nx][ny] == '#') {
            frontier[*frontier_count].x = nx;
            frontier[*frontier_count].y = ny;
            (*frontier_count)++;
        }
    }
}

static void connect_to_passage(char grid[][MAX_COLS], int x, int y, int rows, int cols) {
    int directions[4][2] = { {-2, 0}, {2, 0}, {0, -2}, {0, 2} };

    for (int i = 0; i < 4; i++) {
        int nx = x + directions[i][0];
        int ny = y + directions[i][1];

        if (is_within_bounds(nx, ny, rows, cols) && grid[nx][ny] == ' ') {
            int mid_x = (x + nx) / 2;
            int mid_y = (y + ny) / 2;
            grid[mid_x][mid_y] = ' ';
            grid[x][y] = ' ';
            break;
        }
    }
}

static void generate_maze(void) {
    int rows = MAX_ROWS;
    int cols = MAX_COLS;
    char grid[MAX_ROWS][MAX_COLS];
    Cell frontier[MAX_FRONTIER];
    int frontier_count = 0;

    make_grid(grid, rows, cols);

    int start_x, start_y;
    random_cell(grid, rows, cols, &start_x, &start_y);

    add_frontier_cells(grid, frontier, &frontier_count, start_x, start_y, rows, cols);

    while (frontier_count > 0) {
        int random_value;
        get_random_bytes(&random_value, sizeof(random_value));
        int rand_index = abs(random_value) % frontier_count;
        int fx = frontier[rand_index].x;
        int fy = frontier[rand_index].y;

        connect_to_passage(grid, fx, fy, rows, cols);

        add_frontier_cells(grid, frontier, &frontier_count, fx, fy, rows, cols);

        frontier[rand_index] = frontier[--frontier_count];
    }

    print_grid(grid, rows, cols);
}

// Proc read function
ssize_t maze_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    return simple_read_from_buffer(buf, count, pos, maze_buffer, strlen(maze_buffer));
}

static const struct proc_ops proc_file_ops = {
    .proc_read = maze_read,
};

// Init function
static int __init maze_init(void) {
    maze_buffer = kmalloc(BUF_LEN, GFP_KERNEL);
    if (!maze_buffer) return -ENOMEM;

    maze_proc = proc_create(PROC_NAME, 0444, NULL, &proc_file_ops);
    if (!maze_proc) {
        kfree(maze_buffer);
        return -ENOMEM;
    }

    generate_maze();
    printk(KERN_INFO "Maze module loaded.\n");
    return 0;
}

// Exit function
static void __exit maze_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    kfree(maze_buffer);
    printk(KERN_INFO "Maze module unloaded.\n");
}

module_init(maze_init);
module_exit(maze_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jaleel Rogers");
MODULE_DESCRIPTION("Maze Generator using Prim's Algorithm");

