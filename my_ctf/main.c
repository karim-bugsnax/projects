#include <unistd.h>

static long make_stuff(long a, long b, long c, long d,
            long e, long f, char secret[8]) {
    long xx = a;
    long yy = f;

    return xx + yy;
}

#define MSG "you won't find my secret\n"

__attribute__((weak))
void extract(void) {
    write(STDOUT_FILENO, MSG, sizeof(MSG) - 1);
}

int main(void) {
    char secret[8] = {"iloveasm"};
    make_stuff(42, 45, 48, 49, 'c', 'd', secret);
    extract();
    return 0;
}