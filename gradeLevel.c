#include<stdio.h>
int main(void){
    double score[5];
    int cnt[6] = {0};
    for(int i=0;i<5;i++){  
        scanf("%lf",&score[i]);
        if(score[i]>=90) cnt[1]++;
        else if(score[i]>=80) cnt[2]++;
        else if(score[i]>=70) cnt[3]++;
        else if(score[i]>=70) cnt[4]++;
        else if(score[i]>=50) cnt[5]++;
        else cnt[6]++;
    }


    return 0;
}