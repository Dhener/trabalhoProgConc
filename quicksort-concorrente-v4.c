#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"
#include <time.h>

#define MAX 1000

//-- Variáveis globais
long long int numero_de_elementos;
int *elementos, numero_de_threads;

int inicializarVetores(int opcao, char *argv[])
{

    int *vetor;
    FILE *descritorArquivo; // descritor do arquivo de saida
    size_t ret;

    // aloca memoria para o vetor
    vetor = (int *)malloc(sizeof(int) * numero_de_elementos);
    if (!vetor)
    {
        printf("Erro de alocao da memoria do vetor\n");
        return 2;
    }

    srand(time(NULL));
    if (opcao == 1)
    {
        for (long int j = numero_de_elementos; j > 0; j--)
        {
            vetor[numero_de_elementos - j] = j;
            // printf("%d\n", elementos[j]);
            vetor[numero_de_elementos - j] = j;
        }
    }
    else if (opcao == 2)
    {
        for (long int j = 0; j < numero_de_elementos; j++)
        {
            vetor[j] = j + 1;
            // printf("%d\n", elementos[j]);
            vetor[j] = j + 1;
        }
    }
    else
    {
        for (long int j = 0; j < numero_de_elementos; j++)
        {
            int temp = rand() % MAX;
            vetor[j] = temp;
            // printf("%d\n", elementos[j]);
            vetor[j] = temp;
        }
    }
    descritorArquivo = fopen(argv[4], "wb");
    if (!descritorArquivo)
    {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 3;
    }
    // escreve o tamanho do vetor
    ret = fwrite(&numero_de_elementos, sizeof(long long int), 1, descritorArquivo);
    // escreve os elementos do vetor
    ret = fwrite(vetor, sizeof(float), numero_de_elementos, descritorArquivo);
    if (ret < numero_de_elementos)
    {
        fprintf(stderr, "Erro de escrita no  arquivo\n");
        return 4;
    }

    // finaliza o uso das variaveis
    fclose(descritorArquivo);
    free(vetor);
    return 0;
}

int partition(int vetor[], int inicio, int fim)
{
    //--Escolha de um pivô aleatório
    int pivo_aleatorio = inicio + rand() % (fim - inicio + 1);
    // printf("inicio: %d / pivo: %d / fim: %d\n", inicio, pivo_aleatorio, fim);
    //--Fazendo a troca e Colocando o pivô para o início do vetor
    int temp1 = vetor[inicio];
    vetor[inicio] = vetor[pivo_aleatorio];
    vetor[pivo_aleatorio] = temp1;

    int i = inicio;
    int j = fim;
    int pivo = vetor[inicio];

    //--Separa os números maiores e menores que o pivo
    while (i < j)
    {
        while (i < fim && vetor[i] <= pivo)
        {
            i++;
        }
        while (j > inicio && vetor[j] >= pivo)
        {
            j--;
        }
        if (i < j)
        {
            int temp2 = vetor[i];
            vetor[i] = vetor[j];
            vetor[j] = temp2;
        }
    }

    //--Após a separação, como o j no final conterá o índice de um elemento
    //--Menor que o pivo, faremos a troca novamente
    vetor[inicio] = vetor[j];
    vetor[j] = pivo;

    //--Retorna o índice onde está o pivô
    return j;
}

void Quick(int vetor[], int inicio, int fim)
{
    if (inicio < fim)
    {
        int indice_pivo = partition(vetor, inicio, fim);
        Quick(vetor, inicio, indice_pivo - 1);
        Quick(vetor, indice_pivo + 1, fim);
    }
}

void ordene(int vet[], int inicio, int meio, int fim)
{
    // criando um vetor temporario e copiando o vetor original
    int *vetor_temporario;
    vetor_temporario = (int *)malloc(sizeof(int) * (fim - inicio));
    if (vetor_temporario == NULL)
    {
        printf("--ERRO: malloc()\n");
        exit(-1);
    }
    // int vetor_temporario[fim - inicio];

    int i = inicio;
    int j = meio;
    int pos = 0;

    while (i < meio && j < fim)
    {
        if (vet[i] <= vet[j])
        {
            vetor_temporario[pos++] = vet[i++];
        }
        else
        {
            vetor_temporario[pos++] = vet[j++];
        }
    }

    while (i < meio)
    {
        vetor_temporario[pos++] = vet[i++];
    }

    while (j < fim)
    {
        vetor_temporario[pos++] = vet[j++];
    }

    for (i = inicio; i < fim; i++)
    {
        vet[i] = vetor_temporario[i - inicio];
    }

    free(vetor_temporario);
}

void juntarPequenosBlocos(int vet[], int inicio, int meio, int fim)
{
    if (fim <= numero_de_elementos)
    {
        ordene(vet, inicio, meio, fim);
        juntarPequenosBlocos(vet, inicio, fim, fim + numero_de_elementos / numero_de_threads);
    }
    else if (numero_de_elementos - meio > 0)
    {
        ordene(vet, inicio, meio, numero_de_elementos);
    }
}

// funcao executada pelas threads
void *Sort(void *arg)
{
    int thread_id = *(int *)arg;
    int inicio = (thread_id) * (numero_de_elementos / numero_de_threads);
    int fim = (thread_id + 1 == numero_de_threads) ? numero_de_elementos - 1 : (thread_id + 1) * (numero_de_elementos / numero_de_threads) - 1;
    // printf("inicio: %d / fim: %d\n", inicio, fim);
    Quick(elementos, inicio, fim);

    pthread_exit(NULL);
}

// funcao principal do programa
int main(int argc, char *argv[])
{
    FILE *descritorArquivoEntrada, *descritorArquivoSaida; // descritores dos arquivos
    size_t retEntrada, retSaida;
    // recebe os argumentos de entrada
    if (argc != 6)
    {
        printf("Usa: %s <numero de threads> <numero de elementos> <opcao> <arquivo vetor entrada> <arquivo vetor saida>\n", argv[0]);
        // printf("Usa: %s <arquivo vetor entrada> <arquivo vetor saida> <numero threads>\n", argv[0]);
        return 1;
    }

    //--Obtem argumentos do terminal
    numero_de_threads = atoi(argv[1]);
    numero_de_elementos = atoi(argv[2]);
    int opcao = atoi(argv[3]);
    inicializarVetores(opcao, argv);
    // abre o arquivo para leitura binaria do vetor de entrada
    descritorArquivoEntrada = fopen(argv[4], "rb");
    if (!descritorArquivoEntrada)
    {
        printf("Erro de abertura do arquivo\n");
        return 2;
    }

    // le o tamanho do vetor de entrada
    retEntrada = fread(&numero_de_elementos, sizeof(long long int), 1, descritorArquivoEntrada);
    if (!retEntrada)
    {
        printf("Erro de leitura do tamanho do vetor de entrada\n");
        return 2;
    }

    // aloca memoria para o vetor
    elementos = (int *)malloc(sizeof(int) * numero_de_elementos);
    if (!elementos)
    {
        printf("Erro de alocao da memoria do vetor de entrada\n");
        return 2;
    }

    // carrega o vetor de elementos inteiros do arquivo de entrada
    retEntrada = fread(elementos, sizeof(int), numero_de_elementos, descritorArquivoEntrada);
    if (retEntrada < numero_de_elementos)
    {
        printf("Erro de leitura dos elementos do vetor de entrada\n");
        return 2;
    }

    //--Aloca espaco para os identificadores das threads
    pthread_t threads[numero_de_threads];
    int threadsIds[numero_de_threads];

    //--Inicializa variáveis de tempo
    double temp_inicio, temp_fim, delta;

    //--Tomada de tempo inicial
    GET_TIME(temp_inicio);

    /* Cria as threads */
    for (int i = 0; i < numero_de_threads; i++)
    {
        threadsIds[i] = i;
        if (pthread_create(&threads[i], NULL, Sort, (void *)&threadsIds[i]))
        {
            printf("ERRO -- pthread_create\n");
        }
    }

    //--Espera todas as threads terminarem
    for (int i = 0; i < numero_de_threads; i++)
    {
        if (pthread_join(threads[i], NULL))
        {
            printf("ERRO: pthread_join() \n");
            exit(-1);
        }
    }

    //--Faz a junção dos blocos ordenados por cada thread e os ordena
    //--Não é necessário chamar o mergeSort com 1 thread porque equivale ao quicksort normal
    if (numero_de_threads > 1)
    {
        juntarPequenosBlocos(elementos, 0, numero_de_elementos / numero_de_threads, 2 * (numero_de_elementos / numero_de_threads));
    }

    //--Tomada de tempo final
    GET_TIME(temp_fim);

    //--Tempo decorrido do código
    delta = temp_fim - temp_inicio;
    printf("Tempo Concorrente: %lf\n", delta);

    // abre o arquivo para escrita binaria
    descritorArquivoSaida = fopen(argv[5], "wb");
    if (!descritorArquivoSaida)
    {
        printf("Erro de abertura do arquivo de saida\n");
        return 3;
    }

    // escreve o tamanho do vetor de saída
    retSaida = fwrite(&numero_de_elementos, sizeof(long long int), 1, descritorArquivoSaida);

    // escreve os elementos da matriz
    retSaida = fwrite(elementos, sizeof(int), numero_de_elementos, descritorArquivoSaida);
    if (retSaida < numero_de_elementos)
    {
        printf("Erro de escrita no arquivo de saida\n");
        return 3;
    }

    // finaliza o uso das variaveis
    fclose(descritorArquivoEntrada);
    fclose(descritorArquivoSaida);
    free(elementos);

    pthread_exit(NULL);
}
