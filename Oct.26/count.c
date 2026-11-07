#include <stdio.h>
int main()
{
    int n;
    scanf("%d", &n);
    int a = n / 100;       // 取出百位数字
    int b = n / 10 % 10;  // 取出十位数字
    int c = n % 10;       // 取出个位数字
    int reverse_num = c * 100 + b * 10 + a;
    printf("%d", reverse_num);
    return 0;
}
