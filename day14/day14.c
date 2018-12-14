//
//  day14.c
//  day14
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

typedef struct recipe recipe;
struct recipe
{
    int score;
    recipe *next;
};

#pragma mark -
#pragma mark Functions
#pragma mark

recipe *appendNewRecipe(int newScore, recipe *lastRecipe)
{
    //Create new recipe with new score and insert it after the last recipe
    recipe *newRecipe = malloc(sizeof(recipe));
    newRecipe->score = newScore;
    newRecipe->next = lastRecipe->next;
    lastRecipe->next = newRecipe;
    return newRecipe;
}

void freeRecipes(recipe *startRecipe, int64_t recipesCount)
{
    //Free all recipes
    recipe *freeRecipe = startRecipe;
    for(int i = 0; i < recipesCount; i++)
    {
        recipe *nextRecipe = freeRecipe->next;
        free(freeRecipe);
        freeRecipe = nextRecipe;
    }
}

int *intToDigits(int value, int *count)
{
    //Count number of digits
    *count = 0;
    int c = value;
    while (c > 0)
    {
        c /= 10;
        *count += 1;
    }
    
    //Create array of digits
    int *returnDigits = NULL;
    if(*count > 0)
    {
        returnDigits = malloc(sizeof(int) * *count);
        int d = value;
        int i = *count - 1;
        while(d > 0)
        {
            returnDigits[i] = d % 10;
            d /= 10;
            i--;
        }
    }
    return returnDigits;
}

#pragma mark -
#pragma mark Main Function
#pragma mark

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day14 <INPUT>\n");
        exit(0);
    }
    
    //Parse threshold
    FILE *file = fopen(argv[1], "r");
    int threshold;
    fscanf(file, "%d", &threshold);
    fclose(file);
    
    //Get array of threshold digits
    int thresholdDigitsCount = 0;
    int thresholdMatchIndex = 0;
    int *thresholdDigits = intToDigits(threshold, &thresholdDigitsCount);
    
    //Initialize first two recipes
    int64_t recipesCount = 2;
    recipe *startRecipe = malloc(sizeof(recipe));
    startRecipe->score = 3;
    startRecipe->next = malloc(sizeof(recipe));
    startRecipe->next->score = 7;
    startRecipe->next->next = startRecipe;
    recipe *lastRecipe = startRecipe->next;
    
    //Set elves' initial recipes and start creating new recipes
    recipe *elf1Recipe = startRecipe;
    recipe *elf2Recipe = startRecipe->next;
    for(;;)
    {
        //Save current last recipe and current recipes count
        recipe *currentLastRecipe = lastRecipe;
        int64_t currentRecipesCount = recipesCount;
        
        //Add the elves' scores together to get score sum
        int recipeScoreSum = elf1Recipe->score + elf2Recipe->score;
        if(recipeScoreSum < 10)
        {
            //Score sum only has 1 digit; Add it as a new recipe
            lastRecipe = appendNewRecipe(recipeScoreSum, lastRecipe);
            recipesCount++;
        }
        else
        {
            //Score sum has 2 digits; Add both as new recipes
            lastRecipe = appendNewRecipe(recipeScoreSum / 10, lastRecipe);
            lastRecipe = appendNewRecipe(recipeScoreSum % 10, lastRecipe);
            recipesCount += 2;
        }
        
        //If we have enough recipes to get 10 recipes after threshold
        if(recipesCount == threshold + 10)
        {
            //Get threshold recipe
            recipe *thresholdRecipe = startRecipe;
            for(int i = 0; i < threshold; i++)
            { thresholdRecipe = thresholdRecipe->next; }
            
            //Print next 10 recipes
            printf("part1: 10 recipes after threshold = ");
            for(int i = 0; i < 10; i++)
            {
                printf("%d", thresholdRecipe->score);
                thresholdRecipe = thresholdRecipe->next;
            }
            printf("\n");
        }
        
        //Check new recipes for threshold pattern
        int64_t newRecipes = recipesCount - currentRecipesCount;
        int64_t recipesBeforeThreshold = -1;
        recipe *thresholdCheckRecipe = currentLastRecipe->next;
        for(int i = 0; i < newRecipes; i++)
        {
            //If recipe breaks threshold pattern, reset match index
            thresholdMatchIndex = (thresholdCheckRecipe->score != thresholdDigits[thresholdMatchIndex]) ? 0 : thresholdMatchIndex;
            
            //If recipe score matches next threshold digit in pattern, increment match index
            if(thresholdCheckRecipe->score == thresholdDigits[thresholdMatchIndex])
            { thresholdMatchIndex++; }
            
            //If all threshold digits have been matched
            if(thresholdMatchIndex == thresholdDigitsCount)
            {
                //Calculate start of threshold pattern and break
                recipesBeforeThreshold = (recipesCount - (i + 1)) - thresholdDigitsCount;
                break;
            }
            
            //Get next recipe
            thresholdCheckRecipe = thresholdCheckRecipe->next;
        }
        
        //If threshold pattern was matched
        if(recipesBeforeThreshold >= 0)
        {
            //Print number of recipes before threshold and break
            printf("part2: number of recipes before threshold = %lld\n", recipesBeforeThreshold);
            break;
        }
        
        //Move first elf to next recipe
        int elf1NextRecipeSteps = elf1Recipe->score + 1;
        for(int i = 0; i < elf1NextRecipeSteps; i++)
        { elf1Recipe = elf1Recipe->next; }
        
        //Move second elf to next recipe
        int elf2NextRecipeSteps = elf2Recipe->score + 1;
        for(int i = 0; i < elf2NextRecipeSteps; i++)
        { elf2Recipe = elf2Recipe->next; }
    }
    
    //Cleanup
    freeRecipes(startRecipe, recipesCount);
    free(thresholdDigits);
    return 0;
}
