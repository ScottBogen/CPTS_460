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

