#include<stdio.h>
int main(){
FILE *fp = fopen("in.txt", "r");
    if (fp == NULL) {
        return 0;          /* 文件打不开就直接返回 */
    }
    char line[100];        /* 每行不超过 80 字符，留余量 */
    int count = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        count++;
    }

    fclose(fp);
    printf("%d\n", count);
    
    return 0;
}