/***
 * @author: Karel Norek, xnorek01@stud.fit.vutbr.cz
 * @date 12.12.2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef struct {
    int rows;
    int cols;
    unsigned char *cells;
} Map;

/**
 * Define borders as their bit representation and paths for right and left hand rule
 */
enum borders {
    LEFT = (unsigned) 1, RIGHT = (unsigned) 2, TOP_BOTTOM = (unsigned) 4, LPATH = 45, RPATH = 55
};

/**
 * Function calculates if cell have border or not.
 * @param map - struct for maze
 * @param r - actual row
 * @param c - actual col
 * @param border - actual border
 * @return function return true if border is actually a border and false if it is not a border
 */
bool isborder(Map *map, int r, int c, int border) {
    return (map->cells[(r - 1) * map->cols + (c - 1)] & border) == border;
}

/**
 * Function is going to return which border follows next in left hand rule.
 * @param r - actual row
 * @param c - actual col
 * @param border - actual border
 * @return function returns which border is going to be next
 */
int border_left(int r, int c, int border) {
    if ((r + c) % 2 != 0) {
        if (border == LEFT) {
            return RIGHT;
        } else if (border == RIGHT) {
            return TOP_BOTTOM;
        } else if (border == TOP_BOTTOM) {
            return LEFT;
        }
    } else if ((r + c) % 2 == 0) {
        if (border == LEFT) {
            return TOP_BOTTOM;
        } else if (border == TOP_BOTTOM) {
            return RIGHT;
        } else if (border == RIGHT) {
            return LEFT;
        }
    }
    return 0;
}

/**
 * Function is going to return which border follows next in right hand rule.
 * @param r - actual row
 * @param c - actual col
 * @param border - actual border
 * @return function returns which border is going to be next
 */
int border_right(int r, int c, int border) {
    if ((r + c) % 2 != 0) {
        if (border == LEFT) {
            return TOP_BOTTOM;
        } else if (border == RIGHT) {
            return LEFT;
        } else if (border == TOP_BOTTOM) {
            return RIGHT;
        }
    } else if ((r + c) % 2 == 0) {
        if (border == LEFT) {
            return RIGHT;
        } else if (border == TOP_BOTTOM) {
            return LEFT;
        } else if (border == RIGHT) {
            return TOP_BOTTOM;
        }
    }
    return 0;
}

/**
 * Function switches between path an returns border according to it.
 * @param path - which hand rule is going to be followed
 * @param border - starting border
 * @param r - starting row
 * @param c - starting col
 * @return function return starting border from function border_left, border_right or EXIT_FAILURE on failure
 */
int path_switch(int path, int border, int r, int c) {
    if (path == LPATH) {
        return border_left(r, c, border);
    } else if (path == RPATH) {
        return border_right(r, c, border);
    }
    return -1;
}

/**
 * Function finds starting border of maze in corner when possible. If not possible returns EXIT_FAILURE.
 * @param map - struct where maze is stored
 * @param r - starting row
 * @param c - starting col
 * @param path - which hand rule is going to be followed
 * @return function returns starting border from function path_switch or EXIT_FAILURE on failure
 */
int corner_border(Map *map, int r, int c, int path) {
    if (c == 1) {
        if (!isborder(map, r, c, LEFT)) {
            return path_switch(path, LEFT, r, c);
        } else if ((r % 2 == 0 || r == 1) && !isborder(map, r, c, TOP_BOTTOM)) {
            return path_switch(path, TOP_BOTTOM, r, c);
        }
    } else if (c == map->cols) {
        if (!isborder(map, r, c, RIGHT)) {
            return path_switch(path, RIGHT, r, c);
        } else if ((r % 2 == 0 || r == 1) && !isborder(map, r, c, TOP_BOTTOM)) {
            return path_switch(path, TOP_BOTTOM, r, c);
        }
    }
    return -1;
}

/**
 * Function finds starting border of maze when possible. If not possible returns EXIT_FAILURE.
 * @param map - struct where maze is stored
 * @param border - parameter that is going to be returned
 * @param r - starting row
 * @param c - starting col
 * @param path - which hand rule is going to be followed
 * @return function returns starting border from function path_switch or EXIT_FAILURE on failure
 */
int start_border_return(Map *map, int border, int r, int c, int path) {
    if (!isborder(map, r, c, border)) {
        return path_switch(path, border, r, c);
    }
    return -1;
}

/**
 * Function finds starting border of maze when possible. If not possible returns EXIT_FAILURE.
 * @param map - struct where maze is stored
 * @param r - starting row
 * @param c - starting col
 * @param path - which hand rule is going to be followed
 * @return function returns starting border or EXIT_FAILURE on failure
 */
int start_border(Map *map, int r, int c, int path) {
    if (r == 1) {
        if (c == 1) {
            return corner_border(map, r, c, path);
        } else if (c == map->cols) {
            return corner_border(map, r, c, path);
        } else if (c % 2 != 0) {
            return start_border_return(map, TOP_BOTTOM, r, c, path);
        }

    } else if (r == map->rows) {
        if (c == 1) {
            return corner_border(map, r, c, path);
        } else if (c == map->cols) {
            if (c % 2 == 0) {
                return corner_border(map, r, c, path);
            } else return start_border_return(map, RIGHT, r, c, path);
        } else if ((r % 2 == 0 && c % 2 != 0) || (r % 2 != 0 && c % 2 == 0)) {
            return start_border_return(map, TOP_BOTTOM, r, c, path);
        }

    } else if (r != map->rows && (c == 1)) {
        return start_border_return(map, LEFT, r, c, path);

    } else if (c == map->cols && r != map->rows) {
        return start_border_return(map, RIGHT, r, c, path);
    }
    return -1;
}

/**
 * Function follow left hand rule when looking for a way out of maze.
 * @param map - which struct (maze) is program going to navigate through
 * @param border - starting border
 * @param r - starting row
 * @param c - starting col
 */
void lpath(Map *map, int border, int r, int c) {
    while (r > 0 && r <= map->rows && c > 0 && c <= map->cols) {
        if (!isborder(map, r, c, border)) {
            printf("%d,%d\n", r, c);
            if ((r + c) % 2 == 0) {
                if (border == TOP_BOTTOM) {
                    r--;
                    border = LEFT;
                } else if (border == LEFT) {
                    c--;
                    border = TOP_BOTTOM;
                } else if (border == RIGHT) {
                    c++;
                    border = RIGHT;
                }
            } else if ((r + c) % 2 != 0) {
                if (border == TOP_BOTTOM) {
                    r++;
                    border = RIGHT;
                } else if (border == LEFT) {
                    c--;
                    border = LEFT;
                } else if (border == RIGHT) {
                    c++;
                    border = TOP_BOTTOM;
                }
            }
        } else border = border_left(r, c, border);
    }
}

/**
 * Function follows right hand rule when looking for a way out of maze.
 * @param map - which struct (maze) is program going to navigate through
 * @param border - starting border
 * @param r - starting rows
 * @param c - starting col
 */
void rpath(Map *map, int border, int r, int c) {
    while (r > 0 && r <= map->rows && c > 0 && c <= map->cols) {
        if (!isborder(map, r, c, border)) {
            printf("%d,%d\n", r, c);
            if ((r + c) % 2 == 0) {
                if (border == TOP_BOTTOM) {
                    r--;
                    border = RIGHT;
                } else if (border == LEFT) {
                    c--;
                    border = LEFT;
                } else if (border == RIGHT) {
                    c++;
                    border = TOP_BOTTOM;
                }
            } else if ((r + c) % 2 != 0) {
                if (border == TOP_BOTTOM) {
                    r++;
                    border = LEFT;
                } else if (border == LEFT) {
                    c--;
                    border = TOP_BOTTOM;
                } else if (border == RIGHT) {
                    c++;
                    border = RIGHT;
                }
            }
        } else border = border_right(r, c, border);
    }
}

/**
 * Function chooses which path should program follow.
 * @param argv - argument which determine what path is going to be followed
 * @param map - struct that is going to be used as maze
 * @param arg_rows - starting row in maze (must be on outer layer of maze)
 * @param arg_cols - starting col in maze (must be on outer layer of maze)
 * @return function returns success or failure depending on the result
 */
int which_path(char *argv[], Map *map, int arg_rows, int arg_cols) {
    if (!strcmp(argv[1], "--lpath")) {
        int border = start_border(map, arg_rows, arg_cols, LPATH);
        if (border != -1) {
            lpath(map, border, arg_rows, arg_cols);
        } else return -1;

    } else if (!strcmp(argv[1], "--rpath")) {
        int border = start_border(map, arg_rows, arg_cols, RPATH);
        if (border != -1) {
            rpath(map, border, arg_rows, arg_cols);
        } else return -1;

    } else if (!strcmp(argv[1], "--shortest")) {
        printf("SadCat\n");
    }
    return EXIT_SUCCESS;
}

/**
 * Function initializes struct (allocates memory and writes rows and cols into struct).
 * @param map - struct to be initialized
 * @param rows - number of rows in maze
 * @param cols - number of cols in maze
 */
int map_ctor(Map *map, int rows, int cols) {
    map->cells = malloc(sizeof(unsigned char) * rows * cols);
    if (map->cells == NULL){
        printf("couldn't allocate memory \n");
        return EXIT_FAILURE;
    }
    map->rows = rows;
    map->cols = cols;
    return EXIT_SUCCESS;
}

/**
 * Function checks if struct map contains valid maze.
 * @param map - struct in which the maze is located
 * @return function return if maze is valid or not
 */
int validation(Map *map) {
    for (int i = 0; i < map->rows; i++) {
        for (int j = 0; j < map->cols; j++) {
            if ((map->cells[i * map->cols + j] & RIGHT) == RIGHT) {
                if (j + 1 == map->cols) {
                    break;
                } else if ((map->cells[i * map->cols + j + 1] & LEFT) != LEFT) {
                    return -1;
                }
            }
            if (((i + 1) + (j + 1)) % 2 != 0) {
                if ((map->cells[i * map->cols + j] & TOP_BOTTOM) == TOP_BOTTOM) {
                    if ((i + 1) == map->rows) {
                        break;
                    } else if ((map->cells[(i + 1) * map->cols + j] & TOP_BOTTOM) != TOP_BOTTOM) {
                        return -1;
                    }
                }
            }
        }
    }
    return EXIT_SUCCESS;
}

/**
 * Function loads maze into the struct and checks if it has right parameters.
 * @param map - into which struct should maze be loaded
 * @param maze - from which file should maze load
 * @return function returns EXIT_SUCCES on success and -1 on failure
 */
int map_load(Map *map, FILE *maze) {
    int counter = 0;
    int number;
    for (int i = 0; i < map->rows; i++) {
        for (int j = 0; j < map->cols; j++) {
            if (fscanf(maze, "%d", &number) == 1) {
                if (number >= 0) {
                    map->cells[i * map->cols + j] = number;
                    counter++;
                }
            }
        }
    }
    if (counter == map->rows * map->cols && validation(map) == EXIT_SUCCESS && fscanf(maze, "%d", &number) == EOF) {
        return EXIT_SUCCESS;
    } else return -1;
}

/**
 * Function frees allocated memory for struct.
 * @param map - struct meant to be freed
 */
void map_dtor(Map *map) {
    free(map->cells);
}

/**
 * Function converts argument into int
 * @param argv - argument to be converted into int
 * @return function returns argument converted into int
 */
int arg_check(char *argv) {
    char *check;
    int value = (int) strtol(argv, &check, 10);
    if (!isdigit(value + '0')) {
        return -1;
    }
    if (value < 1) {
        return -1;
    }
    return value;
}

/**
 * Main function that opens file and calls other function while checking their return value.
 * @param argv - which soubor is going to be opened and starting row and col
 * @return function returns EXIT_SUCCESS on success and EXIT_FAILURE on failure
 */
int maze(char *argv[]) {
    FILE *maze;
    if ((maze = fopen(argv[4], "r+")) == NULL) {
        fprintf(stderr, "couldn't open file\n");
        return EXIT_FAILURE;
    }
    int rows, cols;
    if (fscanf(maze, "%d %d[^\n]", &rows, &cols) != 2) {
        fprintf(stderr, "wrong parameters of maze\n");
        fclose(maze);
        return EXIT_FAILURE;
    }
    if (rows < 0 || cols < 0 || arg_check(argv[3]) < 1 || arg_check(argv[2]) < 1) {
        fprintf(stderr, "wrong parameters of maze or wrong arguments\n");
        fclose(maze);
        return EXIT_FAILURE;
    }
    Map map;
    if (map_ctor(&map, rows, cols) == EXIT_FAILURE) {
        fclose(maze);
        return EXIT_FAILURE;
    }

    if (map_load(&map, maze) == -1 || arg_check(argv[3]) > map.cols || arg_check(argv[2]) > map.rows) {
        fprintf(stderr, "wrong parameters of maze or wrong arguments\n");
        map_dtor(&map);
        fclose(maze);
        return EXIT_FAILURE;
    }
    if (which_path(argv, &map, arg_check(argv[2]), arg_check(argv[3])) == -1) {
        fprintf(stderr, "couldn't get into maze\n");
        map_dtor(&map);
        fclose(maze);
        return EXIT_FAILURE;
    }
    map_dtor(&map);
    fclose(maze);
    return EXIT_SUCCESS;
}

/**
 * Function tests if file contains valid or invalid maze.
 * @param argv - which file should program open
 * @return - function returns either EXIT_SUCCESS or EXIT_FAILURE depending on the result
 */
int test(char *argv[]) {
    FILE *maze;
    if ((maze = fopen(argv[2], "r+")) == NULL) {
        fprintf(stderr, "couldn't open file\n");
        return EXIT_FAILURE;
    }
    int rows, cols;
    if (fscanf(maze, "%d %d[\n]", &rows, &cols) != 2) {
        printf("Invalid\n");
        fclose(maze);
        return EXIT_FAILURE;
    }
    if (rows < 0 || cols < 0) {
        printf("Invalid\n");
        fclose(maze);
        return EXIT_FAILURE;
    }
    Map map;
    if (map_ctor(&map, rows, cols) == EXIT_FAILURE) {
        fclose(maze);
        return EXIT_FAILURE;
    }

    if (map_load(&map, maze) == -1) {
        printf("Invalid\n");
        map_dtor(&map);
        fclose(maze);
        return EXIT_FAILURE;
    } else printf("Valid\n");

    map_dtor(&map);
    fclose(maze);
    return EXIT_SUCCESS;
}

/**
 * If argument --help was used print help.
 */
void print_help() {
    printf("--help program prints hint for using program and ends.\n"
           "--test checks that file given by second argument contains valid maze.\n"
           "--rpath look for way out of maze on row R and col C using right hand rule.\n"
           "--lpath look for way out of maze on row R and col C using left hand rule.\n");
}

/**
 * Depending on arguments program is going to take one of 3 paths.
 * @param argc - number of arguments
 * @param argv - arguments
 * @return function return EXIT_SUCCESS on success or EXIT_FAILURE on failure
 */
int main(int argc, char *argv[]) {
    if (argc == 2 && !strcmp(argv[1], "--help")) {
        print_help();
    } else if (argc == 3 && !strcmp(argv[1], "--test")) {
        if (test(argv) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
    } else if (argc == 5 &&
               (!strcmp(argv[1], "--rpath") || !strcmp(argv[1], "--lpath") || !strcmp(argv[1], "--shortest"))) {
        if (maze(argv) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
    } else fprintf(stderr, "invalid arguments\n");
    return EXIT_SUCCESS;
}