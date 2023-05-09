# Algoritmo KNN paralelo em cluster com MPI
**Henrique L. Rieger -** `hlr19@inf.ufpr.br`
**Wagner M. Nunan Zola -** `wagner@inf.ufpr.br`
**Departamento de Informática - Universidade Federal do Paraná**

Este repositório corresponde ao código e aos dados do projeto de mesmo nome, realizado como Iniciação Científica e apresentado na ERAD/RS 2023.

## Setup
É necessário ter instalada a biblioteca Open MPI em seu sistema. Para isso, rode os comandos abaixo:

```bash
sudo apt update && sudo apt upgrade
sudo apt install openmpi-common openmpi-bin
```

## Rodar o projeto
Para rodar as versões com MPI, digite apenas `./roda.sh` no terminal. Para executar a versão sequencial, digite `./roda_seq.sh`. Lembre-se de configurar os scripts para que o MPI acesse as máquinas corretas!

## Dados
Os dados se encontram no diretório `data`, divididos pelas bases de dados que simulam. Cada coluna nos arquivos corresponde a uma configuração de processos: 1, 4, 8 e 16 processos, respectivamente.
