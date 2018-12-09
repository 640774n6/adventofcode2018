//
//  day9.c
//  day9
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

typedef struct marble marble;
struct marble
{
    int worth;
    marble *before;
    marble *after;
};

int64_t playMarbleMania(int playerCount, int lastMarbleValue)
{
    //Initialize starting marble
    marble *startMarble = malloc(sizeof(marble));
    startMarble->worth = 0;
    startMarble->after = startMarble;
    startMarble->before = startMarble;
    
    //Initialize player scores
    int64_t playerScores[playerCount];
    memset(playerScores, 0, sizeof(int64_t) * playerCount);
    
    //Play game until last marble
    int currentPlayer = 0;
    marble *currentMarble = startMarble;
    for(int i = 1; i <= lastMarbleValue; i++)
    {
        if(i % 23 == 0)
        {
            //Get 7th clockwise marble
            marble *marble7CCW = currentMarble;
            for(int i = 0; i < 7; i++)
            { marble7CCW = marble7CCW->before; }
            
            //Add current marble's worth and 7th clockwise marble's worth to current player score
            playerScores[currentPlayer] += i;
            playerScores[currentPlayer] += marble7CCW->worth;
            
            //Remove 7th clockwise marble
            marble7CCW->before->after = marble7CCW->after;
            marble7CCW->after->before = marble7CCW->before;
            
            //Set current marble to 1st marble clockwise the 7th clockwise marble
            currentMarble = marble7CCW->after;
            
            //Free 7th clockwise marble
            free(marble7CCW);
        }
        else
        {
            //Create new marble
            marble *newMarble = malloc(sizeof(marble));
            newMarble->worth = i;
            
            //Get 1st and 2nd clockwise marbles
            marble *marble1CW = currentMarble->after;
            marble *marble2CW = currentMarble->after->after;
            
            //Insert new marble between 1st and 2nd clockwise marbles
            newMarble->after = marble2CW;
            newMarble->before = marble1CW;
            marble1CW->after = newMarble;
            marble2CW->before = newMarble;
            
            //Set new marble as current marble
            currentMarble = newMarble;
        }
        
        //Increment player and restart if last player
        currentPlayer++;
        if(currentPlayer >= playerCount)
        { currentPlayer = 0; }
    }
    
    //Find high score
    int64_t highScore = INT_MIN;
    for(int i = 0; i < playerCount; i++)
    { highScore = MAX(highScore, playerScores[i]); }
    
    //Free all marbles
    marble *marbleToFree = startMarble;
    startMarble->before->after = NULL;
    do
    {
        marble *nextMarble = marbleToFree->after;
        free(marbleToFree);
        marbleToFree = nextMarble;
    }
    while(marbleToFree);
    
    //Return high score
    return highScore;
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day9 <INPUT>\n");
        exit(0);
    }
    
    //Parse player count and last marble points
    FILE *file = fopen(argv[1], "r");
    int playerCount;
    int lastMarbleValue;
    fscanf(file, "%d players; last marble is worth %d points", &playerCount, &lastMarbleValue);
    fclose(file);
    
    int64_t highScorePart1 = playMarbleMania(playerCount, lastMarbleValue);
    printf("part1: high score = %lld\n", highScorePart1);
    
    int64_t highScorePart2 = playMarbleMania(playerCount, lastMarbleValue * 100);
    printf("part2: high score = %lld\n", highScorePart2);
    
    //Finished
    return 0;
}
