//
//  day03.c
//  day03
//
//  Created by Brandon Holland on 2018-12-01.
//  Copyright © 2018 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct claim claim;
struct claim
{
    int id;
    int x;
    int y;
    int w;
    int h;
};

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day03 <INPUT>\n");
        exit(0);
    }
    
    //Initialize variables
    claim *claims = malloc(sizeof(claim));
    int claimsCount = 0;
    int fabricWidth = 0;
    int fabricHeight = 0;
    
    //Parse claims and calculate fabric size
    FILE *file = fopen(argv[1], "r");
    claim newClaim;
    while(fscanf(file, "#%d @ %d,%d: %dx%d ",
         &newClaim.id,
         &newClaim.x,
         &newClaim.y,
         &newClaim.w,
         &newClaim.h) == 5)
    {
        //Add claim
        claimsCount++;
        claims = realloc(claims, sizeof(claim) * claimsCount);
        claims[claimsCount - 1] = newClaim;
        
        //Update min/max fabric size
        int requiredFabricWidth = newClaim.x + newClaim.w;
        int requiredFabricHeight = newClaim.y + newClaim.h;
        fabricWidth = MAX(fabricWidth, requiredFabricWidth);
        fabricHeight = MAX(fabricHeight, requiredFabricHeight);
    }
    fclose(file);
    
    //Initialize fabric
    int fabric[fabricWidth][fabricHeight];
    memset(fabric, 0, sizeof(int) * (fabricWidth * fabricHeight));
    
    //Populate fabric with claims
    for(int i = 0; i < claimsCount; i++)
    {
        claim claim = claims[i];
        for(int r = claim.y; r < (claim.y + claim.h); r++)
        {
            for(int c = claim.x; c < (claim.x + claim.w); c++)
            { fabric[c][r]++; }
        }
    }
    
    //Find total overlapping area on fabric
    int totalOverlapArea = 0;
    for(int r = 0; r < fabricHeight; r++)
    {
        for(int c = 0; c < fabricWidth; c++)
        {
            if (fabric[c][r] > 1)
            { totalOverlapArea++; }
        }
    }
    printf("part1: total overlapping area = %d\n", totalOverlapArea);
    
    //Find correct claim (no overlap on fabric)
    int correctClaimId = -1;
    for(int i = 0; i < claimsCount; i++)
    {
        claim claim = claims[i];
        
        int fabricMaxForClaim = 0;
        for(int r = claim.y; r < (claim.y + claim.h); r++)
        {
            for(int c = claim.x; c < (claim.x + claim.w); c++)
            { fabricMaxForClaim = MAX(fabricMaxForClaim, fabric[c][r]); }
        }
        
        if (fabricMaxForClaim == 1)
        {
            correctClaimId = claim.id;
            break;
        }
    }
    printf("part2: correct claim = %d\n", correctClaimId);
    
    //Cleanup and return
    free(claims);
    return 0;
}
