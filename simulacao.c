#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
 
#define MAX_CARREGADORES 3
#define LIMITE_POTENCIA 50.0  
 

typedef struct {
    char cpf[12];         
    int ativo;            
    float potencia_kw;    
    char horario[20];     
    time_t inicio;        
} Sessao;
 

Sessao carregadores[MAX_CARREGADORES];
 

float potencia_em_uso() {
    float total = 0;
    int i;
    for (i = 0; i < MAX_CARREGADORES; i++) {
        if (carregadores[i].ativo == 1) {
            total += carregadores[i].potencia_kw;
        }
    }
    return total;
}
 

void iniciar_sessao() {
 
    int numero;
    char cpf[12];
    float potencia;
 
    
    float disponivel = LIMITE_POTENCIA - potencia_em_uso();
    if (disponivel <= 0) {
        printf("\nSistema bloqueado: limite de potencia atingido! (%.1f kW em uso)\n", potencia_em_uso());
        return;
    }
 
    printf("\n--- INICIAR SESSAO ---\n");
    printf("Potencia disponivel na estacao: %.1f kW\n\n", disponivel);
 
    
    int i;
    for (i = 0; i < MAX_CARREGADORES; i++) {
        printf("  Carregador %d: %s\n", i + 1, carregadores[i].ativo ? "EM USO" : "LIVRE");
    }
 
    
    do {
        printf("\nEscolha o carregador (1 a %d): ", MAX_CARREGADORES);
        scanf("%d", &numero);
        numero--;  
 
        if (numero < 0 || numero >= MAX_CARREGADORES) {
            printf("Carregador invalido!\n");
            numero = -1;
        } else if (carregadores[numero].ativo == 1) {
            printf("Carregador ja esta em uso!\n");
            numero = -1;
        }
    } while (numero == -1);
 
    
    while (1) {
        printf("Informe seu CPF (apenas numeros): ");
        scanf("%s", cpf);
        if (strlen(cpf) == 11) break;
        printf("CPF invalido! Digite os 11 numeros.\n");
    }
 
    
    do {
        printf("Informe a potencia desejada (1 a %.1f kW): ", disponivel);
        scanf("%f", &potencia);
        if (potencia <= 0 || potencia > disponivel) {
            printf("Potencia invalida! Disponivel: %.1f kW\n", disponivel);
        }
    } while (potencia <= 0 || potencia > disponivel);
 
   
    printf("\n[SEMS+] Verificando disponibilidade...\n");
    if (potencia_em_uso() + potencia > LIMITE_POTENCIA) {
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
 void encerrar_sessao() {
 
    int numero;
 
    printf("\n--- ENCERRAR SESSAO ---\n\n");
 
    
    int i;
    int algum_ativo = 0;
    for (i = 0; i < MAX_CARREGADORES; i++) {
        if (carregadores[i].ativo == 1) {
            printf("  Carregador %d: EM USO | CPF: %s | Potencia: %.1f kW | Inicio: %s\n",
                i + 1,
                carregadores[i].cpf,
                carregadores[i].potencia_kw,
                carregadores[i].horario);
            algum_ativo = 1;
        }
    }
 

    if (algum_ativo == 0) {
        printf("  Nenhuma sessao ativa no momento.\n");
        return;
    }
 
    do {
        printf("\nEscolha o carregador para encerrar (1 a %d): ", MAX_CARREGADORES);
        scanf("%d", &numero);
        numero--;  
 
        if (numero < 0 || numero >= MAX_CARREGADORES) {
            printf("Carregador invalido!\n");
            numero = -1;
        } else if (carregadores[numero].ativo == 0) {
            printf("Esse carregador nao esta em uso!\n");
            numero = -1;
        }
    } while (numero == -1);
 
    
    time_t fim = time(NULL);
    double segundos = difftime(fim, carregadores[numero].inicio);
    double minutos  = segundos / 60.0;
    double horas    = segundos / 3600.0;
    float energia   = carregadores[numero].potencia_kw * horas;
    float preco_kwh = 1.80;
    float total     = energia * preco_kwh;
 
    
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
 
    
    carregadores[numero].ativo = 0;
    strcpy(carregadores[numero].cpf, "");
    carregadores[numero].potencia_kw = 0;
    strcpy(carregadores[numero].horario, "");
}
void ver_status() {
 
    printf("\n--- STATUS DOS CARREGADORES ---\n\n");
 
    int i;
    for (i = 0; i < MAX_CARREGADORES; i++) {
 
        printf("  Carregador %d: ", i + 1);
 
        if (carregadores[i].ativo == 0) {
            printf("LIVRE\n");
        } else {
 
            
            time_t agora = time(NULL);
            double segundos = difftime(agora, carregadores[i].inicio);
            double minutos  = segundos / 60.0;
 
            printf("EM USO\n");
            printf("    CPF       : %s\n", carregadores[i].cpf);
            printf("    Potencia  : %.1f kW\n", carregadores[i].potencia_kw);
            printf("    Inicio    : %s\n", carregadores[i].horario);
            printf("    Decorrido : %.1f minutos\n", minutos);
        }
    }
 
    
    float em_uso     = potencia_em_uso();
    float disponivel = LIMITE_POTENCIA - em_uso;
 
    printf("\n----------------------------------------------\n");
    printf("  Potencia total da estacao : %.1f kW\n", LIMITE_POTENCIA);
    printf("  Em uso                    : %.1f kW\n", em_uso);
    printf("  Disponivel                : %.1f kW\n", disponivel);
    printf("----------------------------------------------\n");

 float em_uso     = potencia_em_uso();
    float disponivel = LIMITE_POTENCIA - em_uso;
 
    printf("\n----------------------------------------------\n");
    printf("  Potencia total da estacao : %.1f kW\n", LIMITE_POTENCIA);
    printf("  Em uso                    : %.1f kW\n", em_uso);
    printf("  Disponivel                : %.1f kW\n", disponivel);
    printf("----------------------------------------------\n");

}

void gerar_relatorio() {
 
    printf("\n==============================================\n");
    printf("         RELATORIO DA ESTACAO\n");
    printf("==============================================\n");
 
    
    time_t agora = time(NULL);
    char horario_atual[20];
    struct tm *t = localtime(&agora);
    strftime(horario_atual, 20, "%d/%m/%Y %H:%M", t);
    printf("  Gerado em: %s\n", horario_atual);
    printf("----------------------------------------------\n");
 
    int i;
    int sessoes_ativas = 0;
    float faturamento_estimado = 0;
    float energia_total = 0;
    float preco_kwh = 1.80;
 
    for (i = 0; i < MAX_CARREGADORES; i++) {
 
        printf("\n  Carregador %d: ", i + 1);
 
        if (carregadores[i].ativo == 0) {
            printf("LIVRE\n");
        } else {
 
            
            double segundos = difftime(agora, carregadores[i].inicio);
            double minutos  = segundos / 60.0;
            double horas    = segundos / 3600.0;
            float energia   = carregadores[i].potencia_kw * horas;
            float valor     = energia * preco_kwh;
 
            faturamento_estimado += valor;
            energia_total        += energia;
            sessoes_ativas++;
 
            printf("EM USO\n");
            printf("    CPF         : %s\n", carregadores[i].cpf);
            printf("    Inicio      : %s\n", carregadores[i].horario);
            printf("    Potencia    : %.1f kW\n", carregadores[i].potencia_kw);
            printf("    Decorrido   : %.1f minutos\n", minutos);
            printf("    Energia     : %.3f kWh\n", energia);
            printf("    Valor atual : R$ %.2f\n", valor);
        }
    }
 
    
    printf("\n==============================================\n");
    printf("         RESUMO GERAL\n");
    printf("==============================================\n");
    printf("  Sessoes ativas          : %d / %d\n", sessoes_ativas, MAX_CARREGADORES);
    printf("  Potencia em uso         : %.1f / %.1f kW\n", potencia_em_uso(), LIMITE_POTENCIA);
    printf("  Energia entregue        : %.3f kWh\n", energia_total);
    printf("  Faturamento estimado    : R$ %.2f\n", faturamento_estimado);
    printf("  Tarifa aplicada         : R$ %.2f/kWh\n", preco_kwh);
    printf("==============================================\n");
}
 
void exibir_menu() {
    printf("\n==============================================\n");
    printf("     CHARGERID INTELLIGENCE - GOODWE\n");
    printf("==============================================\n");
    printf("  1 - Iniciar nova sessao\n");
    printf("  2 - Encerrar sessao\n");
    printf("  3 - Ver status dos carregadores\n");
    printf("  4 - Gerar relatorio\n");
    printf("  0 - Sair\n");
    printf("==============================================\n");
    printf("  Escolha uma opcao: ");
}
 
int main() {
 
    int opcao;
 
   
    int i;
    for (i = 0; i < MAX_CARREGADORES; i++) {
        carregadores[i].ativo = 0;
    }
 
    
    do {
        exibir_menu();
        scanf("%d", &opcao);
 
        switch (opcao) {
            case 1:
                iniciar_sessao();
                break;
            case 2:
                encerrar_sessao();
                break;
            case 3:
                ver_status();
                break;
            case 4:
                gerar_relatorio();
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
 