#include <stdio.h>

int main(){
    int a, b, c, t;
    scanf("%d %d %d", &a, &b, &c);

    if(a > b){ t = a; a = b; b = t; }   // 保证 a <= b
    if(a > c){ t = a; a = c; c = t; }   // 保证 a <= c
    if(b > c){ t = b; b = c; c = t; }   // 保证 b <= c

    printf("%d %d %d\n", a, b, c);
    return 0;
}
