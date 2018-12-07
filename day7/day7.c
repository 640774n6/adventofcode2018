//
//  day7.c
//  day7
//
//  Created by Brandon Holland on 2018-12-03.
//  Copyright © 2018 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct step step;
struct step
{
    char id;
    int working;
    int completed;
    int seconds;
    step *dependencies[26];
};

step *createNewStep(char id)
{
    step *newStep = malloc(sizeof(step));
    newStep->id = id;
    newStep->working = 0;
    newStep->completed = 0;
    newStep->seconds = id - 4;
    memset(newStep->dependencies, 0, sizeof(step *) * 26);
    return newStep;
}

int countStepIncompleteDependencies(step *s)
{
    int c = 0;
    for(int i = 0; i < 26; i++)
    {
        step *d = s->dependencies[i];
        if(d && !d->completed)
        { c++; }
    }
    return c;
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day7 <INPUT>\n");
        exit(0);
    }
    
    //Initialize steps
    step *steps[26];
    memset(steps, 0, sizeof(step *) * 26);
    
    //Parse steps and build dependency graph
    FILE *file = fopen(argv[1], "r");
    char contextStepId;
    char dependencyStepId;
    while(fscanf(file, "Step %c must be finished before step %c can begin. ", &dependencyStepId, &contextStepId) == 2)
    {
        //Get dependency step and if it doesn't exist...
        step *dependencyStep = steps[dependencyStepId - 65];
        if(!dependencyStep)
        {
            //Create new step and add it to steps
            dependencyStep = createNewStep(dependencyStepId);
            steps[dependencyStepId - 65] = dependencyStep;
        }

        //Get context step and if it doesn't exist...
        step *contextStep = steps[contextStepId - 65];
        if(!contextStep)
        {
            //Create new step and add it to steps
            contextStep = createNewStep(contextStepId);
            steps[contextStepId - 65] = contextStep;
        }
        
        //Add dependency to context step
        contextStep->dependencies[dependencyStepId - 65] = dependencyStep;
    }
    fclose(file);
    
    //Find order of steps
    printf("part1: order of steps = ");
    for(int i = 0; i < 26; i++)
    {
        step *s = steps[i];
        if(s && !s->completed && countStepIncompleteDependencies(s) == 0)
        {
            s->completed = 1;
            i = -1;
            printf("%c", s->id);
        }
    }
    printf("\n");
    
    //Reset step completion
    for(int i = 0; i < 26; i++)
    {
        step *s = steps[i];
        if(s)
        { s->completed = 0; }
    }
    
    //Initialize workers
    step *workers[5];
    memset(workers, 0, sizeof(step *) * 5);
    
    //Find total number of seconds to complete steps
    int secondsTotal = 0;
    for(;;)
    {
        //Iterate through each worker
        int working = 0;
        for(int i = 0; i < 5; i++)
        {
            //Attempt to get worker
            step *w = workers[i];
            
            //If worker is working and has completed
            if(w && w->seconds == 0)
            {
                //Mark as completed and release worker
                w->completed = 1;
                w = workers[i] = NULL;
            }
            
            //If worker is available
            if(!w)
            {
                //Iterate through each step
                for(int j = 0; j < 26; j++)
                {
                    //Get step and if it exists, is not completed, not being worked on, and has no outstanding dependencies...
                    step *s = steps[j];
                    if(s && !s->completed && !s->working && countStepIncompleteDependencies(s) == 0)
                    {
                        //Assign worker step and mark it as being worked on
                        w = s;
                        w->working = 1;
                        workers[i] = s;
                        break;
                    }
                }
            }
            
            //If worker has been assigned a step
            if(w)
            {
                //Do work
                w->seconds--;
                working = 1;
            }
        }
        
        //If no workers are working, we are done!
        if(!working)
        {
            secondsTotal--;
            break;
        }
        
        //Increment number of seconds passed
        secondsTotal++;
    }
    printf("part2: total seconds to assemble = %d\n", secondsTotal);
    
    //Cleanup
    for(int i = 0; i < 26; i++)
    { free(steps[i]); }
    
    return 0;
}
