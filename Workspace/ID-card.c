#include <stdio.h>

int main(void)
{
    /* 1. 准备两张表：权重表、校验码表 */
    int weight[17] = {7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2};
    char check[11] = "10X98765432";   /* 下标 0..10 对应 M 值 */

    int n;
    scanf("%d", &n);

    int badCount = 0;        /* 记录有问题的号码个数 */

    for (int k = 0; k < n; k++) {
        char id[19];
        scanf("%17s", id);   /* 用字符串读，防止溢出 */

        /* 2. 前 17 位加权求和 */
        int sum = 0;
        for (int i = 0; i < 17; i++) {
            sum += (id[i] - '0') * weight[i];
        }

        /* 3. 取模得 M，查表得应有的第 18 位 */
        int m = sum % 11;
        char expect = check[m];

        /* 4. 与实际的第 18 位比较 */
        if (id[17] != expect) {
            printf("%s\n", id);
            badCount++;
        }
    }

    /* 5. 全部合格时输出提示 */
    if (badCount == 0) {
        printf("All passed\n");
    }

    return 0;
}
