#include<stdio.h>
#include<math.h>
int main(){
    float a,v;
    scanf("%f %f",&a,&v);
    float length = v*v/(2*a);
    printf("%.2f\n", length);
    return 0;
}