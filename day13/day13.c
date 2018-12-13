//
//  day13.c
//  day13
//
//  Created by Brandon Holland on 2018-12-03.
//  Copyright © 2018 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#pragma mark -
#pragma mark Structures
#pragma mark

typedef enum cart_direction cart_direction;
enum cart_direction
{
    cart_direction_up = '^',
    cart_direction_down = 'v',
    cart_direction_left = '<',
    cart_direction_right = '>'
};

typedef enum cart_turn cart_turn;
enum cart_turn
{
    cart_turn_left = 0,
    cart_turn_straight = 1,
    cart_turn_right = 2
};

typedef struct cart cart;
struct cart
{
    cart_direction direction;
    cart_turn next_turn;
    int lastTick;
};

typedef enum track_type track_type;
enum track_type
{
    track_type_horizontal = '-',
    track_type_vertical = '|',
    track_type_curve_left = '\\',
    track_type_curve_right = '/',
    track_type_intersection = '+'
};

typedef struct track track;
struct track
{
    cart *current_cart;
    track_type type;
};

#pragma mark -
#pragma mark Functions
#pragma mark

void freeTracks(track ***tracks, size_t width, size_t height)
{
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            track *t = tracks[x][y];
            if(t)
            {
                if(t->current_cart)
                { free(t->current_cart); }
                free(t);
            }
        }
        free(tracks[x]);
    }
    free(tracks);
}

track ***copyTracks(track ***tracks, size_t width, size_t height)
{
    track ***tracksCopy = malloc(sizeof(track **) * width);
    for (int x = 0; x < width; x++)
    {
        tracksCopy[x] = malloc(sizeof(track *) * height);
        memset(tracksCopy[x], 0, sizeof(track *) * height);
        
        for(int y = 0; y < height; y++)
        {
            track *t = tracks[x][y];
            if(!t)
            { continue; }
            
            track *trackCopy = malloc(sizeof(track));
            memcpy(trackCopy, t, sizeof(track));
            if(trackCopy->current_cart)
            {
                trackCopy->current_cart = malloc(sizeof(cart));
                memcpy(trackCopy->current_cart, t->current_cart, sizeof(cart));
            }
            tracksCopy[x][y] = trackCopy;
        }
    }
    return tracksCopy;
}

track *getNextTrack(track ***tracks, cart *c, int x, int y, int *nextX, int *nextY)
{
    *nextX = x;
    *nextY = y;
    switch(c->direction)
    {
        case cart_direction_up:
            *nextY -= 1;
            break;
        case cart_direction_down:
            *nextY += 1;
            break;
        case cart_direction_left:
            *nextX -= 1;
            break;
        case cart_direction_right:
            *nextX += 1;
            break;
        default:
            break;
    }
    return tracks[*nextX][*nextY];
}

void updateCartDirection(cart *c, track_type t)
{
    //Get turn for track type
    cart_turn turn = cart_turn_straight;
    switch(t)
    {
        case track_type_curve_left:
            turn = (c->direction == cart_direction_up || c->direction == cart_direction_down) ?
                cart_turn_left : cart_turn_right;
            break;
        case track_type_curve_right:
            turn = (c->direction == cart_direction_up || c->direction == cart_direction_down) ?
                cart_turn_right : cart_turn_left;
            break;
        case track_type_intersection:
            turn = c->next_turn;
            c->next_turn++;
            if(c->next_turn > 2)
            { c->next_turn = 0; }
            break;
        default:
            break;
    }
    
    //Do nothing if we are turning straight
    if(turn == cart_turn_straight)
    { return; }
    
    //Update cart direction
    switch(c->direction)
    {
        case cart_direction_up:
            c->direction = (turn == cart_turn_left) ?
                cart_direction_left : cart_direction_right;
            break;
        case cart_direction_down:
            c->direction = (turn == cart_turn_left) ?
                cart_direction_right : cart_direction_left;
            break;
        case cart_direction_left:
            c->direction = (turn == cart_turn_left) ?
                cart_direction_down : cart_direction_up;
            break;
        case cart_direction_right:
            c->direction = (turn == cart_turn_left) ?
                cart_direction_up : cart_direction_down;
            break;
        default:
            break;
    }
}

int battleCarts(track ***tracks,
                size_t width,
                size_t height,
                int cartsCount,
                int deathMatch,
                int *returnX,
                int *returnY)
{
    //Start battling carts
    int ticks = 0;
    while(cartsCount > 1)
    {
        //Increment tick
        ticks++;
        
        //Move carts
        int cartsMovedCount = 0;
        int cartsCountForTick = cartsCount;
        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                //Skip if there is no track
                //Or if there is no cart on the track
                //Or if the cart has already been moved this tick
                track *currentTrack = tracks[x][y];
                if(!currentTrack ||
                   !currentTrack->current_cart ||
                   currentTrack->current_cart->lastTick == ticks)
                { continue; }
                
                //Get next track for current cart
                int nextX = -1;
                int nextY = -1;
                cart *currentCart = currentTrack->current_cart;
                track *nextTrack = getNextTrack(tracks, currentTrack->current_cart, x, y, &nextX, &nextY);
                
                //Update return x,y
                *returnX = nextX;
                *returnY = nextY;
                
                //If the next track has a cart
                if(nextTrack->current_cart)
                {
                    //Collision! Remove crashed carts
                    currentTrack->current_cart = NULL;
                    nextTrack->current_cart = NULL;
                    free(currentTrack->current_cart);
                    free(nextTrack->current_cart);
                    cartsCount -= 2;

                    //If this isn't a death match, battle is over
                    if(!deathMatch)
                    { return ticks; }
                }
                else
                {
                    //Move cart to next track and update cart direction
                    nextTrack->current_cart = currentCart;
                    currentTrack->current_cart = NULL;
                    updateCartDirection(currentCart, nextTrack->type);
                    
                    //Update cart last tick and return x,y
                    currentCart->lastTick = ticks;
                    *returnX = nextX;
                    *returnY = nextY;
                }
                
                //Increment carts moved and end tick if all are moved
                cartsMovedCount++;
                if(cartsMovedCount == cartsCountForTick)
                { break; }
            }
            
            //End tick if all carts moved
            if(cartsMovedCount == cartsCountForTick)
            { break; }
        }
    }
    
    //Return number of ticks
    return ticks;
}

#pragma mark -
#pragma mark Main Function
#pragma mark

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day13 <INPUT>\n");
        exit(0);
    }
    
    //Open input file
    FILE *file = fopen(argv[1], "r");
    
    //Get total size
    fseek(file, 0, SEEK_END);
    size_t totalSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    //Get line size
    char line[256];
    fgets(line, sizeof(line), file);
    size_t lineSize = strlen(line);
    fseek(file, 0, SEEK_SET);
    
    //Initialize tracks
    size_t width = lineSize - 1;
    size_t height = (totalSize / lineSize) - 1;
    track ***tracks = malloc(sizeof(track **) * width);
    for (int x = 0; x < width; x++)
    {
        tracks[x] = malloc(sizeof(track *) * height);
        memset(tracks[x], 0, sizeof(track *) * height);
    }
    
    //Parse tracks
    int cartsCount = 0;
    int yCounter = 0;
    while(fgets(line, sizeof(line), file))
    {
        int xCounter = 0;
        for(char *c = line; *c != '\n'; c++)
        {
            if(*c == ' ')
            {
                xCounter++;
                continue;
            }
            
            //Create new track
            track *newTrack = malloc(sizeof(track));
            memset(newTrack, 0, sizeof(track));
            
            //If there is a cart on the track
            if(*c == cart_direction_up ||
               *c == cart_direction_down ||
               *c == cart_direction_left ||
               *c == cart_direction_right)
            {
                //Create new cart
                cart *newCart = malloc(sizeof(cart));
                memset(newCart, 0, sizeof(cart));
                newCart->direction = *c;
                
                //Assign new cart to the new track and increment carts counter
                newTrack->current_cart = newCart;
                cartsCount++;
                
                //Swap cart character for correct track type
                switch(newCart->direction)
                {
                    case cart_direction_up:
                    case cart_direction_down:
                        *c = track_type_vertical;
                        break;
                    case cart_direction_left:
                    case cart_direction_right:
                        *c = track_type_horizontal;
                        break;
                    default:
                        break;
                }
            }
            
            //Set track type
            newTrack->type = *c;
            
            //Add track to tracks and increment x counter
            tracks[xCounter][yCounter] = newTrack;
            xCounter++;
        }
        
        //Finished with line; increment y counter
        yCounter++;
    }
    fclose(file);
    
    //Find coordinates of first collision
    int firstCollisionX = -1;
    int firstCollisionY = -1;
    track ***firstCollisionTracks = copyTracks(tracks, width, height);
    int firstCollisionTicks = battleCarts(firstCollisionTracks, width, height, cartsCount, 0, &firstCollisionX, &firstCollisionY);
    printf("part1: first collision @ %d,%d after %d ticks\n", firstCollisionX, firstCollisionY, firstCollisionTicks);
    freeTracks(firstCollisionTracks, width, height);
    
    //Find coordinates of last cart standing
    int lastCartStandingX = -1;
    int lastCartStandingY = -1;
    track ***deathMatchTracks = copyTracks(tracks, width, height);
    int lastCartStandingTicks = battleCarts(deathMatchTracks, width, height, cartsCount, 1, &lastCartStandingX, &lastCartStandingY);
    printf("part2; last cart standing @ %d,%d after %d ticks\n", lastCartStandingX, lastCartStandingY, lastCartStandingTicks);
    freeTracks(deathMatchTracks, width, height);
    
    //Cleanup
    freeTracks(tracks, width, height);
    return 0;
}
