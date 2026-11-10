#include<stdio.h>
int main(void){
    int k;
    scanf("%d",&k);

    char s[26];
    int out[26] = {0};
    for(int i = 0;i < 26; i++) s[i] = 'A' + i;
    
    int i = 0, count = 0, remain = 26;
    while (remain > 1) {
        if (out[i] == 0) count++;
        if (count == k) {
            printf("%c", s[i]);
            out[i] = 1;
            count = 0;
            remain--;
        }
        i = (i + 1) % 26;
    }
    printf("\n");
    return 0;
}