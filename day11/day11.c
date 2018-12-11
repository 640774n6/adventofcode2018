//
//  day11.c
//  day11
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

#define GRID_SIZE  300

void findMaxTotal(int **sums,
                  int size,
                  int *returnTotalLevel,
                  int *returnX,
                  int *returnY)
{
    //Iterate through sum table
    *returnTotalLevel = INT_MIN;
    for(int y = 1; y < GRID_SIZE - size; y++)
    {
        for(int x = 1; x < GRID_SIZE - size; x++)
        {
            //Calculate total sum for area
            int xMin = x - 1;
            int yMin = y - 1;
            int a = sums[xMin][yMin];
            int b = sums[xMin + size][yMin];
            int c = sums[xMin][yMin + size];
            int d = sums[xMin + size][yMin + size];
            int totalLevel = d - b - c + a;
            
            //If sum is larger than current max
            if(totalLevel > *returnTotalLevel)
            {
                //Update current max
                *returnTotalLevel = totalLevel;
                *returnX = x;
                *returnY = y;
            }
        }
    }
}

int calculateLevel(int x, int y, int s)
{
    //Calculate fuel level
    int rackId = x + 10;
    int level = ((rackId * y) + s) * rackId;
    level = (level / 100) % 10;
    return level - 5;
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day11 <INPUT>\n");
        exit(0);
    }
    
    //Parse serial number
    FILE *file = fopen(argv[1], "r");
    int serialNumber;
    fscanf(file, "%d", &serialNumber);
    fclose(file);
    
    //Initialize summed area table
    int **sums = malloc(sizeof(int *) * GRID_SIZE);
    for(int i = 0; i < GRID_SIZE; i++)
    {
        sums[i] = malloc(sizeof(int) * GRID_SIZE);
        memset(sums[i], 0, sizeof(int) * GRID_SIZE);
    }

    //Generate summed area table
    for(int y = 1; y < GRID_SIZE; y++)
    {
        for(int x = 1; x < GRID_SIZE; x++)
        {
            sums[x][y] =
                calculateLevel(x, y, serialNumber) +
                sums[x][y - 1] +
                sums[x - 1][y] -
                sums[x - 1][y - 1];
        }
    }
    
    //Find max total 3x3 level coordinates
    int total3x3 = -1;
    int x3x3 = -1;
    int y3x3 = -1;
    findMaxTotal(sums, 3, &total3x3, &x3x3, &y3x3);
    printf("part1: fuel cell group (3) with largest total power (%d) = %d,%d\n",
           total3x3,
           x3x3,
           y3x3);
    
    //Find max total max size level coordinates
    int maxTotal = INT_MIN;
    int maxX = -1;
    int maxY = -1;
    int maxSize = -1;
    for(int i = 1; i < GRID_SIZE; i++)
    {
        int x = -1;
        int y = -1;
        int total = -1;
        findMaxTotal(sums, i, &total, &x, &y);
        
        if(total > maxTotal)
        {
            maxTotal = total;
            maxX = x;
            maxY = y;
            maxSize = i;
        }
    }
    printf("part2: fuel cell group (%d) with largest total power (%d) = %d,%d\n",
           maxSize,
           maxTotal,
           maxX,
           maxY);
    
    //Cleanup
    for(int i = 0; i < GRID_SIZE; i++)
    { free(sums[i]); }
    free(sums);
    
    return 0;
}
