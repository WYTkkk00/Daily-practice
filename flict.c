#include<stdio.h>
void reverse(int n)
{
    if(n<10)
        printf("%d",n);
    else
    {
        printf("%d",n%10);
        reverse(n/10);
    }
}
    




int main(){
    int num;
    scanf("%d",&num);
    reverse(num);
}