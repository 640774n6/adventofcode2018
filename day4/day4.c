//
//  day4.c
//  day4
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

typedef struct timestamp timestamp;
struct timestamp
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
};

typedef enum logtype logtype;
enum logtype
{
    logtype_start = 0,
    logtype_sleep = 1,
    logtype_wake = 2
};

typedef struct log log;
struct log
{
    timestamp timestamp;
    logtype type;
    int guardId;
};

int compareTimestamps(timestamp t1, timestamp t2)
{
    //Compare years
    if(t1.year > t2.year)
    { return 1; }
    else if (t2.year > t1.year)
    { return 0; }
    
    //Years equal; compare months
    if(t1.month > t2.month)
    { return 1; }
    else if (t2.month > t1.month)
    { return 0; }
    
    //Months equal; compare days
    if(t1.day > t2.day)
    { return 1; }
    else if(t2.day > t1.day)
    { return 0; }
    
    //Days equal; compare hours
    if(t1.hour > t2.hour)
    { return 1; }
    else if(t2.hour > t1.hour)
    { return 0; }
    
    //Hours equal; compare minutes
    if (t1.minute > t2.minute)
    { return 1; }
    else if (t2.minute > t1.minute)
    { return 0; }
    
    //If we made it this far, timestamps are equal
    return 0;
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day4 <INPUT>\n");
        exit(0);
    }
    
    //Initialize variables
    log *logs = malloc(sizeof(log));
    int logsCount = 0;
    int maxGuardId = -1;
    
    //Parse logs
    FILE *file = fopen(argv[1], "r");
    timestamp newTimestamp;
    char newMessage[256];
    while(fscanf(file, "[%4d-%2d-%2d %2d:%2d] %[^\n] ",
                 &newTimestamp.year,
                 &newTimestamp.month,
                 &newTimestamp.day,
                 &newTimestamp.hour,
                 &newTimestamp.minute,
                 newMessage) == 6)
    {
        log newLog;
        newLog.timestamp = newTimestamp;
        
        int guardId = -1;
        if (sscanf(newMessage, "Guard #%d begins shift", &guardId) == 1)
        {
            newLog.type = logtype_start;
            newLog.guardId = guardId;
            maxGuardId = MAX(maxGuardId, guardId);
        }
        else if(!strcmp(newMessage, "falls asleep"))
        { newLog.type = logtype_sleep; }
        else if(!strcmp(newMessage, "wakes up"))
        { newLog.type = logtype_wake; }
        
        logsCount++;
        logs = realloc(logs, sizeof(log) * logsCount);
        logs[logsCount - 1] = newLog;
    }
    fclose(file);
    
    //Sort logs by timestamp
    for(int i = 0; i < logsCount; i++)
    {
        for(int j = (i + 1); j < logsCount; j++)
        {
            if(compareTimestamps(logs[i].timestamp, logs[j].timestamp) == 1)
            {
                log tmp = logs[i];
                logs[i] = logs[j];
                logs[j] = tmp;
            }
        }
    }
    
    //Initialize sleep totals and minute tracker
    int sleepTotals[maxGuardId + 1];
    memset(sleepTotals, 0, sizeof(int) * (maxGuardId + 1));
    int minuteTracker[maxGuardId + 1][60];
    memset(minuteTracker, 0, sizeof(int) * (maxGuardId + 1) * 60);
    
    //Calculate guard sleep totals and populate minute tracker
    int currentGuardId = -1;
    int maxSleepTotalGuardId = -1;
    int maxSleepTotal = -1;
    for(int i = 0; i < logsCount; i++)
    {
        switch(logs[i].type)
        {
            case logtype_start:
            {
                currentGuardId = logs[i].guardId;
                break;
            }
            case logtype_wake:
            {
                sleepTotals[currentGuardId] += (logs[i].timestamp.minute - logs[i - 1].timestamp.minute);
                if(maxSleepTotal < sleepTotals[currentGuardId])
                {
                    maxSleepTotal = sleepTotals[currentGuardId];
                    maxSleepTotalGuardId = currentGuardId;
                }
                
                for(int j = logs[i - 1].timestamp.minute; j < logs[i].timestamp.minute; j++)
                { minuteTracker[currentGuardId][j]++; }
                break;
            }
            default:
                break;
        }
    }
    
    //Find max frequent sleeping minute for longest sleeping guard
    int maxSleepTotalGuardMinute = -1;
    int maxSleepTotalGuardMinuteValue = -1;
    for(int i = 0; i < 60; i++)
    {
        if(maxSleepTotalGuardMinuteValue < minuteTracker[maxSleepTotalGuardId][i])
        {
            maxSleepTotalGuardMinuteValue = minuteTracker[maxSleepTotalGuardId][i];
            maxSleepTotalGuardMinute = i;
        }
    }
    printf("part1: guardId x minute = %d\n", maxSleepTotalGuardId * maxSleepTotalGuardMinute);
    
    //Find guard with overall max frequent sleeping minute
    int maxMinuteGuardId = -1;
    int maxMinute = -1;
    int maxMinuteValue = -1;
    for(int i = 0; i < (maxGuardId + 1); i++)
    {
        for(int j = 0; j < 60; j++)
        {
            if(maxMinuteValue < minuteTracker[i][j])
            {
                maxMinuteValue = minuteTracker[i][j];
                maxMinuteGuardId = i;
                maxMinute = j;
            }
        }
    }
    printf("part2: guardId x minute = %d\n", maxMinuteGuardId * maxMinute);
    
    //Cleanup
    free(logs);
    return 0;
}
