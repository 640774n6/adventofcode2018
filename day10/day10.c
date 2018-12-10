//
//  day10.c
//  day10
//
//  Created by Brandon Holland on 2018-12-03.
//  Copyright © 2018 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct point point;
struct point
{
    int64_t x;
    int64_t y;
    int64_t vx;
    int64_t vy;
};

void printMessage(point *points, int count)
{
    //Get x,y min and max
    int64_t xMin = LONG_LONG_MAX;
    int64_t yMin = LONG_LONG_MAX;
    int64_t xMax = LONG_LONG_MIN;
    int64_t yMax = LONG_LONG_MIN;
    for(int i = 0; i < count; i++)
    {
        point p = points[i];
        xMin = MIN(xMin, p.x);
        yMin = MIN(yMin, p.y);
        xMax = MAX(xMax, p.x);
        yMax = MAX(yMax, p.y);
    }
    
    //Normalize coordinates and get width/height
    int64_t width = LONG_LONG_MIN;
    int64_t height = LONG_LONG_MIN;
    for(int i = 0; i < count; i++)
    {
        point *p = &points[i];
        
        if(xMin < 0)
        { p->x += -xMin; }
        else if(xMin > 0)
        { p->x -= xMin; }
        
        if(yMin < 0)
        { p->y += -yMin; }
        else if (yMin > 0)
        { p->y -= yMin; }
        
        width = MAX(width, p->x);
        height = MAX(height, p->y);
    }
    
    //Print message
    for(int y = 0; y <= height; y++)
    {
        for(int x = 0; x <= width; x++)
        {
            //Search for matching point
            int found = 0;
            for(int i = 0; i < count; i++)
            {
                //If point matches current x,y
                if(points[i].x == x && points[i].y == y)
                {
                    //We found a match
                    found = 1;
                    break;
                }
            }
            
            //Print result of position
            if(found)
            { printf("#"); }
            else
            { printf("."); }
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day10 <INPUT>\n");
        exit(0);
    }
    
    //Initialize points
    point *points = malloc(sizeof(point));
    int pointsCount = 0;
    
    //Get lines
    FILE *file = fopen(argv[1], "r");
    char line[256];
    while(fgets(line, sizeof(line), file))
    {
        //Cleanup whitespace mess
        int i = 0;
        char cleanLine[256];
        for(char *c = line; *c != '\0'; c++)
        {
            if(*c != ' ')
            {
                cleanLine[i] = *c;
                i++;
            }
        }
        
        //Parse points from cleaned up line
        point newPoint;
        if(sscanf(cleanLine, "position=<%lld,%lld>velocity=<%lld,%lld>",
                  &newPoint.x,
                  &newPoint.y,
                  &newPoint.vx,
                  &newPoint.vy) == 4)
        {
            //Add new point
            pointsCount++;
            points = realloc(points, sizeof(point) * pointsCount);
            points[pointsCount - 1] = newPoint;
        }
    }
    fclose(file);
	
    //Search for message    
    int64_t seconds = 0;
    int64_t minSpace = LONG_LONG_MAX;
    for(;;)
    {
        //Get x,y min and max for current iteration
        int64_t xMin = LONG_LONG_MAX;
        int64_t yMin = LONG_LONG_MAX;
        int64_t xMax = LONG_LONG_MIN;
        int64_t yMax = LONG_LONG_MIN;
        for(int i = 0; i < pointsCount; i++)
        {
            xMin = MIN(xMin, points[i].x);
            yMin = MIN(yMin, points[i].y);
            xMax = MAX(xMax, points[i].x);
            yMax = MAX(yMax, points[i].y);
        }
        
        //Calculate space
        int64_t width = llabs(xMin - xMax);
        int64_t height = llabs(yMin - yMax);
        int64_t space = width * height;
        
        //If space is larger than smallest space, we are expanding again.
        //Therefore, previous iteration will be the message.
        if(space > minSpace)
        {
            //Go back to previous iteration
            for(int i = 0; i < pointsCount; i++)
            {
                points[i].x -= points[i].vx;
                points[i].y -= points[i].vy;
            }
            seconds--;
            
            //Print message
            printf("part1: message =\n");
            printMessage(points, pointsCount);
            
            //Print seconds
            printf("part2: seconds = %lld\n", seconds);
            break;
        }
        
        //Update min space
        minSpace = MIN(minSpace, space);
        
        //Advance to next iteration
        for(int i = 0; i < pointsCount; i++)
        {
            points[i].x += points[i].vx;
            points[i].y += points[i].vy;
        }
        seconds++;
    }
    
    //Cleanup
    free(points);
    return 0;
}
