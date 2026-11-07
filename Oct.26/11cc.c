#include<stdio.h>
int main()
{
    long int fac(int n);
    int n,m;
    scanf("%d%d",&n,&m);
    if (n>m)
        printf("%ld\n",fac(n)/(fac(m)*fac(n-m)));
    else
        printf("no\n");    



    return 0;
}

long int fac(int n)
{
    long int i,fact=1;
    for(i=1;i<=n;i++)
        fact*=i;
    return fact;
}