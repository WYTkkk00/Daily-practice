#include <stdio.h>
int main()
{
    int x;
    int start = -1;      // 当前连续段的起点
    int prev  = -1;      // 上一个读到的数
    int first = 1;       // 是否是第一段（控制逗号）

    while (scanf("%d", &x) == 1 && x != -1)
    {
        if (start == -1)         // 第一段开始
        {
            start = prev = x;
        }
        else if (x == prev + 1)  // 和上一个连续
        {
            prev = x;
        }
        else                     // 断开，先输出上一段
        {
            if (!first) printf(",");
            if (start == prev) printf("%d", start);
            else                printf("%d-%d", start, prev);
            first = 0;
            start = prev = x;    // 新段的起点
        }
    }

    // 循环结束后，把最后一段吐出来（最容易漏的一步）
    if (start != -1)
    {
        if (!first) printf(",");
        if (start == prev) printf("%d", start);
        else                printf("%d-%d", start, prev);
    }

    return 0;
}
