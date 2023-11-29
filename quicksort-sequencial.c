#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "timer.h"

// Funções para o Quick Sort
void troca(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int particiona(int vet[], int inicio, int fim)
{
    // pivot randomico
    srand(time(NULL));
    int p = inicio + rand() % (fim - inicio + 1);
    troca(&vet[inicio], &vet[p]);

    int pivot = vet[inicio];
    int i = inicio;
    int j = fim;

    while (i < j)
    {
        while (i < fim && vet[i] <= pivot)
        {
            i++;
        }
        while (j > inicio && vet[j] >= pivot)
        {
            j--;
        }
        if (i < j)
        {
            troca(&vet[i], &vet[j]);
        }
    }
    vet[inicio] = vet[j];
    vet[j] = pivot;
    return j;
}

void quickSort(int vet[], int inicio, int fim)
{
    if (inicio < fim)
    {
        int pivot = particiona(vet, inicio, fim);
        quickSort(vet, inicio, pivot - 1);
        quickSort(vet, pivot + 1, fim);
    }
}

// Fluxo principal
int main(int argc, char *argv[])
{
    int *vetor;
    long long int tam;                                     // tamanho do vetor
    FILE *descritorArquivoEntrada, *descritorArquivoSaida; // descritores dos arquivos
    size_t retEntrada, retSaida;                           // retorno da funcao de leitura nos arquivos
    double inicio, fim, delta;                             // gerenciadores de tempo

    // -- INICIALIZAÇÃO -- //

    // recebe os argumentos de entrada
    if (argc < 3)
    {
        fprintf(stderr, "Digite: %s <arquivo vetor entrada> <arquivo vetor saida>\n", argv[0]);
        return 1;
    }

    // abre o arquivo para leitura binaria do vetor de entrada
    descritorArquivoEntrada = fopen(argv[1], "rb");
    if (!descritorArquivoEntrada)
    {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 2;
    }

    // le o tamanho do vetor de entrada
    retEntrada = fread(&tam, sizeof(long long int), 1, descritorArquivoEntrada);
    if (!retEntrada)
    {
        fprintf(stderr, "Erro de leitura do tamanho do vetor de entrada\n");
        return 2;
    }

    // aloca memoria para o vetor
    vetor = (int *)malloc(sizeof(int) * tam);
    if (!vetor)
    {
        fprintf(stderr, "Erro de alocao da memoria do vetor de entrada\n");
        return 2;
    }

    // carrega o vetor de elementos inteiros do arquivo de entrada
    retEntrada = fread(vetor, sizeof(int), tam, descritorArquivoEntrada);
    if (retEntrada < tam)
    {
        fprintf(stderr, "Erro de leitura dos elementos do vetor de entrada\n");
        return 2;
    }

    // -- ORDENAÇÃO -- //

    GET_TIME(inicio);

    // algoritmo quick Sort
    quickSort(vetor, 0, tam - 1);

    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo de ordenação: %lf\n", delta);

    // -- FINALIZAÇÃO -- //

    // abre o arquivo para escrita binaria
    descritorArquivoSaida = fopen(argv[2], "wb");
    if (!descritorArquivoSaida)
    {
        fprintf(stderr, "Erro de abertura do arquivo de saida\n");
        return 3;
    }

    // escreve o tamanho do vetor de saída
    retSaida = fwrite(&tam, sizeof(long long int), 1, descritorArquivoSaida);

    // escreve os elementos da matriz
    retSaida = fwrite(vetor, sizeof(int), tam, descritorArquivoSaida);
    if (retSaida < tam)
    {
        fprintf(stderr, "Erro de escrita no arquivo de saida\n");
        return 3;
    }

    // finaliza o uso das variaveis
    fclose(descritorArquivoEntrada);
    fclose(descritorArquivoSaida);
    free(vetor);

    return 0;
}
