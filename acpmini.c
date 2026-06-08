#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ROWS 40
#define COLS 80
#define MAX_OBJECTS 100

// Canvas
char canvas[ROWS][COLS];

// Object types
typedef enum { CIRCLE, RECTANGLE, LINE, TRIANGLE } ObjectType;

typedef struct {
    ObjectType type;
    int x1, y1, x2, y2, x3, y3, radius;
    int active;
} Object;

Object objects[MAX_OBJECTS];
int objectCount = 0;

// ─── Canvas Utilities ───────────────────────────────────────────────

void initCanvas() {
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            canvas[i][j] = '_';
}

void displayCanvas() {
    printf("\n");
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++)
            printf("%c", canvas[i][j]);
        printf("\n");
    }
}

void setPixel(int x, int y, char ch) {
    if (x >= 0 && x < ROWS && y >= 0 && y < COLS)
        canvas[x][y] = ch;
}

// ─── Drawing Functions ───────────────────────────────────────────────

void drawCircle(int cx, int cy, int r, char ch) {
    for (int angle = 0; angle < 360; angle++) {
        double rad = angle * 3.14159 / 180.0;
        int x = (int)(cx + r * sin(rad) * 0.5); // 0.5 to adjust aspect ratio
        int y = (int)(cy + r * cos(rad));
        setPixel(x, y, ch);
    }
}

void drawRectangle(int x1, int y1, int x2, int y2, char ch) {
    for (int j = y1; j <= y2; j++) {
        setPixel(x1, j, ch);
        setPixel(x2, j, ch);
    }
    for (int i = x1; i <= x2; i++) {
        setPixel(i, y1, ch);
        setPixel(i, y2, ch);
    }
}

void drawLine(int x1, int y1, int x2, int y2, char ch) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        setPixel(x1, y1, ch);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 <  dx) { err += dx; y1 += sy; }
    }
}

void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, char ch) {
    drawLine(x1, y1, x2, y2, ch);
    drawLine(x2, y2, x3, y3, ch);
    drawLine(x3, y3, x1, y1, ch);
}

// ─── Redraw All Active Objects ───────────────────────────────────────

void redrawAll() {
    initCanvas();
    for (int i = 0; i < objectCount; i++) {
        if (!objects[i].active) continue;
        Object *o = &objects[i];
        switch (o->type) {
            case CIRCLE:    drawCircle(o->x1, o->y1, o->radius, '*'); break;
            case RECTANGLE: drawRectangle(o->x1, o->y1, o->x2, o->y2, '*'); break;
            case LINE:      drawLine(o->x1, o->y1, o->x2, o->y2, '*'); break;
            case TRIANGLE:  drawTriangle(o->x1, o->y1, o->x2, o->y2, o->x3, o->y3, '*'); break;
        }
    }
}

// ─── Add Objects ─────────────────────────────────────────────────────

void addCircle() {
    if (objectCount >= MAX_OBJECTS) { printf("Max objects reached.\n"); return; }
    Object o = {0};
    o.type = CIRCLE; o.active = 1;
    printf("Enter center row, col, and radius: ");
    scanf("%d %d %d", &o.x1, &o.y1, &o.radius);
    objects[objectCount++] = o;
    redrawAll();
    printf("Circle added (ID: %d).\n", objectCount - 1);
}

void addRectangle() {
    if (objectCount >= MAX_OBJECTS) { printf("Max objects reached.\n"); return; }
    Object o = {0};
    o.type = RECTANGLE; o.active = 1;
    printf("Enter top-left (row col) and bottom-right (row col): ");
    scanf("%d %d %d %d", &o.x1, &o.y1, &o.x2, &o.y2);
    objects[objectCount++] = o;
    redrawAll();
    printf("Rectangle added (ID: %d).\n", objectCount - 1);
}

void addLine() {
    if (objectCount >= MAX_OBJECTS) { printf("Max objects reached.\n"); return; }
    Object o = {0};
    o.type = LINE; o.active = 1;
    printf("Enter start (row col) and end (row col): ");
    scanf("%d %d %d %d", &o.x1, &o.y1, &o.x2, &o.y2);
    objects[objectCount++] = o;
    redrawAll();
    printf("Line added (ID: %d).\n", objectCount - 1);
}

void addTriangle() {
    if (objectCount >= MAX_OBJECTS) { printf("Max objects reached.\n"); return; }
    Object o = {0};
    o.type = TRIANGLE; o.active = 1;
    printf("Enter three vertices (row col) each: ");
    scanf("%d %d %d %d %d %d", &o.x1, &o.y1, &o.x2, &o.y2, &o.x3, &o.y3);
    objects[objectCount++] = o;
    redrawAll();
    printf("Triangle added (ID: %d).\n", objectCount - 1);
}

// ─── Delete Object ────────────────────────────────────────────────────

void deleteObject() {
    int id;
    printf("Enter object ID to delete (0 to %d): ", objectCount - 1);
    scanf("%d", &id);
    if (id < 0 || id >= objectCount || !objects[id].active) {
        printf("Invalid or already deleted object ID.\n");
        return;
    }
    objects[id].active = 0;
    redrawAll();
    printf("Object %d deleted.\n", id);
}

// ─── List Objects ─────────────────────────────────────────────────────

void listObjects() {
    const char *names[] = {"Circle", "Rectangle", "Line", "Triangle"};
    printf("\n%-5s %-12s %-30s\n", "ID", "Type", "Parameters");
    printf("------------------------------------------------------\n");
    for (int i = 0; i < objectCount; i++) {
        if (!objects[i].active) continue;
        Object *o = &objects[i];
        printf("%-5d %-12s ", i, names[o->type]);
        switch (o->type) {
            case CIRCLE:    printf("center=(%d,%d) r=%d", o->x1, o->y1, o->radius); break;
            case RECTANGLE: printf("(%d,%d) to (%d,%d)", o->x1, o->y1, o->x2, o->y2); break;
            case LINE:      printf("(%d,%d) to (%d,%d)", o->x1, o->y1, o->x2, o->y2); break;
            case TRIANGLE:  printf("(%d,%d),(%d,%d),(%d,%d)", o->x1,o->y1,o->x2,o->y2,o->x3,o->y3); break;
        }
        printf("\n");
    }
}

// ─── Main Menu ────────────────────────────────────────────────────────

int main() {
    initCanvas();
    int choice;

    while (1) {
        printf("\n╔══════════════════════════════╗\n");
        printf("║   2D Graphics Editor Menu    ║\n");
        printf("╠══════════════════════════════╣\n");
        printf("║ 1. Add Circle                ║\n");
        printf("║ 2. Add Rectangle             ║\n");
        printf("║ 3. Add Line                  ║\n");
        printf("║ 4. Add Triangle              ║\n");
        printf("║ 5. Delete Object             ║\n");
        printf("║ 6. Display Canvas            ║\n");
        printf("║ 7. List Objects              ║\n");
        printf("║ 8. Clear Canvas              ║\n");
        printf("║ 0. Exit                      ║\n");
        printf("╚══════════════════════════════╝\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addCircle();     break;
            case 2: addRectangle();  break;
            case 3: addLine();       break;
            case 4: addTriangle();   break;
            case 5: deleteObject();  break;
            case 6: displayCanvas(); break;
            case 7: listObjects();   break;
            case 8:
                initCanvas();
                objectCount = 0;
                printf("Canvas cleared.\n");
                break;
            case 0:
                printf("Exiting editor.\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
}
