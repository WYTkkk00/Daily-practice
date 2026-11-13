#include<stdio.h>
int main(){
    int score1[5], score2[5];
    scanf("%d %d %d %d %d %d %d %d %d %d",
         &score1[0], &score1[1], &score1[2], &score1[3], &score1[4],
         &score2[0], &score2[1], &score2[2], &score2[3], &score2[4]);
    int sum1 = 0, sum2 = 0;
    sum1 = score1[0] + score1[1] + score1[2] + score1[3] + score1[4];
    sum2 = score2[0] + score2[1] + score2[2] + score2[3] + score2[4];
   
        double variance1 = 0, variance2 = 0;
        for(int i=0; i<5; i++){
            variance1 += (score1[i] - sum1/5.0) * (score1[i] - sum1/5.0);
        }
        for(int i=0; i<5; i++){    
            variance2 += (score2[i] - sum2/5.0) * (score2[i] - sum2/5.0);
        }
            variance1 /= 5.0;
            variance2 /= 5.0;
            double avg1 = sum1 / 5.0;
            double avg2 = sum2 / 5.0;

        if(sum1 > sum2){
            printf("A,%.2f", avg1);
        }
        else if(sum1 < sum2){
            printf("B,%.2f", avg2);
        }
        else{
            if(variance1 < variance2){
                printf("A,%.2f", avg1);
            }
            else if(variance1 > variance2){
                printf("B,%.2f", avg2);
            }
            else{
                printf("A,%.2f", avg1);
            }
        }

    return 0;
}