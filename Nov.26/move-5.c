#include<stdio.h>
int main(){
    char s[10];
    int i,m;
    scanf("%5s %d",s,&m);
    for(int i=m; i < 5; i++){
        putchar(s[i]);
    }
    for(int i=0; i < m; i++){
        putchar(s[i]);
    }
    putchar('\n');
    
    return 0;
}