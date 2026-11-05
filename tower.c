#include<stdio.h>

int gcd(int a, int b) {
    while (b != 0) {
        int temp = a%b;
        a=b;
        b = temp;
    }
    return a;
}
int main() {
    int m,n;
    scanf("%d %d", &m, &n);
if(gcd(m,n)==1)
    printf("yes");
else
    printf("no");
    return 0;
}
