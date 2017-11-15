#include <time.h>
#include <stdlib.h>
#include <stdio.h>

unsigned long mix(unsigned long a, unsigned long b, unsigned long c);

void main(void) {
    srand(mix(clock(), time(NULL), 12789057));
    int cnt = (rand()/31%6) + 7;
    printf("%d\n", cnt);
    
    printf("0.0.0.0/0 N/A\n");

    int mask = 0;
    int i = 1;
    while (i < cnt) {
        int r = rand();
        if ((mask&1   ) == 0 && (r&1   ) != 0 && i < cnt) { printf("186.0.0.0/8 LACNIC\n"); mask|=1   ; i++; }
        if ((mask&2   ) == 0 && (r&2   ) != 0 && i < cnt) { printf("146.0.0.0/8 Various Registries (Maintained by ARIN)\n"); mask|=2   ; i++; }
        if ((mask&4   ) == 0 && (r&4   ) != 0 && i < cnt) { printf("146.164.0.0/16 UFRJ\n"); mask|=4   ; i++; }
        if ((mask&8   ) == 0 && (r&8   ) != 0 && i < cnt) { printf("146.164.3.7/32 acd.ufrj.br\n"); mask|=8   ; i++; }
        if ((mask&16  ) == 0 && (r&16  ) != 0 && i < cnt) { printf("146.164.3.198/32 ct4.ct.ufrj.br\n"); mask|=16  ; i++; }
        if ((mask&32  ) == 0 && (r&32  ) != 0 && i < cnt) { printf("146.164.3.3/32 ava.ufrj.br\n"); mask|=32  ; i++; }
        if ((mask&64  ) == 0 && (r&64  ) != 0 && i < cnt) { printf("146.164.41.0/24 DCC-UFRJ\n"); mask|=64  ; i++; }
        if ((mask&128 ) == 0 && (r&128 ) != 0 && i < cnt) { printf("146.164.41.220/32 Maquina do Claudson\n"); mask|=128 ; i++; }
        if ((mask&256 ) == 0 && (r&256 ) != 0 && i < cnt) { printf("3.0.0.0/8 GE\n"); mask|=256 ; i++; }
        if ((mask&512 ) == 0 && (r&512 ) != 0 && i < cnt) { printf("18.72.0.3/32 name server: bitsy.mit.edu\n"); mask|=512 ; i++; }
        if ((mask&1024) == 0 && (r&1024) != 0 && i < cnt) { printf("18.62.0.0/24 eecs.mit\n"); mask|=1024; i++; }
        // if ((mask&2048) == 0 && (r&2048) != 0 && i < cnt) { printf("\n"); mask|=2048; i++; }
        // if ((mask&4096) == 0 && (r&4096) != 0 && i < cnt) { printf("\n"); mask|=4096; i++; }
    }

    int cnt2 = (rand()/31%4) + 2;
    
    mask = 0;
    i = 1;
    while (i < cnt2) {
        int r = rand();
        
        if ((mask&256 ) == 0 && (r&256 ) != 0) { printf("3.10.12.45\n"); mask|=256 ; i++; }
        else if ((mask&1   ) == 0 && (r&1   ) != 0) { printf("186.192.1.8\n"); mask|=1   ; i++; }
        else if ((mask&512 ) == 0 && (r&512 ) != 0) { printf("18.72.0.3\n"); mask|=512 ; i++; }
        else if ((mask&1024) == 0 && (r&1024) != 0) { printf("18.62.0.100\n"); mask|=1024; i++; }
        else if ((mask&8192) == 0 && (r&8192) != 0) { printf("186.192.10.15\n"); mask|=8192; i++; }

        if ((mask&2   ) == 0 && (r&2   ) != 0) { printf("145.100.9.80\n"); mask|=2   ; i++; }
        else if ((mask&4096) == 0 && (r&4096) != 0) { printf("146.160.8.8\n"); mask|=4096; i++; }
        else if ((mask&2048) == 0 && (r&2048) != 0) { printf("146.164.80.10\n"); mask|=2048; i++; }
        else if ((mask&4   ) == 0 && (r&4   ) != 0) { printf("146.164.1.18\n"); mask|=4   ; i++; }
        else if ((mask&8   ) == 0 && (r&8   ) != 0) { printf("146.164.3.7\n"); mask|=8   ; i++; }
        else if ((mask&16  ) == 0 && (r&16  ) != 0) { printf("146.164.3.198\n"); mask|=16  ; i++; }
        else if ((mask&32  ) == 0 && (r&32  ) != 0) { printf("146.164.3.3\n"); mask|=32  ; i++; }
        else if ((mask&64  ) == 0 && (r&64  ) != 0) { printf("146.164.41.33\n"); mask|=64  ; i++; }
        else if ((mask&128 ) == 0 && (r&128 ) != 0) { printf("146.164.41.220\n"); mask|=128 ; i++; }
    }
}

unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}