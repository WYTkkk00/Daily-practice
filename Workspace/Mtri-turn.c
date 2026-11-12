#include<stdio.h>
int main(){
    int n,i,j;
    int a[10][10];
    scanf("%d\n",&n);
    if(n < 2 || n > 10){return 0;}

    for(i = 0; i < n; i++){
        for(j = 0; j < n; j++){
            scanf("%d", &a[i][j]);
        }
    }
    

    for(i = 0;i < n; i++){
        for(j = 0; j < n; j++){
            int k = a[n - 1 - j][n - 1 -i];
            printf("%3d",k);
        }
        printf("\n");
    }

    return 0;
}