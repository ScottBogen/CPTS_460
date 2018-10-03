
#include <stdio.h>

int main()
{
    printf("Hello World\n");

    //                          i                pos
    int arr[10] = { 1,  2,  3,  5,  6,  7}; //, '\0 '
    int value = 4;

    //           i  pos
    // {1, 2, 3, 5, 5, 6, 7}



    // final product: {1,2,3,4,5,6,7}
    //
    for (int i = 0; i < 10; i++)  {
        if (value < arr[i]) {
            int pos = i;

            printf("insert at pos %d\n", pos);
            // go to end of list
            while (arr[pos] != '\0') {
                pos++;
            }

            printf("end of list at pos %d\n", pos);

            // loop until back at i
            while (pos>i) {
                arr[pos] = arr[pos-1];
                pos--;
            }
            printf("back at pos %d\n", pos);

            // not until after accounted for
            arr[i] = value;
            break;
        }
    }

    int i = 0;
    while (arr[i] != '\0') {
        printf("%d\t", arr[i]);
        i++;
    }
    printf("\n");

    return 0;
}
