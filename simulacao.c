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
                printf("\n[Encerrar sessao - em breve]\n");
                break;
            case 3:
                printf("\n[Status dos carregadores - em breve]\n");
                break;
            case 4:
                printf("\n[Relatorio - em breve]\n");
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
 