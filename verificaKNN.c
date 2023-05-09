# include "verificaKNN.h"

# include <stdio.h>

void verificaKNN( float *Q, int nq, float *P, int n, int D, int k, int *R ) {
    // note que R tem nq linhas por k colunas, para qualquer tamanho de k (colunas)
    // entao é linearizado para acesso como um VETOR
    printf( " ------------ VERIFICA KNN (v2) --------------- \n" );
    for( int linha=0; linha<nq; linha++ ) {
        printf( "knn[%d]: ", linha );
        for( int coluna=0; coluna<k; coluna++ )
            printf( "%d ", R[ linha*k+coluna ] );
        printf( "\n" );
    }
}
