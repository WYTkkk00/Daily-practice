#include<stdio.h>
#include<math.h>
    int main(){
        double e;
        scanf("%lf",&e);
            double term = 1.0;    // 第0项 = 1（也就是 1）
            double sum  = 1.0;    // 累加和，从第0项开始
            double pi   = 2.0;    // 当前 π 近似值
            double prev;    // 上一次 π
            int n;
            for(n = 1; n < 1000000; n++) {
                term *= (double)n/(double)(2.0*n+1); // 计算第n项
                sum += term;                              // 累加和
                pi = 2.0 * sum;                           // 当前 π 近似值
                if(fabs(pi - prev) < e) {                 // 判断是否满足精度要求
                    break;
                }
                prev = pi;                                // 更新上一次 π
            }
        printf("%d %.7f",n+1,pi);
        return 0;
    }