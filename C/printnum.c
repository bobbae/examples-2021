#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


#define HEXNUMS "0123456789ABCDEF"
int tohex(int numbers[], int numlen, int base, char sign, char *outbuf)
{
    int i = 0;
    int j = 0;
    int n, num;

    if(sign != ' '){
        outbuf[i++] = sign;
    }

    while (j < numlen) {
        num = numbers[j++];
        n = num >> 4;
        outbuf[i++] = HEXNUMS[n % base];
        n = num & 0xf;
        outbuf[i++] = HEXNUMS[n % base];
    }

    outbuf[i] = 0;
}

int main() {
    char outbuf[128];
    int nums[] = { 0xde, 0xad, 0xbe, 0xef, 0x01, 
                    0x02, 0x9e, 0xef, 0x34, 0x87, 
                    0x23, 0x12};

    tohex(nums, sizeof(nums)/sizeof(int), 16, ' ', outbuf);
    printf("%s", outbuf);
}