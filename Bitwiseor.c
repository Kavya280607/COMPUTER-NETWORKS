#include <stdio.h>
int main() {
    int n, a[100];
    int result[10000];
    int count = 0;
    printf("Enter number of elements: ");
    scanf("%d", &n);
    printf("Enter array elements:\n");
    for (int i = 0; i < n; i++)
        scanf("%d", &a[i]);
    for (int i = 0; i < n; i++) {
        int OR = 0;
        for (int j = i; j < n; j++) {
            OR |= a[j];
            int found = 0;
            for (int k = 0; k < count; k++) {
                if (result[k] == OR) {
                    found = 1;
                    break;
                }
            }
            if (!found)
                result[count++] = OR;
        }
    }
    printf("Distinct BitwiseOR values:\n");
    for (int i = 0; i < count; i++)
        printf("%d ", result[i]);
    printf("\nCount = %d\n", count);
    return 0;
}
