#include <stdio.h>

int main(void) {
    double v[5];
    int i;
    for (i = 0; i < 5; i++) scanf("%lf", &v[i]);

    double mn = v[0], mx = v[0];
    for (i = 1; i < 5; i++) {
        if (v[i] < mn) mn = v[i];
        if (v[i] > mx) mx = v[i];
    }

    for (i = 0; i < 5; i++) {
        double y;
        if (mx == mn) y = 0.0;            // 防止除零
        else          y = (v[i] - mn) / (mx - mn);

        if (i > 0) putchar(' ');          // 数字间一个空格
        printf("%.6f", y);
    }
    putchar('\n');
    return 0;
}
