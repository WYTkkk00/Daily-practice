#include<stdio.h>
#include<string.h>
int main(){
    char s[100];
    int n;
    fgets(s, sizeof s, stdin);
    s[strcspn(s,"\r\n")] ='\0';
    scanf("%d",&n);
        char len = strlen(s);

    for(int i = n; i < len;i++)  {
        putchar(s[i]);
    }  
    for(int i = 0; i < n;i++) {
        putchar(s[i]);
    }
    putchar('\n');
    return 0;
}