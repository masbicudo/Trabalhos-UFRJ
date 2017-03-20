#include <stdio.h>
#include <stdlib.h>

#define DEBUG

int main() {

    int count;
    scanf("%d ", &count);

    char* s;
    size_t length = 100;
    s = (char*)malloc(length * sizeof(char));

    if(s == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }

    int it;
    for (it = 0; it < count; it++) {
        size_t lenread = getline(&s, &length, stdin);

        if (feof(stdin))
            break;

        while (s[lenread-1] == '\n' || s[lenread-1] == '\r')
            s[--lenread] == '\0';
        if (lenread > 0) {
#ifdef DEBUG
            printf("SUBREDE: %s", s);
#endif
        }
    }

    while(1) {
        size_t lenread = getline(&s, &length, stdin);

        if (feof(stdin))
            break;

        while (s[lenread-1] == '\n' || s[lenread-1] == '\r')
            s[--lenread] == '\0';
        if (lenread > 0) {
#ifdef DEBUG
            printf("IP: %s", s);
#endif
        }
    }

    free(s);
    return 0;
}
