#include<stdio.h>
#include<string.h>
int main(void){
    char str[1005];
    int pos,len;
    if (fgets(str, sizeof(str), stdin)==NULL)return 0;

    size_t n = strlen(str);
    if (n>0 && str[n-1]=='\n') str[n-1]='\0';

    scanf("%d %d",&pos,&len);
    for (int i = pos; i< pos + len && str[i] != '\0'; i++)
        {putchar(str[i]);}
    putchar('\n');
    return 0;
    }
