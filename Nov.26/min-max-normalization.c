#include<stdio.h>
#define MAX 1
#define MIN 0
int main() {
    int a,b,c,d,e;
    scanf("%d %d %d %d %d",&a,&b,&c,&d,&e);
    double x, x_min, x_max, x_norm;
    x_min = (double)(a < b ? (a < c ? (a < d ? (a < e ? a : e) : (d < e ? d : e)) : (c < d ? (c < e ? c : e) : (d < e ? d : e))) : (b < c ? (b < d ? (b < e ? b : e) : (d < e ? d : e)) : (c < d ? (c < e ? c : e) : (d < e ? d : e))));
    x_max = (double)(a > b ? (a > c ? (a > d ? (a > e ? a : e) : (d > e ? d : e)) : (c > d ? (c > e ? c : e) : (d > e ? d : e))) : (b > c ? (b > d ? (b > e ? b : e) : (d > e ? d : e)) : (c > d ? (c > e ? c : e) : (d > e ? d : e))));
    for(int i = 0; i < 5; i++) {
        if(i == 0) x = a;
        else if(i == 1) x = b;
        else if(i == 2) x = c;
        else if(i == 3) x = d;
        else x = e;
        x_norm = (x - x_min)/(x_max - x_min);
        if(i > 0) putchar(' ');
        printf("%.6f", x_norm);
    }
    putchar('\n');
    return 0;
}