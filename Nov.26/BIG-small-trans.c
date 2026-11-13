#include<stdio.h>
#include<ctype.h>
int main(){
    char s[105];
    fgets(s,105,stdin);
    for( int i = 0; s[i] != '\0'; i++){
        if(isupper(s[i]))
            s[i] = tolower(s[i]);
        else if(islower(s[i]))
            s[i] = toupper(s[i]);
        else
            s[i] = s[i];putchar(s[i]);    
    }
    putchar('\n');
    return 0;
}