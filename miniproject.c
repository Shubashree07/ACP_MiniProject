#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ROWS 30
#define COLS 80
#define MAX_OBJECTS 50

char canvas[ROWS][COLS];

#define CIRCLE    1
#define RECTANGLE 2
#define LINE      3
#define TRIANGLE  4

typedef struct
{
    int type;
    int active;

    int x1, y1;
    int x2, y2;
    int x3, y3;

    int radius;
} Object;

Object objects[MAX_OBJECTS];
int total = 0;

/*--------------------------------------------------*/
/* Canvas Functions                                  */
/*--------------------------------------------------*/

void clear_canvas()
{
    int r, c;

    for (r = 0; r < ROWS; r++)
    {
        for (c = 0; c < COLS; c++)
        {
            canvas[r][c] = '-';
        }
    }
}

void plot(int row, int col, char ch)
{
    if (row >= 0 && row < ROWS &&
        col >= 0 && col < COLS)
    {
        canvas[row][col] = ch;
    }
}

void display_picture()
{
    int r, c;

    printf("\n+");
    for (c = 0; c < COLS; c++)
        printf("-");
    printf("+\n");

    for (r = 0; r < ROWS; r++)
    {
        printf("|");

        for (c = 0; c < COLS; c++)
        {
            printf("%c", canvas[r][c]);
        }

        printf("|\n");
    }

    printf("+");
    for (c = 0; c < COLS; c++)
        printf("-");
    printf("+\n");
}

/*--------------------------------------------------*/
/* Line (Bresenham)                                  */
/*--------------------------------------------------*/

void draw_line(int r1, int c1, int r2, int c2, char ch)
{
    int dx = abs(c2 - c1);
    int dy = abs(r2 - r1);

    int sx = (c1 < c2) ? 1 : -1;
    int sy = (r1 < r2) ? 1 : -1;

    int err = dx - dy;

    while (1)
    {
        plot(r1, c1, ch);

        if (r1 == r2 && c1 == c2)
            break;

        int e2 = 2 * err;

        if (e2 > -dy)
        {
            err -= dy;
            c1 += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            r1 += sy;
        }
    }
}

/*--------------------------------------------------*/
/* Rectangle Boundary                                */
/*--------------------------------------------------*/

void draw_rectangle(int r1, int c1, int r2, int c2, char ch)
{
    int r, c;

    if (r1 > r2)
    {
        int t = r1;
        r1 = r2;
        r2 = t;
    }

    if (c1 > c2)
    {
        int t = c1;
        c1 = c2;
        c2 = t;
    }

    for (c = c1; c <= c2; c++)
    {
        plot(r1, c, ch);
        plot(r2, c, ch);
    }

    for (r = r1; r <= r2; r++)
    {
        plot(r, c1, ch);
        plot(r, c2, ch);
    }
}

/*--------------------------------------------------*/
/* Circle Boundary                                   */
/*--------------------------------------------------*/

void draw_circle(int centerRow, int centerCol,
                 int radius, char ch)
{
    int r, c;

    for (r = centerRow - radius;
         r <= centerRow + radius;
         r++)
    {
        for (c = centerCol - radius;
             c <= centerCol + radius;
             c++)
        {
            int dx = c - centerCol;
            int dy = r - centerRow;

            double dist = sqrt(dx * dx + dy * dy);

            if (fabs(dist - radius) < 0.5)
            {
                plot(r, c, ch);
            }
        }
    }
}

/*--------------------------------------------------*/
/* Triangle Boundary                                 */
/*--------------------------------------------------*/

void draw_triangle(int r1, int c1,
                   int r2, int c2,
                   int r3, int c3,
                   char ch)
{
    draw_line(r1, c1, r2, c2, ch);
    draw_line(r2, c2, r3, c3, ch);
    draw_line(r3, c3, r1, c1, ch);
}

/*--------------------------------------------------*/
/* Render                                             */
/*--------------------------------------------------*/

void render()
{
    int i;

    clear_canvas();

    for (i = 0; i < total; i++)
    {
        if (!objects[i].active)
            continue;

        switch (objects[i].type)
        {
            case CIRCLE:
                draw_circle(objects[i].y1,
                            objects[i].x1,
                            objects[i].radius,
                            '*');
                break;

            case RECTANGLE:
                draw_rectangle(objects[i].y1,
                               objects[i].x1,
                               objects[i].y2,
                               objects[i].x2,
                               '*');
                break;

            case LINE:
                draw_line(objects[i].y1,
                          objects[i].x1,
                          objects[i].y2,
                          objects[i].x2,
                          '*');
                break;

            case TRIANGLE:
                draw_triangle(objects[i].y1,
                              objects[i].x1,
                              objects[i].y2,
                              objects[i].x2,
                              objects[i].y3,
                              objects[i].x3,
                              '*');
                break;
        }
    }
}

/*--------------------------------------------------*/
/* List Objects                                       */
/*--------------------------------------------------*/

void list_objects()
{
    int i;

    printf("\nObjects:\n");

    for (i = 0; i < total; i++)
    {
        if (!objects[i].active)
            continue;

        printf("ID %d : ", i + 1);

        switch (objects[i].type)
        {
            case CIRCLE:
                printf("Circle\n");
                break;

            case RECTANGLE:
                printf("Rectangle\n");
                break;

            case LINE:
                printf("Line\n");
                break;

            case TRIANGLE:
                printf("Triangle\n");
                break;
        }
    }
}

/*--------------------------------------------------*/
/* Add Circle                                         */
/*--------------------------------------------------*/

void add_circle()
{
    if (total >= MAX_OBJECTS)
        return;

    objects[total].type = CIRCLE;
    objects[total].active = 1;

    printf("Center column row: ");
    scanf("%d %d",
          &objects[total].x1,
          &objects[total].y1);

    printf("Radius: ");
    scanf("%d",
          &objects[total].radius);

    total++;
}

/*--------------------------------------------------*/
/* Add Rectangle                                      */
/*--------------------------------------------------*/

void add_rectangle()
{
    if (total >= MAX_OBJECTS)
        return;

    objects[total].type = RECTANGLE;
    objects[total].active = 1;

    printf("Top-left column row: ");
    scanf("%d %d",
          &objects[total].x1,
          &objects[total].y1);

    printf("Bottom-right column row: ");
    scanf("%d %d",
          &objects[total].x2,
          &objects[total].y2);

    total++;
}

/*--------------------------------------------------*/
/* Add Line                                           */
/*--------------------------------------------------*/

void add_line_object()
{
    if (total >= MAX_OBJECTS)
        return;

    objects[total].type = LINE;
    objects[total].active = 1;

    printf("Start column row: ");
    scanf("%d %d",
          &objects[total].x1,
          &objects[total].y1);

    printf("End column row: ");
    scanf("%d %d",
          &objects[total].x2,
          &objects[total].y2);

    total++;
}

/*--------------------------------------------------*/
/* Add Triangle                                       */
/*--------------------------------------------------*/

void add_triangle()
{
    if (total >= MAX_OBJECTS)
        return;

    objects[total].type = TRIANGLE;
    objects[total].active = 1;

    printf("Vertex1 column row: ");
    scanf("%d %d",
          &objects[total].x1,
          &objects[total].y1);

    printf("Vertex2 column row: ");
    scanf("%d %d",
          &objects[total].x2,
          &objects[total].y2);

    printf("Vertex3 column row: ");
    scanf("%d %d",
          &objects[total].x3,
          &objects[total].y3);

    total++;
}

/*--------------------------------------------------*/
/* Delete Object                                      */
/*--------------------------------------------------*/

void delete_object()
{
    int id;

    list_objects();

    printf("Enter object ID to delete: ");
    scanf("%d", &id);

    if (id >= 1 && id <= total)
    {
        objects[id - 1].active = 0;
        printf("Object deleted.\n");
    }
}

/*--------------------------------------------------*/
/* Main                                               */
/*--------------------------------------------------*/

int main()
{
    int choice;

    memset(objects, 0, sizeof(objects));

    while (1)
    {
        printf("\n");
        printf("1. Add Circle\n");
        printf("2. Add Rectangle\n");
        printf("3. Add Line\n");
        printf("4. Add Triangle\n");
        printf("5. Delete Object\n");
        printf("6. List Objects\n");
        printf("7. Display Picture\n");
        printf("0. Exit\n");
        printf("Choice: ");

        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                add_circle();
                break;

            case 2:
                add_rectangle();
                break;

            case 3:
                add_line_object();
                break;

            case 4:
                add_triangle();
                break;

            case 5:
                delete_object();
                break;

            case 6:
                list_objects();
                break;

            case 7:
                render();
                display_picture();
                break;

            case 0:
                printf("Goodbye!\n");
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }
}
