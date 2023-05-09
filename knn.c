# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <time.h>
# include <mpi.h>
# include "verificaKNN.h"
# include "chrono.c"

# define NUM_PARAMETROS 5

void getParameters(int argc, char **argv, int np, int *n, int *nq, int *k, int *d)
{
    if (argc != NUM_PARAMETROS)
    {
        perror("Numero incorreto de parametros");
        exit(-1);
    }

    *n = atoi(argv[1]);
    *nq = atoi(argv[2]);
    *k = atoi(argv[3]);
    *d = atoi(argv[4]);

    if (*k > *n)
    {
        perror("Amostra nao pode ser maior que o conjunto");
        exit(-1);
    }

    if (*nq % np)
    {
        perror("Tamanho do vetor de pontos precisa ser multiplo do numero de processos");
        exit(-1);
    }
}

float distanciaQuadrada(float *p, float *q, int d)
{
    float dist = 0;
    for (int i = 0; i < d; i++)
    {
        dist += (p[i] - q[i]) * (p[i] - q[i]);
    }

    return dist;
}

float **geraConjuntoDadosVazio(int nc, int d)
{
    float **c = malloc(nc * sizeof(float *));
    float *data = malloc(nc * d * sizeof(float));
    if (!c || !data)
    {
        perror("Impossivel gerar conjunto de pontos");
        if (c) free(c);
        if (data) free(data);
        exit(-1);
    }
    
    for (int i = 0; i < nc; i++)
        c[i] = data + d*i;

    return c;
}

float **geraConjuntoDeDados(int nc, int d)
{
    float **c = geraConjuntoDadosVazio(nc, d);

    for (int i = 0; i < nc; i++)
        for (int j = 0; j < d; j++)
            c[i][j] = (float) (random() % 100);

    return c;
}

void destroiConjuntoDeDados(float **c)
{
    free(c[0]);
    free(c);
}

int **geraMatrizResultados(int nq, int k)
{
    int **r = malloc(nq * sizeof(int *));
    int *data = malloc(nq * k * sizeof(int));

    if (!r || !data)
    {
        perror("Impossivel alocar matriz de resultado");
        if (r) free(r);
        if (data) free(data);
        exit(-1);
    }

    for (int i = 0; i < nq; i++)
        r[i] = data + k*i;

    return r;
}

void destroiMatrizResultados(int **r)
{
    free(r[0]);
    free(r);
}

void achaResultadosChunk(float **p, float **chunk, int **r, int n, int chunkSize, int d, int k)
{
    float *distancias = alloca(k * sizeof(float));
    if (!distancias)
    {
        perror("Impossivel alocar vetor de distancias");
        exit(-1);
    }
    float maiorDist = 0;
    int maiorInd = 0;

    for (int i = 0; i < chunkSize; i++)
    {
        for (int j = 0; j < k; j++)
        {
            distancias[j] = distanciaQuadrada(p[j], chunk[i], d);

            # ifdef DEBUG
                printf("%1.0f\t", distancias[j]);
            # endif

            r[i][j] = j;
            if (distancias[j] > maiorDist)
            {
                maiorDist = distancias[j];
                maiorInd = j;
            }
        }

        for (int j = k; j < n; j++)
        {
            float dist = distanciaQuadrada(p[j], chunk[i], d);

            # ifdef DEBUG
                printf("%1.0f\t", dist);
            # endif

            if (dist < maiorDist)
            {
                distancias[maiorInd] = dist;
                r[i][maiorInd] = j;
                maiorDist = 0;
                for (int l = 0; l < k; l++)
                {
                    if (distancias[l] > maiorDist) {
                        maiorDist = distancias[l];
                        maiorInd = l;
                    }
                }
            }
        }

        # ifdef DEBUG
            printf("\n");
        # endif 
    }
}

int calculateChunkSize(int rank, int nq, int maxChunkSize)
{
    int overflowChunk = (rank+1) * maxChunkSize;
    return overflowChunk <= nq ? maxChunkSize : nq - (rank * maxChunkSize);
}

void achaResultados(int np, int rank, float **p, float **q, int **r, int n, int nq, int d, int k)
{
    // int maxChunkSize = (nq-1) / np + 1;
    // int chunkSize = calculateChunkSize(rank, nq, maxChunkSize);
    int chunkSize = nq / np;

    float **chunk = geraConjuntoDadosVazio(chunkSize, d);
    int **res = geraMatrizResultados(chunkSize, k);

    MPI_Scatter(q[0], chunkSize*d, MPI_FLOAT, chunk[0], chunkSize*d, MPI_FLOAT, 0, MPI_COMM_WORLD);
    achaResultadosChunk(p, chunk, res, n, chunkSize, d, k);
    MPI_Gather(res[0], chunkSize*k, MPI_INT, r[0], chunkSize*k, MPI_INT, 0, MPI_COMM_WORLD);

    destroiConjuntoDeDados(chunk);
    destroiMatrizResultados(res);
}

int main(int argc, char **argv)
{
    // srandom(time(NULL));
    srandom(777);

    MPI_Init(&argc, &argv);

    int np;
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int n, nq, k, d;
    getParameters(argc, argv, np, &n, &nq, &k, &d);    

    float **p, **q;
    p = geraConjuntoDeDados(n, d);
    q = geraConjuntoDeDados(nq, d);

    int **r = geraMatrizResultados(nq, k);

    # ifdef DEBUG
    if (rank == 0)
    {
        printf("-- Conjunto P --\n");
        for (int i = 0; i < n; i++)
        {
            // printf("%d: ", i);
            for (int j = 0; j < d; j++)
                printf("%1.0f\t", p[i][j]);
            printf("\n");
        }
        printf("\n");

        printf("-- Conjunto Q --\n");
        for (int i = 0; i < nq; i++)
        {
            // printf("%d: ", i);
            for (int j = 0; j < d; j++)
                printf("%1.0f\t", q[i][j]);
            printf("\n");
        }
        printf("\n");

        printf("-- Distâncias --\n");
    }
    # endif

    chronometer_t chrono;
    chrono_reset(&chrono);

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) chrono_start(&chrono);

    achaResultados(np, rank, p, q, r, n, nq, d, k);

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) chrono_stop(&chrono);

    # ifdef DEBUG
    if (rank == 0)
    {
        printf("-- Conjunto R --\n");
        for (int i = 0; i < nq; i++)
        {
            // printf("%d: ", i);
            for (int j = 0; j < k; j++)
                printf("%d\t", r[i][j]);
            printf("\n");
        }
    }
    # endif

    # ifdef VERIFICA
    if (rank == 0)
        verificaKNN(q[0], nq, p[0], n, d, k, r[0]);
    # endif

    if (rank == 0)
    {
        double time = chrono_gettotal(&chrono) / 1000000000.0;
        printf("%lf ", time);
    }

    destroiConjuntoDeDados(p);
    destroiConjuntoDeDados(q);
    destroiMatrizResultados(r);

    MPI_Finalize();

    return 0;
}
