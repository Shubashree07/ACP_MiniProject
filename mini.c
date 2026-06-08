#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef USE_NCURSES
#include <ncurses.h>
#endif

#define WIDTH 40
#define HEIGHT 20
#define MAX_OBJECTS 100

typedef enum {
    OBJECT_CIRCLE = 1,
    OBJECT_RECTANGLE,
    OBJECT_LINE,
    OBJECT_TRIANGLE
} ObjectType;

typedef struct {
    int id;
    ObjectType type;
    int x1, y1;
    int x2, y2;
    int x3, y3;
    int radius;
    char brush;
} GraphicObject;

static char canvas[HEIGHT][WIDTH];
static GraphicObject objects[MAX_OBJECTS];
static int objectCount = 0;
static int nextId = 1;

void clearCanvas(void) {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            canvas[y][x] = '_';
        }
    }
}

int isInsideCanvas(int x, int y) {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

void plotPixel(int x, int y, char brush) {
    if (isInsideCanvas(x, y)) {
        canvas[y][x] = brush;
    }
}

void drawLine(int x1, int y1, int x2, int y2, char brush) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (1) {
        plotPixel(x1, y1, brush);
        if (x1 == x2 && y1 == y2) {
            break;
        }
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void drawRectangle(int x1, int y1, int x2, int y2, char brush) {
    int left = x1 < x2 ? x1 : x2;
    int right = x1 < x2 ? x2 : x1;
    int top = y1 < y2 ? y1 : y2;
    int bottom = y1 < y2 ? y2 : y1;

    for (int x = left; x <= right; ++x) {
        plotPixel(x, top, brush);
        plotPixel(x, bottom, brush);
    }
    for (int y = top; y <= bottom; ++y) {
        plotPixel(left, y, brush);
        plotPixel(right, y, brush);
    }
}

void drawCircle(int cx, int cy, int radius, char brush) {
    if (radius <= 0) {
        return;
    }
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            double dx = x - cx;
            double dy = y - cy;
            double distance = sqrt(dx * dx + dy * dy);
            if (fabs(distance - radius) <= 0.5) {
                plotPixel(x, y, brush);
            }
        }
    }
}

void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, char brush) {
    drawLine(x1, y1, x2, y2, brush);
    drawLine(x2, y2, x3, y3, brush);
    drawLine(x3, y3, x1, y1, brush);
}

void redrawCanvas(void) {
    clearCanvas();
    for (int i = 0; i < objectCount; ++i) {
        GraphicObject *obj = &objects[i];
        switch (obj->type) {
            case OBJECT_CIRCLE:
                drawCircle(obj->x1, obj->y1, obj->radius, obj->brush);
                break;
            case OBJECT_RECTANGLE:
                drawRectangle(obj->x1, obj->y1, obj->x2, obj->y2, obj->brush);
                break;
            case OBJECT_LINE:
                drawLine(obj->x1, obj->y1, obj->x2, obj->y2, obj->brush);
                break;
            case OBJECT_TRIANGLE:
                drawTriangle(obj->x1, obj->y1, obj->x2, obj->y2, obj->x3, obj->y3, obj->brush);
                break;
        }
    }
}

#ifndef USE_NCURSES
void displayPicture(void) {
    printf("\nPicture (%dx%d):\n", WIDTH, HEIGHT);
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            putchar(canvas[y][x]);
        }
        putchar('\n');
    }
}

void listObjects(void) {
    if (objectCount == 0) {
        printf("No objects in the picture.\n");
        return;
    }
    printf("\nObjects:\n");
    for (int i = 0; i < objectCount; ++i) {
        GraphicObject *obj = &objects[i];
        printf("ID %d: ", obj->id);
        switch (obj->type) {
            case OBJECT_CIRCLE:
                printf("Circle center=(%d,%d) radius=%d\n", obj->x1, obj->y1, obj->radius);
                break;
            case OBJECT_RECTANGLE:
                printf("Rectangle corner1=(%d,%d) corner2=(%d,%d)\n", obj->x1, obj->y1, obj->x2, obj->y2);
                break;
            case OBJECT_LINE:
                printf("Line from=(%d,%d) to=(%d,%d)\n", obj->x1, obj->y1, obj->x2, obj->y2);
                break;
            case OBJECT_TRIANGLE:
                printf("Triangle (%d,%d), (%d,%d), (%d,%d)\n", obj->x1, obj->y1, obj->x2, obj->y2, obj->x3, obj->y3);
                break;
            default:
                printf("Unknown object type\n");
                break;
        }
    }
}

int promptInt(const char *prompt) {
    int value;
    printf("%s", prompt);
    scanf("%d", &value);
    return value;
}

void promptCoordinates(const char *prompt, int *x, int *y) {
    printf("%s", prompt);
    scanf("%d %d", x, y);
}

void pauseMessage(const char *message) {
    printf("%s\n", message);
}

void addCircle(void) {
    if (objectCount >= MAX_OBJECTS) {
        printf("Cannot add more objects.\n");
        return;
    }
    GraphicObject obj;
    obj.id = nextId++;
    obj.type = OBJECT_CIRCLE;
    obj.brush = '*';
    promptCoordinates("Enter center x y (0..39 0..19): ", &obj.x1, &obj.y1);
    obj.radius = promptInt("Enter radius: ");
    objects[objectCount++] = obj;
    redrawCanvas();
    printf("Circle added with ID %d.\n", obj.id);
}

void addRectangle(void) {
    if (objectCount >= MAX_OBJECTS) {
        printf("Cannot add more objects.\n");
        return;
    }
    GraphicObject obj;
    obj.id = nextId++;
    obj.type = OBJECT_RECTANGLE;
    obj.brush = '*';
    promptCoordinates("Enter first corner x y (0..39 0..19): ", &obj.x1, &obj.y1);
    promptCoordinates("Enter opposite corner x y: ", &obj.x2, &obj.y2);
    objects[objectCount++] = obj;
    redrawCanvas();
    printf("Rectangle added with ID %d.\n", obj.id);
}

void addLine(void) {
    if (objectCount >= MAX_OBJECTS) {
        printf("Cannot add more objects.\n");
        return;
    }
    GraphicObject obj;
    obj.id = nextId++;
    obj.type = OBJECT_LINE;
    obj.brush = '*';
    promptCoordinates("Enter start point x y (0..39 0..19): ", &obj.x1, &obj.y1);
    promptCoordinates("Enter end point x y: ", &obj.x2, &obj.y2);
    objects[objectCount++] = obj;
    redrawCanvas();
    printf("Line added with ID %d.\n", obj.id);
}

void addTriangle(void) {
    if (objectCount >= MAX_OBJECTS) {
        printf("Cannot add more objects.\n");
        return;
    }
    GraphicObject obj;
    obj.id = nextId++;
    obj.type = OBJECT_TRIANGLE;
    obj.brush = '*';
    promptCoordinates("Enter first vertex x y (0..39 0..19): ", &obj.x1, &obj.y1);
    promptCoordinates("Enter second vertex x y: ", &obj.x2, &obj.y2);
    promptCoordinates("Enter third vertex x y: ", &obj.x3, &obj.y3);
    objects[objectCount++] = obj;
    redrawCanvas();
    printf("Triangle added with ID %d.\n", obj.id);
}

void deleteObject(void) {
    if (objectCount == 0) {
        printf("No objects to delete.\n");
        return;
    }
    int id = promptInt("Enter object ID to delete: ");
    int index = -1;
    for (int i = 0; i < objectCount; ++i) {
        if (objects[i].id == id) {
            index = i;
            break;
        }
    }
    if (index < 0) {
        printf("Object ID %d not found.\n", id);
        return;
    }
    for (int i = index; i + 1 < objectCount; ++i) {
        objects[i] = objects[i + 1];
    }
    objectCount--;
    redrawCanvas();
    printf("Object ID %d deleted.\n", id);
}

void printMenu(void) {
    printf("\n2D Graphics Editor:\n");
    printf("1. Add circle\n");
    printf("2. Add rectangle\n");
    printf("3. Add line\n");
    printf("4. Add triangle\n");
    printf("5. Delete object\n");
    printf("6. Display picture\n");
    printf("7. List objects\n");
    printf("8. Clear picture\n");
    printf("0. Exit\n");
}

int main(void) {
    clearCanvas();
    int choice = -1;

    do {
        printMenu();
        choice = promptInt("Choose an option: ");
        switch (choice) {
            case 1:
                addCircle();
                break;
            case 2:
                addRectangle();
                break;
            case 3:
                addLine();
                break;
            case 4:
                addTriangle();
                break;
            case 5:
                deleteObject();
                break;
            case 6:
                displayPicture();
                break;
            case 7:
                listObjects();
                break;
            case 8:
                objectCount = 0;
                nextId = 1;
                clearCanvas();
                printf("Picture cleared.\n");
                break;
            case 0:
                printf("Exiting graphics editor.\n");
                break;
            default:
                printf("Please choose a valid option.\n");
                break;
        }
    } while (choice != 0);

    return 0;
}
#else
static const int canvasRow = 1;
static const int menuRow = HEIGHT + 2;
static const int promptRow = HEIGHT + 9;

void drawCanvasNcurses(void) {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            mvaddch(canvasRow + y, x, canvas[y][x]);
        }
    }
    refresh();
}

int promptIntNcurses(const char *prompt) {
    int value = 0;
    echo();
    curs_set(1);
    mvprintw(promptRow, 0, "%-80s", "");
    mvprintw(promptRow, 0, "%s", prompt);
    move(promptRow + 1, 0);
    clrtoeol();
    refresh();
    scanw("%d", &value);
    noecho();
    curs_set(0);
    return value;
}

void promptCoordinatesNcurses(const char *prompt, int *x, int *y) {
    echo();
    curs_set(1);
    mvprintw(promptRow, 0, "%-80s", "");
    mvprintw(promptRow, 0, "%s", prompt);
    move(promptRow + 1, 0);
    clrtoeol();
    refresh();
    scanw("%d %d", x, y);
    noecho();
    curs_set(0);
}

void pauseNcurses(const char *message) {
    mvprintw(promptRow + 2, 0, "%-80s", "");
    mvprintw(promptRow + 2, 0, "%s", message);
    refresh();
    getch();
}

void listObjects(void) {
    int row = menuRow;
    mvprintw(row++, 0, "Objects:");
    if (objectCount == 0) {
        mvprintw(row++, 0, "No objects in the picture.");
    } else {
        for (int i = 0; i < objectCount; ++i) {
            GraphicObject *obj = &objects[i];
            switch (obj->type) {
                case OBJECT_CIRCLE:
                    mvprintw(row++, 0, "ID %d: Circle center=(%d,%d) radius=%d", obj->id, obj->x1, obj->y1, obj->radius);
                    break;
                case OBJECT_RECTANGLE:
                    mvprintw(row++, 0, "ID %d: Rectangle corner1=(%d,%d) corner2=(%d,%d)", obj->id, obj->x1, obj->y1, obj->x2, obj->y2);
                    break;
                case OBJECT_LINE:
                    mvprintw(row++, 0, "ID %d: Line from=(%d,%d) to=(%d,%d)", obj->id, obj->x1, obj->y1, obj->x2, obj->y2);
                    break;
                case OBJECT_TRIANGLE:
                    mvprintw(row++, 0, "ID %d: Triangle (%d,%d), (%d,%d), (%d,%d)", obj->id, obj->x1, obj->y1, obj->x2, obj->y2, obj->x3, obj->y3);
                    break;
                default:
                    mvprintw(row++, 0, "ID %d: Unknown object type", obj->id);
                    break;
            }
        }
    }
    refresh();
    pauseNcurses("Press any key to continue...");
}

void addCircle(void) {
    if (objectCount >= MAX_OBJECTS) {
        pauseNcurses("Cannot add more objects. Press any key...");
        return;
    }
    GraphicObject obj;
    obj.id = nextId++;
    obj.type = OBJECT_CIRCLE;
    obj.brush = '*';
    promptCoordinatesNcurses("Enter center x y (0..39 0..19): ", &obj.x1, &obj.y1);
    obj.radius = promptIntNcurses("Enter radius: ");
    objects[objectCount++] = obj;
    redrawCanvas();
    pauseNcurses("Circle added. Press any key...");
}

void addRectangle(void) {
    if (objectCount >= MAX_OBJECTS) {
        pauseNcurses("Cannot add more objects. Press any key...");
        return;
    }
    GraphicObject obj;
    obj.id = nextId++;
    obj.type = OBJECT_RECTANGLE;
    obj.brush = '*';
    promptCoordinatesNcurses("Enter first corner x y (0..39 0..19): ", &obj.x1, &obj.y1);
    promptCoordinatesNcurses("Enter opposite corner x y: ", &obj.x2, &obj.y2);
    objects[objectCount++] = obj;
    redrawCanvas();
    pauseNcurses("Rectangle added. Press any key...");
}

void addLine(void) {
    if (objectCount >= MAX_OBJECTS) {
        pauseNcurses("Cannot add more objects. Press any key...");
        return;
    }
    GraphicObject obj;
    obj.id = nextId++;
    obj.type = OBJECT_LINE;
    obj.brush = '*';
    promptCoordinatesNcurses("Enter start point x y (0..39 0..19): ", &obj.x1, &obj.y1);
    promptCoordinatesNcurses("Enter end point x y: ", &obj.x2, &obj.y2);
    objects[objectCount++] = obj;
    redrawCanvas();
    pauseNcurses("Line added. Press any key...");
}

void addTriangle(void) {
    if (objectCount >= MAX_OBJECTS) {
        pauseNcurses("Cannot add more objects. Press any key...");
        return;
    }
    GraphicObject obj;
    obj.id = nextId++;
    obj.type = OBJECT_TRIANGLE;
    obj.brush = '*';
    promptCoordinatesNcurses("Enter first vertex x y (0..39 0..19): ", &obj.x1, &obj.y1);
    promptCoordinatesNcurses("Enter second vertex x y: ", &obj.x2, &obj.y2);
    promptCoordinatesNcurses("Enter third vertex x y: ", &obj.x3, &obj.y3);
    objects[objectCount++] = obj;
    redrawCanvas();
    pauseNcurses("Triangle added. Press any key...");
}

void deleteObject(void) {
    if (objectCount == 0) {
        pauseNcurses("No objects to delete. Press any key...");
        return;
    }
    int id = promptIntNcurses("Enter object ID to delete: ");
    int index = -1;
    for (int i = 0; i < objectCount; ++i) {
        if (objects[i].id == id) {
            index = i;
            break;
        }
    }
    if (index < 0) {
        pauseNcurses("Object ID not found. Press any key...");
        return;
    }
    for (int i = index; i + 1 < objectCount; ++i) {
        objects[i] = objects[i + 1];
    }
    objectCount--;
    redrawCanvas();
    pauseNcurses("Object deleted. Press any key...");
}

void displayPicture(void) {
    drawCanvasNcurses();
    pauseNcurses("Picture displayed. Press any key...");
}

void printMenu(void) {
    int row = menuRow;
    mvprintw(row++, 0, "2D Graphics Editor (ncurses)");
    mvprintw(row++, 0, "1. Add circle");
    mvprintw(row++, 0, "2. Add rectangle");
    mvprintw(row++, 0, "3. Add line");
    mvprintw(row++, 0, "4. Add triangle");
    mvprintw(row++, 0, "5. Delete object");
    mvprintw(row++, 0, "6. Display picture");
    mvprintw(row++, 0, "7. List objects");
    mvprintw(row++, 0, "8. Clear picture");
    mvprintw(row++, 0, "0. Exit");
    mvprintw(row++, 0, "Choose an option:");
    refresh();
}

int main(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    clearCanvas();
    redrawCanvas();

    int choice = -1;
    do {
        clear();
        drawCanvasNcurses();
        printMenu();
        choice = promptIntNcurses("");
        switch (choice) {
            case 1:
                addCircle();
                break;
            case 2:
                addRectangle();
                break;
            case 3:
                addLine();
                break;
            case 4:
                addTriangle();
                break;
            case 5:
                deleteObject();
                break;
            case 6:
                displayPicture();
                break;
            case 7:
                listObjects();
                break;
            case 8:
                objectCount = 0;
                nextId = 1;
                clearCanvas();
                redrawCanvas();
                pauseNcurses("Picture cleared. Press any key...");
                break;
            case 0:
                pauseNcurses("Exiting graphics editor. Press any key...");
                break;
            default:
                pauseNcurses("Please choose a valid option. Press any key...");
                break;
        }
    } while (choice != 0);

    endwin();
    return 0;
}
#endif
