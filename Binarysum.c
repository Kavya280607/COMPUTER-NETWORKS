#include <stdio.h>
#include <string.h>
int main() {
    char a[100], b[100], result[101];
    int i, j, k = 0, carry = 0;
    printf("Enter first binary number: ");
    scanf("%s", a);
    printf("Enter second binary number: ");
    scanf("%s", b);
    i = strlen(a) - 1;
    j = strlen(b) - 1;
    while (i >= 0 || j >= 0 || carry) {
        int sum = carry;
        if (i >= 0) sum += a[i--] - '0';
        if (j >= 0) sum += b[j--] - '0';
        result[k++] = (sum % 2) + '0';
        carry = sum / 2;
    }
    result[k] = '\0';
    printf("Sum = ");
    for (i = k - 1; i >= 0; i--)
        printf("%c", result[i]);
    printf("\n");
    return 0;
}
