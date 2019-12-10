#include <stdio.h>

#include <stdio.h>

int foo() {
    printf("im in foo\n");
    return 1;
}

int foobar() {
    printf("im in foobar\n");
    return 2;
}

int bar(int i) {
    printf("im in bar\n");
    if (i)  {
        int b = foo();
        return 1 + b;
    } else {
        int c = foobar();
        return 2 + c;
    }
}

int main() {
    int d;
    printf("input: ");
    scanf("%d", &d);
    printf("%d\n", bar(d));
    return 0;
}

void log_msg(char* msg) {
    fprintf(stderr, "%s\n", msg);
}

void log_call_count(int cnt) {
    fprintf(stderr, "CallInst called %d times\n", cnt);
}
