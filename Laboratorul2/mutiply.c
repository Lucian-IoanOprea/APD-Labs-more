#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int NReps;
int printLevel;
int N;
int P;
int** a;
int** b;
int** c;

void* threadFunction(void *args)
{
    // Implement parallel multiply of matrices: C = A * B
    // using P threads by splitting the outer loop.

    int tid     = *(int*)args;
    int start   = tid * ceil((double)N / P);
    int end     = MIN(N, (tid + 1) * ceil((double)N / P));

    int i, j, k, t, tmp;

    for (t = 0; t < NReps; ++t)
    {
        for (i = start; i < end; i++)
        {
            for (j = 0; j < N; j++)
            {
                tmp = 0;

                for (k = 0; k < N; k++)
                {
                    tmp += a[i][k] * b[k][j];
                }

                c[i][j] = tmp;
            }
        }
    }
    
    return NULL;
}

void getArgs(int argc, char **argv)
{
    if (argc < 5)
    {
        printf("Not enough paramters: ./program N NReps printLevel P\n");
        exit(1);
    }

    N = atoi(argv[1]);
    NReps = atoi(argv[2]);
    printLevel = atoi(argv[3]);
    P = atoi(argv[4]);
}

void init()
{
    a = malloc(sizeof(int *) * N);
    b = malloc(sizeof(int *) * N);
    c = malloc(sizeof(int *) * N);

    if (a == NULL || b == NULL || c == NULL)
    {
        printf("malloc failed!");
        exit(1);
    }

    int i, j;

    for (i = 0; i < N; i++)
    {
        a[i] = malloc(sizeof(int) * N);
        b[i] = malloc(sizeof(int) * N);
        c[i] = malloc(sizeof(int) * N);

        if(a[i] == NULL || b[i] == NULL || c[i] == NULL)
        {
            printf("malloc failed!");
            exit(1);
        }
    }

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {            
            if (i <= j)
            {
                a[i][j] = 1;
                b[i][j] = 1;
            } else
            {
                a[i][j] = 0;
                b[i][j] = 0;
            }
        }
    }
}

void destroy()
{
    int i, j;

    for (i = 0; i < N; ++i)
    {
        free(a[i]);
        free(b[i]);
        free(c[i]);
    }

    free(a);
    free(b);
    free(c);
}

void printAll()
{
    int i, j;
    
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            printf("%i\t",c[i][j]);
        }
        printf("\n");
    }
}

void printPartial()
{
    printAll();
}

void print()
{
    if (printLevel == 0)
    {
        return;
    } else if (printLevel == 1)
    {
        printPartial();
    } else 
    {
        printAll();
    }
}

int main(int argc, char *argv[])
{
    int i;
    getArgs(argc, argv);
    init();

    pthread_t tid[P];
    int thread_id[P];

    for (i = 0;i < P; i++)
    {
        thread_id[i] = i;
    }

    for (i = 0; i < P; i++)
    {
        pthread_create(tid + i, NULL, threadFunction, thread_id + i);
    }

    for (i = 0; i < P; i++)
    {
        pthread_join(tid[i], NULL);
    }

    print();
    destroy();

    return 0;
}