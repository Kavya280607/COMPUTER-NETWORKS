int* maxSlidingWindow(int* nums, int numsSize, int k, int* returnSize)
{
    int *ans = (int*)malloc(sizeof(int)*(numsSize-k+1));
    int dq[numsSize];
    int front=0,rear=-1;
    int i;
    *returnSize=0;
    for(i=0;i<numsSize;i++)
    {
        while(front<=rear && dq[front]<=i-k)
            front++;
        while(front<=rear && nums[dq[rear]]<nums[i])
            rear--;
        dq[++rear]=i;
        if(i>=k-1)
            ans[(*returnSize)++]=nums[dq[front]];
    }
    return ans;
}