#include<stdio.h>
int main(){
    char s[100];
    int cnt[26] = {0};

    scanf("%s", s);
    for(int i=0; s[i] != '\0'; i++){
    
    if(s[i] >= 'a' && s[i] <= 'z'){
        cnt[s[i] - 'a']++;
    }
    else if(s[i] >= 'A' && s[i] <= 'Z'){
        cnt[s[i] - 'A']++;
    }
}
    int max = cnt[0];
    int pos = 0;
    for(int i=1; i<26; i++){
        if(cnt[i] > max){
            max = cnt[i];
            pos = i;
        }
    }
    printf("%c\n", 'A' + pos);
    return 0;
}