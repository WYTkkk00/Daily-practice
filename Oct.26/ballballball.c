#include <stdio.h>
int main(){
    int a,b,c,m;
    scanf("%d %d %d %d",&a,&b,&c,&m);
    int found = 1;
    for(int i=0;i<=a;i++){
        for(int j = 0;j<=b&&i+j<=m;j++)
        
        {
            int k = m-i-j;
            if(k <= c)
            {
                printf("%d-%d-%d\n",i,j,k);
                found = 1;
            }
        }
        }
if(!found)printf("None");
return 0;
}