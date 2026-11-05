#include<stdio.h>
    int main(){
        int n;
        scanf("%d",&n);
for(int i = 0; i <= n ; i++){
    for(int j = 0; j < n-i; j++){
    printf(" ");
}
for(int k = 0; k < i; k++)
    printf("%c",'A'+k);
for(int l = i-2; l >= 0; l--)
    printf("%c",'A'+l);
printf("\n");
}
for(int i = n-1; i >= 1 ; i--){
for(int j = 0; j < n-i; j++)printf(" ");

    for(int k = 0; k < i; k++)
    printf("%c",'A'+k);

    for(int k = i-2; k >= 0; k--)
    printf("%c",'A'+k);
    
printf("\n");
}
return 0;}
    