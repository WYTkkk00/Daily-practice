#include<stdio.h>
int main(){
    int n, s = 0;
    int term = 0;
    scanf("%d",&n);

    for(int i = 1;i<=n;i++){
        term = term + i;
        s = s + term;
    }
    printf("%d",s);
    return 0;
}