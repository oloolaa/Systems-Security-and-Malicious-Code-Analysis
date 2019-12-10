# include <stdio.h>
void qsort(int a[], int left, int right);
void log_msg(char* msg);
void log_call_count(int cnt);

/*  qsort: sort v[left]...v[right] in increasing order  */
int main(void)
{
    printf("Input 5 numbers delimited by Enter('\\n'):\n");
    int array[5];
    for (int i=0; i<5; i++) {
        scanf("%d", &array[i]);
    }

    qsort(array, 0, 4);
    printf("sorted outputs:\n");
    for (int i=0; i<5; i++) {
        printf("%d\n", array[i]);
    }
    return 0;
}

void qsort(int a[], int left, int right)
{
    int i, last;
    void swap(int v[], int i, int j);
    
    if (left >= right) {
        return;
    }
    swap(a, left, (left+right)/2);
    last = left;
    for (i = left + 1; i <= right; i++) {
        if (a[i] < a[left]) {
            
            swap(a, ++last, i);
        }
    }
    swap(a, last, left);
    qsort(a, left, last-1);
    qsort(a, 1+last, right);
}


void swap(int v[], int i, int j)
{
    int temp;
    temp = v[i];
    v[i] = v[j];
    v[j] = temp;
}

void log_msg(char* msg) {
	fprintf(stderr, "%s", msg);
}

void log_call_count(int cnt) {
	fprintf(stderr, "CallInst called %d times\n", cnt);
}


