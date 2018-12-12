//
//  day12.c
//  day12
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

#define STATE_PADDING   300

unsigned int calculateRuleIndex(char *rule)
{
    unsigned int ruleIndex = 0;
    for(int i = 0; i < 5; i++)
    {
        ruleIndex <<= 1;
        if(rule[i] == '#')
        { ruleIndex |= 1; }
    }
    return ruleIndex;
}

int64_t calculateStateSum(char *ruleResultMap,
                          char *initialState,
                          size_t stateCount,
                          int zeroIndex,
                          int64_t generationCount)
{
    //Initialize states
    char *newState = malloc(sizeof(char) * stateCount);
    char *currentState = malloc(sizeof(char) * stateCount);
    memcpy(currentState, initialState, sizeof(char) * stateCount);
    
    //Start generation loop
    int64_t lastStateSum = 0;
    int64_t lastDelta = 0;
    for(int64_t g = 1; g < generationCount + 1; g++)
    {
        //Get next generation
        int64_t currentStateSum = 0;
        for(int i = 2; i < stateCount - 2; i++)
        {
            //Get rule for current index
            //Calculate rule index using rule
            //Get rule result from map using rule index
            char *rule = currentState + i - 2;
            int ruleIndex = calculateRuleIndex(rule);
            char ruleResult = ruleResultMap[ruleIndex];
            
            //Set result to index in new state and increment current state sum
            newState[i] = ruleResult;
            currentStateSum += (ruleResult == '#') ? (i - zeroIndex) : 0;
        }
        
        //If last state sum delta is the same, we have stabalized
        int64_t d = currentStateSum - lastStateSum;
        if(lastDelta == d)
        {
            //Use stabalized delta to calculate final state sum
            int64_t gDelta = generationCount - g;
            lastStateSum = currentStateSum + (d * gDelta);
            break;
        }
        
        //Swap current and new
        char *tmp = currentState;
        currentState = newState;
        newState = tmp;
        
        //Update last state & delta
        lastStateSum = currentStateSum;
        lastDelta = d;
    }
    
    //Cleanup
    free(currentState);
    free(newState);
    
    //Return last state sum
    return lastStateSum;
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day12 <INPUT>\n");
        exit(0);
    }
    
    //Parse initial state
    FILE *file = fopen(argv[1], "r");
    char state[256];
    fscanf(file, "initial state: %s\n\n", state);

    //Initialize initial state with padding
    size_t stateCount = strlen(state);
    size_t initialStateCount = stateCount + STATE_PADDING;
    int zeroIndex = STATE_PADDING / 2;
    char *initialState = malloc(sizeof(char) * initialStateCount);
    memset(initialState, '.', sizeof(char) * initialStateCount);
    memcpy(initialState + zeroIndex, state, stateCount);
    
    //Initialize rule result map
    char ruleResultMap[32];
    memset(ruleResultMap, '.', 32);
    
    //Parse rules
    char rule[6];
    char ruleResult;
    while(fscanf(file, "%s => %c", rule, &ruleResult) == 2)
    {
        int ruleIndex = calculateRuleIndex(rule);
        ruleResultMap[ruleIndex] = ruleResult;
    }
    fclose(file);

    //Find state sum after twenty generations
    int64_t lastStateSumTwenty = calculateStateSum(ruleResultMap,
                                                   initialState,
                                                   initialStateCount,
                                                   zeroIndex,
                                                   20);
    printf("part1: state sum after twenty = %lld\n", lastStateSumTwenty);
    
    //Find state sum after fifty billion generations
    int64_t lastStateSumFiftyBillion = calculateStateSum(ruleResultMap,
                                                         initialState,
                                                         initialStateCount,
                                                         zeroIndex,
                                                         50000000000);
    printf("part2: state sum after fifty billion = %lld\n", lastStateSumFiftyBillion);
    
    //Finished
    return 0;
}
