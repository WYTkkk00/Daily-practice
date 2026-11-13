#include <stdio.h>

int main(void) {
    int n, m;
    scanf("%d %d", &n, &m);

    int a[5][5];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &a[i][j]);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if ((i + j) % 2 == 0)
                printf("%3d", a[i][j]);
            else
                printf("   ");
        }
        printf("\n");
    }
    return 0;
}
