#include<stdio.h>
#include<string.h>
int main(){
    char i[100],j[100],k[100];
    char result[300] = "";
    fgets(i, sizeof(i),stdin);
    fgets(j, sizeof(j),stdin);
    fgets(k, sizeof(k),stdin);

    i[strcspn(i, "\r\n")] = '\0';
    j[strcspn(j, "\r\n")] = '\0';
    k[strcspn(k, "\r\n")] = '\0';

    if (strlen(j) == 0){ printf("%s\n", i); return 0;}

    char *p = i;
    char *found;
    int pos = 0;


    while ((found = strstr(p,j)) != NULL){
        int gap = found - p;
        memcpy(result + pos ,p, gap);
        pos += gap;

        strcpy(result + pos ,k);
        pos += strlen(k);

        p = found + strlen(j);
    }
    strcpy(result + pos, p);

    printf("%s\n", result);
    return 0;
}