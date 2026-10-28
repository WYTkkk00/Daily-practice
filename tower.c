#include<stdio.h>
int main() {
    int a, b, c;
    int first_flag = 1;
    scanf("%d %d", &a, &b);
    for(c = a; c <= b; c++){
        // 核心条件：能被3整除 且 不能被5整除
        if(c % 3 == 0 && c % 5 != 0){
            if(first_flag){
                printf("%d", c);
                first_flag = 0;
            }else{
                printf(" %d", c);
            }
        }
    }
    return 0;
}
