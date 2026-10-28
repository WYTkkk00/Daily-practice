#include<stdio.h>
#include<math.h>
int main()
{
    double price1,price2;
    int num1,num2;
    scanf("%lf %d",&price1,&num1);
    scanf("%lf %d",&price2,&num2);
    printf("Item 1: Price = %.2f, Quantity = %d, Total = %.2f\n", price1, num1, price1 * num1);
    printf("Item 2: Price = %.2f, Quantity = %d, Total = %.2f\n", price2, num2, price2 * num2);
    return 0;
}