#include <stdio.h>

int main() {
    int n;
    scanf("%d", &n);

    // 上半部分（含中间行）
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j < n - i; j++) printf(" ");

        for (int k = 0; k < i; k++)          // 升序
            printf("%c", 'A' + k);
        for (int k = i - 2; k >= 0; k--)     // 降序
            printf("%c", 'A' + k);

        printf("\n");
    }

    // 下半部分
    for (int i = n - 1; i >= 1; i--) {
        for (int j = 0; j < n - i; j++) printf(" ");

        for (int k = 0; k < i; k++)
            printf("%c", 'A' + k);
        for (int k = i - 2; k >= 0; k--)
            printf("%c", 'A' + k);

        printf("\n");
    }

    return 0;
}
