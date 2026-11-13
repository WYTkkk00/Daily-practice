#include<stdio.h>
#include<string.h>
int main(){
    char digits[100] = {0};
    char others[100] = {0};
    int di = 0, oi = 0;
    char s[100];


    fgets(s, sizeof(s),stdin);
    s[strcspn(s, "\n")] = '\0';


    scanf("%s", s);
    for(int i=0; s[i] != '\0'; i++){
        if(s[i] >= '0' && s[i] <= '9'){
            digits[di++] = s[i];;
        }

        else{
            others[oi++] = s[i];
        }
    
    }
    int num = 0;
    for (int j = 0; j < di; j++){
        num = num * 10 + (digits[j] - '0');
    }

    printf("%d\n",num * 2);
    printf("%s\n", others);
    return 0;
}