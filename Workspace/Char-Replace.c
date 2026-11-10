#include<stdio.h>
int main(){
    char i[100],j[100],k[100];
    fgets(i, sizeof(i),stdin);
    fgets(j, sizeof(j),stdin);
    fgets(k, sizeof(k),stdin);

    i[strcspn(i, "\n")] = '\0';
    j[strcspn(j, "\n")] = '\0';
    k[strcspn(k, "\n")] = '\0';

    char *p = strstr(i, j);
    if (p != NULL){
        printf("偏移 = %d\n", (int)(p - i));
    }else{
        printf("没找到\n");
    }

    printf("[%s][%s][%s]\n",i,j,k);
    return 0;
}