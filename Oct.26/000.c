#include<stdio.h>

int main()
{
    int m,n,r;
    int gcd_recusion(int m,int n);
    scanf("%d%d",&m,&n);
    r=gcd_recusion(m,n);
    printf("gcd=%d\n",r);


    return 0;
}


int gcd_recusion(int m,int n)
{
    int k;
    k=m%n;
    if(k==0)
        return n;
    else{
            m=n;
            n=k;
        return gcd_recusion(n,k);
    }



}