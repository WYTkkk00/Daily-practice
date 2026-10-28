#include<stdio.h>
int main(){
    const double PI = 3.1415926;
    float r,h;
    scanf("%f %f",&r,&h);
    float volume = PI*r*r*h;
    float area = 2*PI*r*h + 2*PI*r*r;
    printf("%.4f,%.4f\n", volume, area);
    return 0;
}