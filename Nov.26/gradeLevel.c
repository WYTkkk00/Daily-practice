#include<stdio.h>
int main(void){
    double score[5];
    int cnt[6] = {0};
    int target;

    for(int i = 0;i < 5; i++)
    scanf("%lf",&score[i]);
    scanf("%d",&target);
    for(int i=0;i<5;i++){  
        
        if(score[i]>=90)      cnt[1]++;
        else if(score[i]>=80) cnt[2]++;
        else if(score[i]>=70) cnt[3]++;
        else if(score[i]>=60) cnt[4]++;
        else                  cnt[5]++;
    }
    printf("%d\n",cnt[target]);

    return 0;
}