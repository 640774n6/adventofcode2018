//
//  day16.c
//  day16
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

typedef struct operation operation;
struct operation
{
    unsigned code;
    unsigned a;
    unsigned b;
    unsigned c;
};

typedef struct sample sample;
struct sample
{
    operation op;
    unsigned rb[4];
    unsigned ra[4];
};

#pragma mark -
#pragma mark Functions
#pragma mark

void executeOperation(operation op, unsigned c, unsigned *r)
{
    switch(c)
    {
        case 0: //addr
            r[op.c] = r[op.a] + r[op.b];
            break;
        case 1: //addi
            r[op.c] = r[op.a] + op.b;
            break;
        case 2: //mulr
            r[op.c] = r[op.a] * r[op.b];
            break;
        case 3: //muli
            r[op.c] = r[op.a] * op.b;
            break;
        case 4: //banr
            r[op.c] = r[op.a] & r[op.b];
            break;
        case 5: //bani
            r[op.c] = r[op.a] & op.b;
            break;
        case 6: //borr
            r[op.c] = r[op.a] | r[op.b];
            break;
        case 7: //bori
            r[op.c] = r[op.a] | op.b;
            break;
        case 8: //setr
            r[op.c] = r[op.a];
            break;
        case 9: //seti
            r[op.c] = op.a;
            break;
        case 10: //gtir
            r[op.c] = op.a > r[op.b];
            break;
        case 11: //gtri
            r[op.c] = r[op.a] > op.b;
            break;
        case 12: //gtrr
            r[op.c] = r[op.a] > r[op.b];
            break;
        case 13: //eqir
            r[op.c] = op.a == r[op.b];
            break;
        case 14: //eqri
            r[op.c] = r[op.a] == op.b;
            break;
        case 15: //eqrr
            r[op.c] = r[op.a] == r[op.b];
            break;
        default:
            break;
    }
}

unsigned *createOpcodeMap(sample *samples, int samplesCount)
{
    int reverseOpcodeMap[16];
    memset(reverseOpcodeMap, -1, sizeof(int) * 16);
    for(int i = 0; i < samplesCount; i++)
    {
        sample s = samples[i];
        
        int matchingCount = 0;
        unsigned lastMatchingCode = -1;
        for(int c = 0; c < 16; c++)
        {
            if(reverseOpcodeMap[c] != -1)
            { continue; }
            
            unsigned r[4];
            memcpy(r, s.rb, sizeof(unsigned) * 4);
            
            executeOperation(s.op, c, r);
            if(!memcmp(r, s.ra, sizeof(unsigned) * 4))
            {
                lastMatchingCode = c;
                matchingCount++;
            }
        }
        
        if(matchingCount == 1)
        { reverseOpcodeMap[lastMatchingCode] = s.op.code; }
    }
    
    unsigned *opcodeMap = malloc(sizeof(unsigned) * 16);
    for(int i = 0; i < 16; i++)
    {
        int c = reverseOpcodeMap[i];
        opcodeMap[c] = i;
    }
    return opcodeMap;
}

int numberOfSamplesMatching3Plus(sample *samples, int samplesCount)
{
    int threePlusCount = 0;
    for(int i = 0; i < samplesCount; i++)
    {
        int matchingCount = 0;
        sample s = samples[i];
        for(int c = 0; c < 16; c++)
        {
            unsigned r[4];
            memcpy(r, s.rb, sizeof(unsigned) * 4);
            
            executeOperation(s.op, c, r);
            if(!memcmp(r, s.ra, sizeof(unsigned) * 4))
            { matchingCount++; }
            
            if(matchingCount == 3)
            {
                threePlusCount++;
                break;
            }
        }
    }
    return threePlusCount;
}

#pragma mark -
#pragma mark Main Function
#pragma mark

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day16 <INPUT>\n");
        exit(0);
    }
    
    //Initialize samples
    sample *samples = malloc(sizeof(sample));
    int samplesCount = 0;
    
    //Initialize operations
    operation *ops = malloc(sizeof(operation));
    int opsCount = 0;
    
    //Open input file
    FILE *file = fopen(argv[1], "r");
    
    //Parse samples
    sample s;
    while(fscanf(file, "Before: [%u, %u, %u, %u]\n", &s.rb[0], &s.rb[1], &s.rb[2], &s.rb[3]) == 4 &&
          fscanf(file, "%u %u %u %u\n", &s.op.code, &s.op.a, &s.op.b, &s.op.c) == 4 &&
          fscanf(file, "After:  [%u, %u, %u, %u]\n", &s.ra[0], &s.ra[1], &s.ra[2], &s.ra[3]) == 4)
    {
        samplesCount++;
        samples = realloc(samples, sizeof(sample) * samplesCount);
        samples[samplesCount - 1] = s;
    }
    
    //Skip separator
    while(fgetc(file) == '\n');
    fseek(file, -sizeof(char), SEEK_CUR);
    
    //Parse operations
    operation op;
    while(fscanf(file, "%u %u %u %u", &op.code, &op.a, &op.b, &op.c) == 4)
    {
        opsCount++;
        ops = realloc(ops, sizeof(operation) * opsCount);
        ops[opsCount - 1] = op;
    }
    
    //Close input file
    fclose(file);
    
    //Count samples matching 3+ operations
    int threePlusCount = numberOfSamplesMatching3Plus(samples, samplesCount);
    printf("part1: samples matching 3+ ops = %d\n", threePlusCount);
    
    //Create opcode map
    unsigned *opcodeMap = createOpcodeMap(samples, samplesCount);
    
    //Execute operations
    unsigned r[4] = { 0, 0, 0, 0 };
    for(int i = 0; i < opsCount; i++)
    {
        operation op = ops[i];
        executeOperation(op, opcodeMap[op.code], r);
    }
    printf("part2: value of register 0 = %u\n", r[0]);
    
    //Cleanup
    free(opcodeMap);
    free(ops);
    free(samples);
    return 0;
}
