//
//  day11.c
//  day11
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

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day11 <INPUT>\n");
        exit(0);
    }
    
    //Parse
    FILE *file = fopen(argv[1], "r");
    fclose(file);
	
    //Cleanup
    return 0;
}
