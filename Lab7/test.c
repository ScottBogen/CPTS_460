/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <string.h>

void tokenize(char* pathname);

int *name[32];

int main() {
    char pathname[32] = "abc/def/hij";
    tokenize(pathname);

    int i = 0;
    while (name[i]) {
	printf("%s\n", name[i]);
	i++;
    }


    return 0;
}

void tokenize(char* pathname) {
    char* str;
    int i = 0;

    printf("tokenize pathname = %s\n", pathname);

    str = strtok(pathname, "/");

    while (str != NULL) {
        name[i++] = str;
        str = strtok(NULL, "/");
    }
}

// if a low rank process receives a value from a high rank process, it will not store the value
// global = value received + local


/*
    Parallel prefix(A[1...n]) {
        Let rank denote my rank
        L = A[rank]   // local array
        G = L         // global array, initially the same as L

        for (i = 0 to (lg p - 1)) {
          // If i'm rank 101 (5), my neighbors are 001, 111, and 100
          // mate = toggle the bits of my rank in binary

        }

    }




*/
