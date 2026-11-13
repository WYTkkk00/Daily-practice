#include<stdio.h>

    void decToBinary(int n){
    int bin [32];
    int i = 0;
    while (n > 0) {
        bin[i] = n % 2;
        n = n / 2;
        i++;
    }
    for (int k = i; k < 8;k++) {
        printf("0");
    }
    for (int j = i - 1; j >= 0; j--) {
        printf("%d", bin[j]);
    }
}
int main(void){
    int n;
    scanf("%d", &n);
    decToBinary(n);
    printf("\n");
    return 0;

}