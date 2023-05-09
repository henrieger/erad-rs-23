#!/bin/bash

DATA_DIR=./data

SIZES=(128 256 1024 4096)
SAMPLES=(32 128 256)

# Compila se não foi compilado ainda
make -B knn_seq &> /dev/null

# Conjuntos de dados simulados
MNIST=(70000 784)
IMAGENET=(1274219 128)
DATASETS=(MNIST IMAGENET)
for DATASET in ${DATASETS[@]}
do
    mkdir $DATA_DIR/$DATASET
done

# laco principal
for DATASET in ${DATASETS[@]}
do
    eval SIZE_P=\${$DATASET[0]}
    eval DIMENSIONS=\${$DATASET[1]}

    for SIZE_Q in ${SIZES[@]}
    do
        for SAMPLE in ${SAMPLES[@]}
        do
            echo Rodando $DATASET NQ = $SIZE_Q K = $SAMPLE
            FILE=$DATA_DIR/$DATASET/${DATASET}_${SIZE_Q}_${SAMPLE}_SEQ.dat
            for i in $(seq 0 29)
            do
                echo -e "\t $i - SEQ"
                ./knn_seq $SIZE_P $SIZE_Q $SAMPLE $DIMENSIONS 2> /dev/null | tr -d "\n" >> $FILE
            done
        done
    done
done
