#include<stdio.h>
#include<math.h>
    int main(){
        int flag,s;
        double item,pi,e;
        scanf("%lf",&e);
        flag=s=1;
        item=1;
        pi=0;
        while(fabs(item=flag*1.0/s)>e)
        {
            pi=pi+item;
            flag=-flag;
            s=s+2;

        }    
        pi=pi*4;
        printf("%lf",pi);
        return 0;
    }