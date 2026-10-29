#include<stdio.h>
    int main() {
        int n;
        int sum = 0;
        scanf("%d",&n);
        for(int s = 1; s <= n; s++) {
            int term = 0;
            for (int j = 1;j <= s; j++){
                term += j;

            }
            sum += term;
        }
printf("%d",sum);









        return 0;

    }