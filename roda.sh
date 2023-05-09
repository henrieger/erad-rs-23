#!/bin/bash

DATA_DIR=./data

SIZES=(128 256 1024 4096)
SAMPLES=(32 128 256)

# Compila se não foi compilado ainda
make -B &> /dev/null

# Conjuntos de dados simulados
MNIST=(70000 784)
IMAGENET=(1274219 128)
DATASETS=(MNIST IMAGENET)
for DATASET in ${DATASETS[@]}
do
    mkdir $DATA_DIR/$DATASET
done

# configuracao do hostfile
MACHINES=(j1 j5 j6 j7)
SLOTS=(1 2 4)
for SLOT in ${SLOTS[@]}
do
    rm -f hostfile_${SLOT}.txt
    for MACHINE in ${MACHINES[@]}
    do
        echo "$MACHINE slots=$SLOT" >> hostfile_${SLOT}.txt
    done
done

# remove arquivos de dados anteriores
# for DATASET in ${DATASETS[@]}
# do
#     rm -f $DATA_DIR/$DATASET/*.dat
# done

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
            FILE=$DATA_DIR/$DATASET/${DATASET}_${SIZE_Q}_${SAMPLE}.dat
            for i in $(seq 0 29)
            do
                echo -e "\t $i - SEQ"
                mpirun -np 1 ./knn $SIZE_P $SIZE_Q $SAMPLE $DIMENSIONS 2> /dev/null | tr -d "\n" >> $FILE
                for SLOT in ${SLOTS[@]}
                do
                    echo -e "\t $i - CLUSTER 4x$SLOT"
                    mpirun --hostfile hostfile_${SLOT}.txt ./knn $SIZE_P $SIZE_Q $SAMPLE $DIMENSIONS 2> /dev/null | tr -d "\n" >> $FILE
                done

                echo "" >> $FILE

            done
        done
    done
done
