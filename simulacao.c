#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CARREGADORES 3
#define MAX_sessoes 100
#define LIMITE_POTENCIA 50.0
#define PRECO_KWH 1.80

typedef struct
{
    char cpf[12];
    float potencia_kw;
    char horario[20];
    time_t inicio;
    int ativo;
} Carregador;

typedef struct
{
    int id;
    char cpf[12];
    int carregador;
    float potencia_kw;
    float energia;
    float tempo;
    float custo;
    char horario[20];
} Registro;

Carregador carregadores[MAX_CARREGADORES];
Registro historico[MAX_sessoes];
int total_sessoes = 0;
int proximo_id = 1;

float potencia_em_uso()
{
    float total = 0;
    int i;
    for (i = 0; i < MAX_CARREGADORES; i++)
    {
        if (carregadores[i].ativo == 1)
        {
            total += carregadores[i].potencia_kw;
        }
    }
    return total;
}

void iniciar_sessao()
{

    int numero;
    char cpf[12];
    float potencia;

    float disponivel = LIMITE_POTENCIA - potencia_em_uso();
    if (disponivel <= 0)
    {
        printf("\nSistema bloqueado: limite de potencia atingido! (%.1f kW em uso)\n", potencia_em_uso());
        return;
    }

    printf("\n--- INICIAR SESSAO ---\n");
    printf("Potencia disponivel na estacao: %.1f kW\n\n", disponivel);

    int i;
    for (i = 0; i < MAX_CARREGADORES; i++)
    {
        printf("  Carregador %d: %s\n", i + 1, carregadores[i].ativo ? "EM USO" : "LIVRE");
    }

    do
    {
        printf("\nEscolha o carregador (1 a %d): ", MAX_CARREGADORES);
        scanf("%d", &numero);
        numero--;

        if (numero < 0 || numero >= MAX_CARREGADORES)
        {
            printf("Carregador invalido!\n");
            numero = -1;
        }
        else if (carregadores[numero].ativo == 1)
        {
            printf("Carregador ja esta em uso!\n");
            numero = -1;
        }
    } while (numero == -1);

    while (1)
    {
        printf("Informe seu CPF (apenas numeros): ");
        scanf("%s", cpf);
        if (strlen(cpf) == 11)
            break;
        printf("CPF invalido! Digite os 11 numeros.\n");
    }

    do
    {
        printf("Informe a potencia desejada (1 a %.1f kW): ", disponivel);
        scanf("%f", &potencia);
        if (potencia <= 0 || potencia > disponivel)
        {
            printf("Potencia invalida! Disponivel: %.1f kW\n", disponivel);
        }
    } while (potencia <= 0 || potencia > disponivel);

    printf("\n[SEMS+] Verificando disponibilidade...\n");
    if (potencia_em_uso() + potencia > LIMITE_POTENCIA)
    {
        printf("[SEMS+] RECUSADO: potencia solicitada excede o limite da estacao.\n");
        return;
    }
    printf("[SEMS+] APROVADO: sessao autorizada.\n");

    strcpy(carregadores[numero].cpf, cpf);
    carregadores[numero].potencia_kw = potencia;
    carregadores[numero].ativo = 1;
    carregadores[numero].inicio = time(NULL);

    struct tm *t = localtime(&carregadores[numero].inicio);
    strftime(carregadores[numero].horario, 20, "%d/%m/%Y %H:%M", t);

    printf("\n----------------------------------------------\n");
    printf("  SESSAO INICIADA COM SUCESSO!\n");
    printf("----------------------------------------------\n");
    printf("  CPF        : %s\n", carregadores[numero].cpf);
    printf("  Carregador : Box %d\n", numero + 1);
    printf("  Potencia   : %.1f kW\n", carregadores[numero].potencia_kw);
    printf("  Horario    : %s\n", carregadores[numero].horario);
    printf("----------------------------------------------\n");
}

float calcular_tarifa(float potencia_solicitada)
{

    time_t agora = time(NULL);
    struct tm *t = localtime(&agora);
    int hora = t->tm_hour;

    float tarifa_base;

    if (hora >= 18 && hora <= 21)
    {
        tarifa_base = 2.50;
        printf("[TARIFA] Horario de pico (18h-21h): R$ %.2f/kWh\n", tarifa_base);
    }
    else if (hora >= 22 || hora <= 5)
    {
        tarifa_base = 1.20;
        printf("[TARIFA] Horario economico (22h-05h): R$ %.2f/kWh\n", tarifa_base);
    }
    else
    {
        tarifa_base = 1.80;
        printf("[TARIFA] Horario normal: R$ %.2f/kWh\n", tarifa_base);
    }

    float carga_atual = potencia_em_uso();
    float percentual = carga_atual / LIMITE_POTENCIA;

    if (percentual >= 0.80)
    {
        tarifa_base *= 1.30;
        printf("[TARIFA] Adicional de demanda alta (+30%%): R$ %.2f/kWh\n", tarifa_base);
    }
    else if (percentual >= 0.50)
    {
        tarifa_base *= 1.10;
        printf("[TARIFA] Adicional de demanda media (+10%%): R$ %.2f/kWh\n", tarifa_base);
    }

    if (potencia_solicitada >= 30.0)
    {
        tarifa_base *= 1.15;
        printf("[TARIFA] Adicional carga rapida (+15%%): R$ %.2f/kWh\n", tarifa_base);
    }

    printf("[TARIFA] Tarifa final aplicada: R$ %.2f/kWh\n", tarifa_base);
    return tarifa_base;
}

void encerrar_sessao()
{

    int numero;

    printf("\n--- ENCERRAR SESSAO ---\n\n");

    int i;
    int algum_ativo = 0;
    for (i = 0; i < MAX_CARREGADORES; i++)
    {
        if (carregadores[i].ativo == 1)
        {
            printf("  Carregador %d: EM USO | CPF: %s | Potencia: %.1f kW | Inicio: %s\n",
                   i + 1,
                   carregadores[i].cpf,
                   carregadores[i].potencia_kw,
                   carregadores[i].horario);
            algum_ativo = 1;
        }
    }

    if (algum_ativo == 0)
    {
        printf("  Nenhuma sessao ativa no momento.\n");
        return;
    }

    do
    {
        printf("\nEscolha o carregador para encerrar (1 a %d): ", MAX_CARREGADORES);
        scanf("%d", &numero);
        numero--;

        if (numero < 0 || numero >= MAX_CARREGADORES)
        {
            printf("Carregador invalido!\n");
            numero = -1;
        }
        else if (carregadores[numero].ativo == 0)
        {
            printf("Esse carregador nao esta em uso!\n");
            numero = -1;
        }
    } while (numero == -1);

    time_t fim = time(NULL);
    double segundos = difftime(fim, carregadores[numero].inicio);
    double minutos = segundos / 60.0;
    double horas = segundos / 3600.0;
    float energia = carregadores[numero].potencia_kw * horas;
    float preco_kwh = calcular_tarifa(carregadores[numero].potencia_kw);
    float total = energia * preco_kwh;

    char horario_fim[20];
    struct tm *t = localtime(&fim);
    strftime(horario_fim, 20, "%d/%m/%Y %H:%M", t);

    printf("\n----------------------------------------------\n");
    printf("  SESSAO ENCERRADA!\n");
    printf("----------------------------------------------\n");
    printf("  CPF        : %s\n", carregadores[numero].cpf);
    printf("  Carregador : Box %d\n", numero + 1);
    printf("  Inicio     : %s\n", carregadores[numero].horario);
    printf("  Fim        : %s\n", horario_fim);
    printf("  Duracao    : %.1f minutos\n", minutos);
    printf("  Energia    : %.3f kWh\n", energia);
    printf("  Tarifa     : R$ %.2f/kWh\n", preco_kwh);
    printf("  Total      : R$ %.2f\n", total);
    printf("----------------------------------------------\n");

    if (total_sessoes < MAX_sessoes)
    {
        historico[total_sessoes].id = proximo_id++;
        strcpy(historico[total_sessoes].cpf, carregadores[numero].cpf);
        historico[total_sessoes].carregador = numero;
        historico[total_sessoes].potencia_kw = carregadores[numero].potencia_kw;
        historico[total_sessoes].energia = energia;
        historico[total_sessoes].tempo = (float)minutos;
        historico[total_sessoes].custo = total;
        strcpy(historico[total_sessoes].horario, horario_fim);
        total_sessoes++;
    }
    else
    {
        printf("Aviso: historico cheio, sessao nao foi salva!\n");
    }

    carregadores[numero].ativo = 0;
    strcpy(carregadores[numero].cpf, "");
    carregadores[numero].potencia_kw = 0;
    strcpy(carregadores[numero].horario, "");
}

void listar_sessoes()
{

    printf("\n--- HISTORICO DE SESSOES ---\n\n");

    if (total_sessoes == 0)
    {
        printf("Nenhuma sessao registrada.\n");
        return;
    }

    int i;
    for (i = 0; i < total_sessoes; i++)
    {
        printf("  ID: %d | CPF: %s | Box: %d | %.2f kWh | %.1f min | R$ %.2f | %s\n",
               historico[i].id,
               historico[i].cpf,
               historico[i].carregador + 1,
               historico[i].energia,
               historico[i].tempo,
               historico[i].custo,
               historico[i].horario);
    }
    printf("\nTotal de sessoes registradas: %d\n", total_sessoes);
}

void ver_status()
{

    printf("\n--- STATUS DOS CARREGADORES ---\n\n");

    int i;
    for (i = 0; i < MAX_CARREGADORES; i++)
    {

        printf("  Carregador %d: ", i + 1);

        if (carregadores[i].ativo == 0)
        {
            printf("LIVRE\n");
        }
        else
        {

            time_t agora = time(NULL);
            double segundos = difftime(agora, carregadores[i].inicio);
            double minutos = segundos / 60.0;

            printf("EM USO\n");
            printf("    CPF       : %s\n", carregadores[i].cpf);
            printf("    Potencia  : %.1f kW\n", carregadores[i].potencia_kw);
            printf("    Inicio    : %s\n", carregadores[i].horario);
            printf("    Decorrido : %.1f minutos\n", minutos);
        }
    }

    float em_uso = potencia_em_uso();
    float disponivel = LIMITE_POTENCIA - em_uso;

    printf("\n----------------------------------------------\n");
    printf("  Potencia total da estacao : %.1f kW\n", LIMITE_POTENCIA);
    printf("  Em uso                    : %.1f kW\n", em_uso);
    printf("  Disponivel                : %.1f kW\n", disponivel);
    printf("----------------------------------------------\n");
}
void buscar_sessao()
{
    if (total_sessoes == 0)
    {
        printf("\n  Nenhuma sessao registrada ainda.\n");
        return;
    }

    int id_busca;
    printf("\n--- BUSCAR SESSAO ---\n");
    printf("Digite o ID da sessao: ");
    scanf("%d", &id_busca);

    int encontrado = -1;
    for (int i = 0; i < total_sessoes; i++)
    {
        if (historico[i].id == id_busca)
        {
            encontrado = i;
            break;
        }
    }

    if (encontrado == -1)
    {
        printf("  Sessao com ID %d nao encontrada.\n", id_busca);
    }
    else
    {
        printf("\n----------------------------------------------\n");
        printf("  SESSAO ENCONTRADA!\n");
        printf("----------------------------------------------\n");
        printf("  ID         : %d\n", historico[encontrado].id);
        printf("  CPF        : %s\n", historico[encontrado].cpf);
        printf("  Carregador : Box %d\n", historico[encontrado].carregador);
        printf("  Horario    : %s\n", historico[encontrado].horario);
        printf("  Duracao    : %.1f minutos\n", historico[encontrado].tempo);
        printf("  Energia    : %.3f kWh\n", historico[encontrado].energia);
        printf("  Total      : R$ %.2f\n", historico[encontrado].custo);
    }
}
void ordenar_sessoes()
{

    if (total_sessoes == 0)
    {
        printf("\n  Nenhuma sessao para ordenar.\n");
        return;
    }

    printf("\n--- ORDENAR SESSOES ---\n");
    printf("  1 - Por ID\n");
    printf("  2 - Por energia consumida\n");
    printf("  3 - Por custo\n");
    printf("  4 - Por tempo\n");
    printf("  Escolha: ");

    int criterio;
    scanf("%d", &criterio);

    int i, j, min_idx;
    Registro temp;

    for (i = 0; i < total_sessoes - 1; i++)
    {
        min_idx = i;

        for (j = i + 1; j < total_sessoes; j++)
        {

            int trocar = 0;

            if (criterio == 1 && historico[j].id < historico[min_idx].id)
                trocar = 1;
            else if (criterio == 2 && historico[j].energia < historico[min_idx].energia)
                trocar = 1;
            else if (criterio == 3 && historico[j].custo < historico[min_idx].custo)
                trocar = 1;
            else if (criterio == 4 && historico[j].tempo < historico[min_idx].tempo)
                trocar = 1;

            if (trocar)
                min_idx = j;
        }

        if (min_idx != i)
        {
            temp = historico[i];
            historico[i] = historico[min_idx];
            historico[min_idx] = temp;
        }
    }

    printf("  Sessoes ordenadas com sucesso!\n");
    listar_sessoes();
}

void mostrar_estatisticas()
{

    printf("\n==============================================\n");
    printf("           ESTATISTICAS DA ESTACAO\n");
    printf("==============================================\n");

    if (total_sessoes == 0)
    {
        printf("  Nenhuma sessao registrada ainda.\n");
        return;
    }

    float energia_total = 0;
    float faturamento = 0;
    float maior_consumo = historico[0].energia;
    float menor_consumo = historico[0].energia;

    int i;
    for (i = 0; i < total_sessoes; i++)
    {
        energia_total += historico[i].energia;
        faturamento += historico[i].custo;

        if (historico[i].energia > maior_consumo)
            maior_consumo = historico[i].energia;
        if (historico[i].energia < menor_consumo)
            menor_consumo = historico[i].energia;
    }

    float ticket_medio = faturamento / total_sessoes;

    printf("  Sessoes realizadas : %d\n", total_sessoes);
    printf("  Energia fornecida  : %.3f kWh\n", energia_total);
    printf("  Faturamento total  : R$ %.2f\n", faturamento);
    printf("  Ticket medio       : R$ %.2f\n", ticket_medio);
    printf("  Maior consumo      : %.3f kWh\n", maior_consumo);
    printf("  Menor consumo      : %.3f kWh\n", menor_consumo);
    printf("==============================================\n");
}

void exibir_menu()
{
    printf("\n==============================================\n");
    printf("     CHARGERID INTELLIGENCE - GOODWE\n");
    printf("==============================================\n");
    printf("  1 - Iniciar nova sessao\n");
    printf("  2 - Encerrar sessao\n");
    printf("  3 - Listar sessoes\n");
    printf("  4 - Buscar sessao por ID\n");
    printf("  5 - Ordernar sessoes\n");
    printf("  6 - Estatisticas\n");
    printf("  7 - Status dos carregadores\n");
    printf("  0 - Sair\n");
    printf("==============================================\n");
    printf("  Escolha uma opcao: ");
}

int main()
{

    int opcao;

    int i;
    for (i = 0; i < MAX_CARREGADORES; i++)
    {
        carregadores[i].ativo = 0;
    }

    do
    {
        exibir_menu();
        scanf("%d", &opcao);

        switch (opcao)
        {
        case 1:
            iniciar_sessao();
            break;
        case 2:
            encerrar_sessao();
            break;
        case 3:
            listar_sessoes();
            break;
        case 4:
            buscar_sessao();
            break;
        case 5:
            ordenar_sessoes();
            break;
        case 6:
            mostrar_estatisticas();
            break;
        case 7:
            ver_status();
            break;
        case 0:
            printf("\nSistema encerrado.\n");
            break;
        default:
            printf("\nOpcao invalida!\n");
        }

    } while (opcao != 0);

    return 0;
}
