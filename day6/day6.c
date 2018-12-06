//
//  day6.c
//  day6
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

typedef struct coordinate coordinate;
struct coordinate
{
    int id;
    int x;
    int y;
    int area;
    int finate;
};

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day6 <INPUT>\n");
        exit(0);
    }
    
    //Initialize variables
    coordinate *coordinates = malloc(sizeof(coordinate));
    int coordinatesCount = 0;

    coordinate maxCoordinate;
    maxCoordinate.x = INT_MIN;
    maxCoordinate.y = INT_MIN;
    
    coordinate minCoordinate;
    minCoordinate.x = INT_MAX;
    minCoordinate.x = INT_MAX;
    
    //Parse coordinates
    FILE *file = fopen(argv[1], "r");
    coordinate newCoordinate;
    while(fscanf(file, "%d, %d", &newCoordinate.x, &newCoordinate.y) == 2)
    {
        //Setup coordinate
        newCoordinate.id = coordinatesCount + 1;
        newCoordinate.area = 0;
        newCoordinate.finate = 1;
        
        //Add coordinate
        coordinatesCount++;
        coordinates = realloc(coordinates, sizeof(coordinate) * coordinatesCount);
        coordinates[coordinatesCount - 1] = newCoordinate;
        
        //Update max/min coordinates
        maxCoordinate.x = MAX(maxCoordinate.x, newCoordinate.x);
        maxCoordinate.y = MAX(maxCoordinate.y, newCoordinate.y);
        minCoordinate.x = MIN(minCoordinate.x, newCoordinate.x);
        minCoordinate.y = MIN(minCoordinate.y, newCoordinate.y);
    }
    fclose(file);
    
    //Calculate grid dimensions
    int gridWidth = maxCoordinate.x + 1;
    int gridHeight = maxCoordinate.y + 1;
    
    //Calculate coordinate areas
    for(int y = minCoordinate.y; y < gridHeight; y++)
    {
        for(int x = minCoordinate.x; x < gridWidth; x++)
        {
            //Initialize distances
            int distancesCounter[maxCoordinate.x + maxCoordinate.y + 1];
            memset(distancesCounter, 0, sizeof(int) * (maxCoordinate.x + maxCoordinate.y + 1));
            
            //Find id of min distance coordinate
            int minDistance = INT_MAX;
            int minDistanceCoordinateId = -1;
            for(int c = 0; c < coordinatesCount; c++)
            {
                coordinate coordinate = coordinates[c];
                int distance = abs(coordinate.x - x) + abs(coordinate.y - y);
                distancesCounter[distance]++;
                
                if(minDistance > distance)
                {
                    minDistance = distance;
                    minDistanceCoordinateId = coordinate.id;
                }
            }
            
            //If min distance only occured for one coordinate
            if(distancesCounter[minDistance] == 1)
            {
                //Increment coordinate area
                coordinates[minDistanceCoordinateId - 1].area++;
                
                //If location is min or max; area for min distance coordinate is infinate
                if(x == minCoordinate.x ||
                   x == maxCoordinate.x ||
                   y == minCoordinate.y ||
                   y == maxCoordinate.y)
                { coordinates[minDistanceCoordinateId - 1].finate = 0; }
            }
        }
    }
    
    //Find max area
    int maxArea = INT_MIN;
    for(int i = 0; i < coordinatesCount; i++)
    {
        coordinate coordinate = coordinates[i];
        if(coordinate.finate == 1)
        { maxArea = MAX(maxArea, coordinate.area); }
    }
    printf("part1: max finate area = %d\n", maxArea);
    
    //Calculate safe region area
    int safeRegionArea = 0;
    for(int y = minCoordinate.y; y < gridHeight; y++)
    {
        for(int x = minCoordinate.x; x < gridWidth; x++)
        {
            //Calculate total of distances to all coordinates
            int distanceTotal = 0;
            for(int c = 0; c < coordinatesCount; c++)
            {
                coordinate coordinate = coordinates[c];
                int distance = abs(coordinate.x - x) + abs(coordinate.y - y);
                distanceTotal += distance;
            }
            
            //If it is below the safe threshold; increment safe region area
            if(distanceTotal < 10000)
            { safeRegionArea++; }
        }
    }
    printf("part2: safe region area = %d\n", safeRegionArea);
    
    //Cleanup
    free(coordinates);
    return 0;
}
