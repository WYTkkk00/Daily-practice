#include <stdio.h>

int main(void)
{
    int weight[17] = {7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2};
    char check[11] = "10X98765432";

    int n;
    scanf("%d", &n);

    int badCount = 0;

    for (int k = 0; k < n; k++) {
        char id[19];
        scanf("%17s", id);

        int sum = 0;
        for (int i = 0; i < 17; i++) {
            sum += (id[i] - '0') * weight[i];
        }

        int m = sum % 11;
        char expect = check[m];

        if (id[17] != expect) {
            printf("%s\n", id);
            badCount++;
        }
    }

    if (badCount == 0) {
        printf("All passed.\n");
    }

    return 0;
}
