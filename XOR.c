#include <stdlib.h>
#include <stdbool.h>

int cmp(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

int binarySearch(int arr[], int n, int key)
{
    int l = 0, r = n - 1;

    while (l <= r)
    {
        int mid = (l + r) / 2;

        if (arr[mid] == key)
            return 1;
        else if (arr[mid] < key)
            l = mid + 1;
        else
            r = mid - 1;
    }

    return 0;
}

int findMaximumXOR(int* nums, int numsSize)
{
    int max = 0;
    int mask = 0;

    for (int bit = 30; bit >= 0; bit--)
    {
        mask |= (1U << bit);

        int *prefix = (int *)malloc(numsSize * sizeof(int));

        for (int i = 0; i < numsSize; i++)
            prefix[i] = nums[i] & mask;

        qsort(prefix, numsSize, sizeof(int), cmp);

        int candidate = max | (1U << bit);
        bool found = false;

        for (int i = 0; i < numsSize; i++)
        {
            if (binarySearch(prefix, numsSize, prefix[i] ^ candidate))
            {
                found = true;
                break;
            }
        }

        if (found)
            max = candidate;

        free(prefix);
    }

    return max;
}