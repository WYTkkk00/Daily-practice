#include<stdio.h>
int main(){
    int n,i,j;
    scanf("%d",&n);
    
    if (n < 2 || n > 25) {   /* 超出范围默认为 5 */
        n = 5;
    }

    for(i = 0; i < n; i++){
            for(j = 0; j < n; j++){
                int d = i - j;
                if (d<0) d = -d;
                int v = d + 1;
                printf("%3d",v);
            }
            printf("\n");
        }
    


    return 0;
}