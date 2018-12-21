//
//  day15.c
//  day15
//
//  Created by Brandon Holland on 2018-12-03.
//  Copyright © 2018 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define START_HP        200
#define DEFAULT_ATTACK  3

#pragma mark -
#pragma mark Structures
#pragma mark

typedef enum unit_type unit_type;
enum unit_type
{
    unit_type_elf = 'E',
    unit_type_goblin = 'G'
};

typedef struct unit unit;
struct unit
{
    int x;
    int y;
    unit_type type;
    int hp;
    int lastRound;
};

typedef enum tile_type tile_type;
enum tile_type
{
    tile_type_open = '.',
    tile_type_wall = '#'
};

typedef struct tile tile;
struct tile
{
    int x;
    int y;
    unit *current_unit;
    tile_type type;
};

typedef struct step step;
struct step
{
    tile *t;
    int d;
    step *parent;
};

typedef enum check_state check_state;
enum check_state
{
    check_state_unchecked = 0,
    check_state_queued = 1,
    check_state_checked = 2
};

#pragma mark -
#pragma mark Functions
#pragma mark

void freeMap(tile ***map, size_t width, size_t height)
{
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            tile *t = map[x][y];
            if(t->current_unit)
            { free(t->current_unit); }
        }
        free(map[x]);
    }
    free(map);
}

tile ***copyMap(tile ***map, size_t width, size_t height)
{
    tile ***mapCopy = malloc(sizeof(tile **) * width);
    for (int x = 0; x < width; x++)
    {
        mapCopy[x] = malloc(sizeof(tile *) * height);
        memset(mapCopy[x], 0, sizeof(tile *) * height);
        
        for(int y = 0; y < height; y++)
        {
            tile *t = map[x][y];
            if(!t)
            { continue; }
            
            tile *tileCopy = malloc(sizeof(tile));
            memcpy(tileCopy, t, sizeof(tile));
            if(tileCopy->current_unit)
            {
                tileCopy->current_unit = malloc(sizeof(unit));
                memcpy(tileCopy->current_unit, t->current_unit, sizeof(unit));
            }
            mapCopy[x][y] = tileCopy;
        }
    }
    return mapCopy;
}

unit **getUnitsOnMap(tile ***map, size_t width, size_t height, unit_type ut, int *count)
{
    unit **units = malloc(sizeof(unit *));
    int unitsCount = 0;
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            tile *t = map[x][y];
            if(t->current_unit && t->current_unit->type == ut)
            {
                unitsCount++;
                units = realloc(units, sizeof(unit *) * unitsCount);
                units[unitsCount - 1] = t->current_unit;
            }
        }
    }
    *count = unitsCount;
    return units;
}

int tieBreakUnits(unit *a, unit *b)
{ return (b->y < a->y || (b->y == a->y && b->x < a->x)); }

int tieBreakTiles(tile *a, tile *b)
{ return (b->y < a->y || (b->y == a->y && b->x < a->x)); }

unit *getEnemyToAttack(unit *u, tile ***map, size_t width, size_t height)
{
    unit *enemy = NULL;
    int minEnemyHP = INT_MAX;
    
    tile **attackTiles = malloc(sizeof(tile *) * 4);
    attackTiles[0] = (u->y - 1 >= 0) ? map[u->x][u->y - 1] : NULL;     //up
    attackTiles[1] = (u->x - 1 >= 0) ? map[u->x - 1][u->y] : NULL;     //left
    attackTiles[2] = (u->x + 1 < width) ? map[u->x + 1][u->y] : NULL;  //right
    attackTiles[3] = (u->y + 1 < height) ? map[u->x][u->y + 1] : NULL; //down
    for(int t = 0; t < 4; t++)
    {
        tile *attackTile = attackTiles[t];
        if(attackTile &&
           attackTile->current_unit &&
           attackTile->current_unit->type != u->type)
        {
            unit *attackUnit = attackTile->current_unit;
            if(minEnemyHP == attackUnit->hp && tieBreakUnits(enemy, attackUnit))
            { enemy = attackUnit; }
            else if(minEnemyHP > attackUnit->hp)
            {
                minEnemyHP = attackUnit->hp;
                enemy = attackUnit;
            }
        }
    }
    free(attackTiles);
    return enemy;
}

tile *getNextMove(unit *u, tile ***map, size_t width, size_t height, unit **enemies, int enemiesCount)
{
    //Initialize variables
    int minDistance = INT_MAX;
    tile *minDistanceNextMove = NULL;
    tile *minDistanceAvailablePosition = NULL;
    
    //Initialize tiles check state
    check_state **tilesCheckState = malloc(sizeof(check_state *) * width);
    for(int x = 0; x < width; x++)
    {
        tilesCheckState[x] = malloc(sizeof(int) * height);
        memset(tilesCheckState[x], check_state_unchecked, sizeof(int) * height);
    }
    
    //Iterate through each enemy
    for(int e = 0; e < enemiesCount; e++)
    {
        //Get next alive enemy
        unit *currentEnemy = enemies[e];
        if(currentEnemy->hp == 0)
        { continue; }

        //Initialize root step
        step *rootStep = malloc(sizeof(step));
        rootStep->parent = NULL;
        rootStep->d = 0;
        rootStep->t = map[u->x][u->y];
        
        //Initialize steps to check and add root step
        step **stepsToCheck = malloc(sizeof(step *));
        int stepsToCheckCount = 1;
        stepsToCheck[0] = rootStep;
        
        //Iterate through each steps to check
        for(int s = 0; s < stepsToCheckCount; s++)
        {
            //Get next step and mark it as checked
            step *currentStep = stepsToCheck[s];
            tilesCheckState[currentStep->t->x][currentStep->t->y] = check_state_checked;
            
            //If the tile's current unit is current enemy, we found shortest path to enemy
            if(currentStep->t->current_unit == currentEnemy)
            {
                //Trace back to step after root
                step *nextMoveStep = currentStep;
                while(nextMoveStep->parent != rootStep)
                { nextMoveStep = nextMoveStep->parent; }
                
                //Update min enemy bfs path
                if(currentStep->d == minDistance && tieBreakTiles(minDistanceAvailablePosition, currentStep->parent->t))
                {
                    minDistanceNextMove = nextMoveStep->t;
                    minDistanceAvailablePosition = currentStep->parent->t;
                }
                else if(currentStep->d < minDistance)
                {
                    minDistance = currentStep->d;
                    minDistanceNextMove = nextMoveStep->t;
                    minDistanceAvailablePosition = currentStep->parent->t;
                }
                break;
            }
            
            //Add child (neighbor) tiles if they qualify
            tile *childTiles[4];
            childTiles[0] = (currentStep->t->y - 1 >= 0) ? map[currentStep->t->x][currentStep->t->y - 1] : NULL;     //up
            childTiles[1] = (currentStep->t->x - 1 >= 0) ? map[currentStep->t->x - 1][currentStep->t->y] : NULL;     //left
            childTiles[2] = (currentStep->t->x + 1 < width) ? map[currentStep->t->x + 1][currentStep->t->y] : NULL;  //right
            childTiles[3] = (currentStep->t->y + 1 < height) ? map[currentStep->t->x][currentStep->t->y + 1] : NULL; //down
            for(int t = 0; t < 4; t++)
            {
                tile *childTile = childTiles[t];
                if(childTile &&
                   tilesCheckState[childTile->x][childTile->y] == check_state_unchecked &&
                   childTile->type == tile_type_open &&
                   (!childTile->current_unit || childTile->current_unit->type != u->type))
                {
                    //Create child step for tile
                    step *childStep = malloc(sizeof(step));
                    childStep->parent = currentStep;
                    childStep->d = currentStep->d + 1;
                    childStep->t = childTile;
                    
                    //Add child step to steps to check
                    stepsToCheckCount++;
                    stepsToCheck = realloc(stepsToCheck, sizeof(step *) * stepsToCheckCount);
                    stepsToCheck[stepsToCheckCount - 1] = childStep;
                    
                    //Mark child step as queued
                    tilesCheckState[childStep->t->x][childStep->t->y] = check_state_queued;
                }
            }
        }
        
        //Free all steps to check
        for(int s = 0; s < stepsToCheckCount; s++)
        { free(stepsToCheck[s]); }
        free(stepsToCheck);
        
        //Reset tiles check state
        for(int x = 0; x < width; x++)
        { memset(tilesCheckState[x], check_state_unchecked, sizeof(int) * height); }
    }
    
    //Free tiles check state
    for(int x = 0; x < width; x++)
    { free(tilesCheckState[x]); }
    free(tilesCheckState);
    
    //Return next move tile if an attack position is available
    return minDistanceNextMove;
}

int fightToTheDeath(tile ***map,
                    size_t width,
                    size_t height,
                    int elfAttackPower,
                    int goblinAttackPower,
                    unit **elves,
                    unit **goblins,
                    int *elvesCount,
                    int *goblinsCount,
                    unit_type *winner)
{
    //Initialize variables
    int elvesTotalHP = *elvesCount * START_HP;
    int goblinsTotalHP = *goblinsCount * START_HP;
    int startElvesCount = *elvesCount;
    int startGoblinsCount = *goblinsCount;
    int round = 0;
    
    //Battle forever until winner found
    for(;;)
    {
        //Iterate through each tile
        int unitTurnsForRound = 0;
        int totalUnitsForRound = *elvesCount + *goblinsCount;
        for(int y = 0; y < height; y++)
        {
            int allUnitTookTurn = 0;
            for(int x = 0; x < width; x++)
            {
                //Skip if tile doesn't have a current unit or current unit already had a turn
                tile *mapTile = map[x][y];
                if(!mapTile->current_unit || mapTile->current_unit->lastRound == round)
                { continue; }
                
                //Get current unit and update last round
                unit *currentUnit = mapTile->current_unit;
                currentUnit->lastRound = round;
                
                //Get enemies and enemy counts for current unit
                unit **enemies = (currentUnit->type == unit_type_elf) ? goblins : elves;
                int *enemiesCount = (currentUnit->type == unit_type_elf) ? &startGoblinsCount : &startElvesCount;
                int *remainingEnemiesCount = (currentUnit->type == unit_type_elf) ? goblinsCount : elvesCount;
                
                //If there are no remaining enemies
                if(*remainingEnemiesCount == 0)
                {
                    //Find winner of the battle's total hp and return outcome
                    *winner = (elvesTotalHP == 0) ? unit_type_goblin : unit_type_elf;
                    int winnerTotalHP = (elvesTotalHP == 0) ? goblinsTotalHP : elvesTotalHP;
                    int battleOutcome = round * winnerTotalHP;
                    return battleOutcome;
                }
                
                //If unit doesn't have an enemy to attack
                unit *currentEnemy = getEnemyToAttack(currentUnit, map, width, height);
                if(!currentEnemy)
                {
                    //If a next move to closest enemy exists
                    tile *nextMove = getNextMove(currentUnit, map, width, height, enemies, *enemiesCount);
                    if(nextMove)
                    {
                        //Move the current unit
                        currentUnit->x = nextMove->x;
                        currentUnit->y = nextMove->y;
                        mapTile->current_unit = NULL;
                        nextMove->current_unit = currentUnit;
                        
                        //See if there is an enemy to attack now
                        currentEnemy = getEnemyToAttack(currentUnit, map, width, height);
                    }
                }
                
                //If there is an enemy to attack
                if(currentEnemy)
                {
                    //Attack and update enemies total hp
                    int attackPower = (currentUnit->type == unit_type_elf) ? elfAttackPower : goblinAttackPower;
                    int damage = (currentEnemy->hp >= attackPower) ? attackPower : currentEnemy->hp;
                    int *enemiesTotalHP = (currentUnit->type == unit_type_elf) ? &goblinsTotalHP : &elvesTotalHP;
                    *enemiesTotalHP -= damage;
                    currentEnemy->hp -= damage;
                    
                    //If enemy is killed
                    if(currentEnemy->hp == 0)
                    {
                        //Remove enemy from map
                        tile *currentEnemyTile = map[currentEnemy->x][currentEnemy->y];
                        currentEnemyTile->current_unit = NULL;
                        
                        //Decrement remaining enemy count
                        *remainingEnemiesCount -= 1;
                    }
                }
                
                //If number of unit turns this round is equal to total number of units for round
                unitTurnsForRound++;
                if(unitTurnsForRound == totalUnitsForRound)
                {
                    //All units took turn this round; break
                    allUnitTookTurn = 1;
                    break;
                }
            }
            
            //All units took turn this round; break
            if(allUnitTookTurn)
            { break; }
        }
        
        //Increment round
        round++;
    }
}

#pragma mark -
#pragma mark Main Function
#pragma mark

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day15 <INPUT>\n");
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
    
    //Initialize map
    size_t width = lineSize - 1;
    size_t height = totalSize / lineSize;
    tile ***map = malloc(sizeof(tile **) * width);
    for(int x = 0; x < width; x++)
    { map[x] = malloc(sizeof(tile *) * height); }
 
    //Parse map
    int elvesCount = 0;
    int goblinsCount = 0;
    int yCounter = 0;
    while(fgets(line, sizeof(line), file))
    {
        int xCounter = 0;
        for(char *c = line; *c != '\n'; c++)
        {
            //Get map tile
            tile *mapTile = malloc(sizeof(tile));
            mapTile->x = xCounter;
            mapTile->y = yCounter;
            mapTile->current_unit = NULL;
            map[xCounter][yCounter] = mapTile;
            
            //If there is a unit on the tile
            if(*c == unit_type_elf || *c == unit_type_goblin)
            {
                //Create new unit
                unit *newUnit = malloc(sizeof(unit));
                newUnit->x = xCounter;
                newUnit->y = yCounter;
                newUnit->lastRound = -1;
                newUnit->type = *c;
                newUnit->hp = START_HP;
                
                //Increment unit counters
                if(newUnit->type == unit_type_elf)
                { elvesCount++; }
                else
                { goblinsCount++; }
                
                //Assign new unit to the map tile
                mapTile->current_unit = newUnit;
                
                //Swap unit character for open tile type
                *c = tile_type_open;
            }
            
            //Set tile type
            mapTile->type = *c;
            
            //Increment x counter
            xCounter++;
        }
        
        //Finished with line; increment y counter
        yCounter++;
    }
    fclose(file);
    
    //Fight to the death using natural configuration
    tile ***naturalOutcomeMap = copyMap(map, width, height);
    int naturalElvesCount = 0;
    int naturalGoblinsCount = 0;
    unit **naturalElves = getUnitsOnMap(naturalOutcomeMap, width, height, unit_type_elf, &naturalElvesCount);
    unit **naturalGoblins = getUnitsOnMap(naturalOutcomeMap, width, height, unit_type_goblin, &naturalGoblinsCount);
    unit_type naturalWinner;
    int naturalOutcome = fightToTheDeath(naturalOutcomeMap,
                                         width,
                                         height,
                                         DEFAULT_ATTACK,
                                         DEFAULT_ATTACK,
                                         naturalElves,
                                         naturalGoblins,
                                         &naturalElvesCount,
                                         &naturalGoblinsCount,
                                         &naturalWinner);
    char *winnerName = (naturalWinner == unit_type_elf) ? "elves" : "goblins";
    printf("part1: natural fight outcome = %s with %d\n", winnerName, naturalOutcome);
    freeMap(naturalOutcomeMap, width, height);
    free(naturalElves);
    free(naturalGoblins);
    
    //Fight to the death increasing elf attack until elves win
    unit_type modifiedWinner;
    int modifiedElvesCount = 0;
    int modifiedGoblinsCount = 0;
    int modifiedOutcome;
    int elfAttackPower = 3;
    do
    {
        elfAttackPower++;
        
        tile ***modifiedOutcomeMap = copyMap(map, width, height);
        unit **modifiedElves = getUnitsOnMap(modifiedOutcomeMap, width, height, unit_type_elf, &modifiedElvesCount);
        unit **modifiedGoblins = getUnitsOnMap(modifiedOutcomeMap, width, height, unit_type_goblin, &modifiedGoblinsCount);
        modifiedOutcome = fightToTheDeath(modifiedOutcomeMap,
                                          width,
                                          height,
                                          elfAttackPower,
                                          DEFAULT_ATTACK,
                                          modifiedElves,
                                          modifiedGoblins,
                                          &modifiedElvesCount,
                                          &modifiedGoblinsCount,
                                          &modifiedWinner);
        freeMap(modifiedOutcomeMap, width, height);
        free(modifiedElves);
        free(modifiedGoblins);
    }
    while(modifiedWinner != unit_type_elf || modifiedElvesCount != elvesCount);
    printf("part2: elf best fight outcome = %d @ %d attack\n", modifiedOutcome, elfAttackPower);
    
    //Cleanup
    freeMap(map, width, height);
    return 0;
}
