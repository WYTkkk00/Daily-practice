#include<stdio.h>
int main(){
    int a[7];
    int i,j,t;

    for (i = 0; i < 7; i++){
        scanf("%d", &a[i]);
    }

    for ( i = 0; i < 6; i++){
        for (j = 0; j < 6 - i; j++){
            if(a[j] > a[j+1]){
                t = a[j];
                a[j] = a[j+1];
                a[j+1] = t;
            }
        }
    }

    printf("%d\n", a[3]);

    return 0;
}