/*                       PROJETO DE ESTRUTURA DE DADOS
    Esse código se trata de uma simulação de vôos dos aviões em um aeroporto.
Por meio de interação com usuário através do arquivo "entrada.txt", o código
irá imprimir diferentes resultados em todas simulações, a simulação contém 
algumas situações adversas que mudará o rumo do avião. No código foi implementado
fila na inserção do avião na pista e lista para os aviões decolando, tornando 
possível pousar avião que chegou ao destino por razões de distância ou velocidade.
    Para melhorar a expêriencia do usuário, o IA requisitado pelo projeto foi 
improvisado com função rand que irá gerar um número "sortido" através do cálculo 
com desempenho da máquina onde o código está sendo compilado, podendo assim
criar situações randômica na simulação. 
    Quando finalizado o código libera memórias alocadas durante a simulação.
    As intruções dos parâmetros e comandos da simulação estão contidas no
"relatório.pdf". Por favor seguir as instruções para que não haja problemas
ao compilar.
     
    Código C feito por aluno:
    Giovanni Verardi Tacchini   2169088
    João Lucas Biglia Lobo      2022770
    Bruno Souza                 2165236  

Att.
28 de Novembro de 2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/* 

    Definindo as estruturas de avião e da fila que terá nome TCabeca

*/

typedef struct aviao{
    int codigo;
    char modelo[10];
    char destino[50];
    int distancia;
    int tempo;
    float latitude;
    float longitude;
    float altura;
    float velocidade;           //Da Linha 38 até 46 são características atribuídas ao avião;
    int estado;                 //Aqui definirá o estado do avião: 0-Esperando, 1-Voando, 2-Aterrissando;
    struct aviao *prox;         //Para indicar o próximo avião, criando cadeia estruturas conectadas.
}aviao;

typedef struct cabeca{          //Criando fila.
	aviao *inicio;
	aviao *fim;
}TCabeca;

/* 

  Chamando as funções no escopo do código que serão futuramentes utilizados respectivamentes paramêtros 
  que a função retornará.

*/
TCabeca *criaCabeca();          
TCabeca **pistas(int qtd); 
void insere_aviao(TCabeca *p, int pista, int cod, char *modelo, char *destino, int dist, int temp);
TCabeca *decolar(TCabeca *p, TCabeca *ar); 
void iniciasimulacao(TCabeca *ar, int mov, int tempestade, int neblina, int turbulencia); 
void aviao_move(TCabeca *ar);
void aterrissando(aviao *av, TCabeca *ar);
void liberaCabeca(TCabeca *p);

//Únicas duas variáveis globais, para entrada do arquivo e saída do mesmo.
FILE *saida, *entrada;

/*

    Função principal onde:
    -Irá abrir o arquivo de entrada e imprimir um arquivo de saída.
    -Utiliza loop para ler os comandos do arquivo de entrada e chamar funções correspondentes.
    -Chama as funções de manipulação de nodos para simulação do aeroporto.
    -Libera a memória alocada antes de encerrar o programa.

*/
int main()
{
    int pista, qtd, temp, nebli, turbu;     //Criando váriaveis locais para servir de entradas nas funções chamadas quando necessário
    saida = fopen("saida.txt", "w");
    entrada = fopen("entrada.txt", "r");
    if (!saida || !entrada)
    {
        printf("/nERRO AO ABRIR O ARQUIVO/n");
        return 1;
    }                                      

    TCabeca *ar = criaCabeca();             
    TCabeca **p = NULL;
    char nomeFuncao[50];

    while (fscanf(entrada, "%s", nomeFuncao) == 1)
    {
        if (strcmp(nomeFuncao, "pistas") == 0)
        {
            fscanf(entrada, "%d", &qtd);
            p = pistas(qtd);
        }
        else if (strcmp(nomeFuncao, "insere_aviao") == 0)
        {
            int cod, dist, temp;
            char modelo[50], destino[50];
            fscanf(entrada, "%d %d %s %s %d %d", &pista, &cod, modelo, destino, &dist, &temp);
            insere_aviao(p[pista], pista, cod, modelo, destino, dist, temp);
        }
        else if (strcmp(nomeFuncao, "decolar") == 0)
        {
            fscanf(entrada, "%d", &pista);
            ar = decolar (p[pista], ar);
        }
        else if (strcmp(nomeFuncao, "tempestade") == 0)
        {
            fscanf(entrada, "%d", &temp);
        }
        else if (strcmp(nomeFuncao, "neblina") == 0)
        {
            fscanf(entrada, "%d", &nebli);
        }
        else if (strcmp(nomeFuncao, "turbulencia") == 0)
        {
            fscanf(entrada, "%d", &turbu);
        }
        else if (strcmp(nomeFuncao, "iniciasimulacao") == 0)
        {
            int mov;
            fscanf(entrada, "%d", &mov);
            iniciasimulacao(ar, mov, temp, nebli, turbu);
        }
        else if (strcmp(nomeFuncao, "fim") == 0)
        {
            for (int i = 1; i <= qtd; i++)
            {
                fprintf(saida, "\nLIBERANDO PISTA %d...\n", i);
                liberaCabeca(p[i]);
            }

            free(p);
            fprintf(saida, "\nLIBERANDO AVIOES VOANDO...\n");
            liberaCabeca(ar);

            fprintf(saida, "\nFECHANDO ARQUIVOS...\n");
            fclose(entrada);
            fclose(saida);
            return 0;
        }
        else
        {
            fprintf(saida, "!!!COMANDO NAO ENCONTRADO NO SISTEMA!!!");
        }
    }
    return 0;
}

void liberaCabeca(TCabeca *p)
{
    aviao *av = p->inicio;
    aviao *aux;
    while (av != NULL)
    {
        aux = av;
        av = av->prox;
        free(aux);
    }
    free(p);
}

/*

    Cria a quantidade de fila requisitada pelo usuário. Um vetor de filas (ex: aviao *pista[i]).
    Cada pista é inicializada chamando criaCabeça();

*/
TCabeca **pistas (int qtd)
{
    int i;
    TCabeca **novo = (TCabeca **)malloc(qtd * sizeof(TCabeca *));
    if(novo == NULL)
    {
        printf("ERRO AO ALOCAR A MEMORIA PARA PISTAS");
        exit;
    }
    for(i=1; i<=qtd; i++)
    {
        novo[i] = criaCabeca();
    }
    return novo;
}

/*

    Aloca e inicializa uma nova estrutura 'TCabeca'(cabeça da fila).

*/
TCabeca *criaCabeca()
{
	TCabeca *novo;
	novo= (TCabeca *)malloc(sizeof(TCabeca));
	if(novo)
	{
		novo->inicio=NULL;
		novo->fim=NULL;
		return novo;
	}
	return NULL;
}

/*

    Insere um avião novo no fim da fila representada por estrutura 'TCabeca' e aloca dinamicamente 
    um novo nó(avião), preenche os dados com a "entrada.txt" mais os valores já instaciado pelo
    código e o insere no final da fila.

*/
void insere_aviao(TCabeca *p, int pista, int cod, char *modelo, char *destino, int dist, int temp)
{
	aviao *novo = NULL;
	novo = (aviao *)malloc(sizeof(aviao));
	if(novo)
	{
		novo->codigo=cod;
		strcpy(novo->modelo, modelo);
		strcpy(novo->destino, destino);
		novo->distancia=dist;
		novo->tempo=temp;
		novo->prox=NULL;
		novo->latitude=0;
		novo->longitude=0;
		novo->altura=0;
		novo->velocidade=0;
		novo->estado=0;
		if(p->fim!=NULL)
			p->fim->prox=novo;
		p->fim=novo;
		if(p->inicio==NULL)
			p->inicio=novo;
	}
	else
        printf("ERRO AO ALOCAR MEMORIA");
	fprintf(saida, "\nNOVO AVIAO INSERIDO NA PISTA %d \nCODIGO: %d \nMODELO: %s \nDESTINO: %s \nDISTANCIA: %d \nTEMPO: %d\n", pista, novo->codigo, novo->modelo, novo->destino, novo->distancia, novo->tempo);
}

/*

    Função 'decolar' move o primeiro avião para a pista de decolagem, atribui valores através de cálculo com rand
    para latitude, longitude, altura e velocidade e finaliza alterando estado do avião para voando. 
    Ele remove avião do comeco da pista e insere no final da fila voando.
*/
TCabeca *decolar(TCabeca *p, TCabeca *ar)
{
    srand(time(NULL));
    aviao *decolando = p->inicio;

    if (decolando)
    {
        decolando->latitude = -22.9103;
        decolando->longitude = -43.1631;
        decolando->altura = (rand() % 3000) + 9000;
        decolando->velocidade = (rand() % 60) + 260;
        decolando->estado = 1;

        fprintf(saida, "\nDECOLANDO AVIAO %d\nLATITUDE: %f\nLONGITUDE: %f\nALTURA: %2.f\nVELOCIDADE: %2.f\n", decolando->codigo, decolando->latitude, decolando->longitude, decolando->altura, decolando->velocidade);

        p->inicio = decolando->prox;
        if (p->fim == decolando)
            p->fim = NULL;

        if (ar->fim != NULL)
            ar->fim->prox = decolando;
        else
            ar->inicio = decolando;

        ar->fim = decolando;
        decolando->prox = NULL;

        return ar;
    }
    else
    {
        printf("/nImpossivel decolar, nao ha mais avioes na pista/n");
        return ar;
    }
}

/*

    Função 'aviao_move' move os aviões na fila simulando seu deslocante durante a simulação,
    por meio de "for" ele verifica possíveis colisões quando as latitudes são iguais e ajusta
    se necessário. Caso avião já esteja próximo do aeroporto ele chama a função 'aterrisando()'

*/
void aviao_move(TCabeca *ar)
{
    aviao *colisao;
    aviao *aux = ar->inicio;
    int distanciaAux = aux->distancia;

    while(aux)
    {
        aux->distancia -= 40;
        aux->altura += rand() % 20 - 10; // valor entre -10 e 10
        aux->latitude += (rand() % 10 - 5) / 10.0; // valor entre -0.5 e 0.5
        aux->longitude += (rand() % 10 - 5) / 10.0; // valor entre -0.5 e 0.5
        aux->tempo -= 1;

        for(colisao = ar->inicio; colisao != NULL; colisao = colisao->prox)
        {
            if((aux->latitude == colisao->latitude) && (aux->longitude == colisao->longitude) && (aux->codigo != colisao->codigo))
            {
                fprintf(saida, "\nRISCO DE COLISAO - MUDANDO LATITUDE DO AVIAO %d\n", aux->codigo);
                aux->latitude += 1;
            }
        }

        if ((aux->distancia <= (distanciaAux * 0.1) || aux->tempo <= 0) && ar->inicio != NULL)
            {
                fprintf(saida,"\nAviao %d aterrissando!!!\n", aux->codigo);
                aux->estado = 2;
                aterrissando(aux, ar);
            }

        aux = aux->prox;
    }
}

/*

    Inicia simulação, alterando características de aviões em momentos especifícos.
    Altera altitude e velocidade de aviões em condições climáticas adversas(tempestade, neblina, turbulência).
    Aqui que se receberá as entradas do arquivo.txt para se realizar o requerido pelo usuário.

*/
void iniciasimulacao(TCabeca *ar, int mov, int tempestade, int neblina, int turbulencia)
{
    //Sorteia avião que irá passar por situações adversas
    srand(time(NULL));
    int i, aviaoSorteado, qtdAvioes = 0;
    aviao *aux = ar->inicio;
    aviao *tempestadeAux = ar->inicio;
    aviao *neblinaAux = ar->inicio;
    aviao *turbulenciaAux = ar->inicio;

    // Conta a quantidade de aviões
    while (aux != NULL)
    {
        qtdAvioes++;
        aux = aux->prox;
    }

    aux = ar->inicio;

    for (i = 0; i < mov; i++)
    {
        if (i == tempestade)
        {
            aviaoSorteado = rand() % qtdAvioes;
            aux = ar->inicio;

            if (aux != NULL)
            {
                for (int j = 0; j <= aviaoSorteado; j++)
                {
                    if (j == aviaoSorteado)
                    {
                        fprintf(saida,"\nTempestade no aviao %d na movimentacao %d Altura %2.f e Velocidade %2.f!", tempestadeAux->codigo, i, tempestadeAux->altura, tempestadeAux->velocidade);
                        tempestadeAux->altura -= 300;
                        tempestadeAux->velocidade = tempestadeAux->velocidade - (tempestadeAux->velocidade * 0.1);
                        fprintf(saida,"\nNova altura = %2.f Nova velocidade = %2.f\n", tempestadeAux->altura, tempestadeAux->velocidade);
                    }
                    else
                    {
                        tempestadeAux = tempestadeAux->prox;
                    }
                }
            }
        }


        if (i == neblina)
        {
            int aviaoSorteado = rand() % qtdAvioes;

            aux = ar->inicio;

            if (aux != NULL)
            {
                for (int j = 0; j <= aviaoSorteado; j++)
                {
                    if (j == aviaoSorteado)
                    {
                        fprintf(saida,"\nNeblina no aviao %d na movimentacao %d Latitude %f e Velocidade %2.f!", neblinaAux->codigo, i, neblinaAux->latitude, neblinaAux->velocidade);
                        neblinaAux->latitude -= 0.5;
                        neblinaAux->velocidade = neblinaAux->velocidade - (neblinaAux->velocidade * 0.05);
                        fprintf(saida,"\nNova latitude = %f Nova velocidade = %2.f\n", neblinaAux->latitude, neblinaAux->velocidade);
                    }
                    else
                    {
                        neblinaAux = neblinaAux->prox;
                    }
                }
            }
        }

        if (i == turbulencia)
        {
            int aviaoSorteado = rand() % qtdAvioes;

            aux = ar->inicio;

            if (aux != NULL)
            {
                for (int j = 0; j <= aviaoSorteado; j++)
                {
                    if (j == aviaoSorteado)
                    {
                        fprintf(saida,"\nTurbulencia no aviao %d na movimentacao %d Velocidade %2.f!", neblinaAux->codigo, i, neblinaAux->velocidade);
                        turbulenciaAux->velocidade = turbulenciaAux->velocidade - (turbulenciaAux->velocidade * 0.15);
                        fprintf(saida,"\nNova velocidade = %2.f\n", turbulenciaAux->velocidade);
                    }
                    else
                    {
                        turbulenciaAux = turbulenciaAux->prox;
                    }
                }
            }
        }
        if(ar->inicio)
            aviao_move(ar);
    }
}

/*

    Remove um avião da fila, indicando que está aterrisando. Libera a memória do nó removido.

*/
void aterrissando(aviao *av, TCabeca *ar)
{
    if (av == NULL || ar == NULL || ar->inicio == NULL)
        return;


    if (ar->inicio == av)
        ar->inicio = av->prox;
    else
    {
        aviao *anterior = ar->inicio;
        while (anterior != NULL && anterior->prox != av)
        {
            anterior = anterior->prox;
        }

        if (anterior != NULL)
        {

            anterior->prox = av->prox;
        }
        else
            return;
    }
    free(av);
}

