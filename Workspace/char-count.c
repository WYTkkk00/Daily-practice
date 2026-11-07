#include<stdio.h>
#include<string.h>
int main(){
    char s[100];
    fgets(s,100,stdin);
    s[strcspn(s, "\n")] = 0; 
    // Remove the newline character from the input string
    char c;
    scanf(" %c", &c);
    int count = 0;
    for(int i = 0; s[i] != '\0'; i++){
        if(s[i] == c)
            count++;
    }
    printf("%d\n", count);
    return 0;
}