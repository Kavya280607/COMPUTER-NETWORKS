int findComplement(int num) {
    unsigned int mask=1;
    while(mask<=num)
    {
        mask=mask<<1;
    }
    mask=mask-1;
    return mask^num;
}