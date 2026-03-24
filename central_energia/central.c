
#include "central.h"

// Probabilidade de entrar em manutenção a cada ciclo 
#define PROB_MANUTENCAO 5 
#define TEMPO_MANUTENCAO 30
#define CICLO_SIMULACAO 3 // Segundos entre cada produção, se for muito baixo, polui o terminal

// Estados da Central
#define ESTADO_ATIVO 1
#define ESTADO_MANUTENCAO 0

// --- Lógica de cada Central ---
void comportamento_central(int id_indice, int total_centrais, EstadoCentral *mem_centrais) {
    
    // A seed aleatória baseada no PID, se for só a hora, como são criados muito próximos
    // poderia dar seeds iguais 
    srand(getpid() + time(NULL));

   
    // É importante abrir apos o gestor ter criado os pipes com mkfifo.
    // Se o gestor não estiver a correr, o open pode bloquear.
    int fd_energia = open(ENERGIA, O_WRONLY);
    int fd_alerta = open(ALERTA, O_WRONLY);

    // Verificamos os 2 porque não faz sentido continuar se algum falhar
    if (fd_energia == -1 || fd_alerta == -1) {
        perror("Erro ao abrir pipes (O gestor está a correr?)");
        exit(1);
    }

    // Registar a informação da central na memória partilhada
    mem_centrais[id_indice].pid = getpid();
    mem_centrais[id_indice].estado = ESTADO_ATIVO;
    mem_centrais[id_indice].pendente_aviso_entrada = 0;
    mem_centrais[id_indice].pendente_aviso_saida = 0;

    printf("Central %d (PID %d) iniciada e ativa.\n", id_indice+1, getpid());

    while (1) {
        // Verificar ALERTAS PENDENTES
        // Verifica se algum irmão precisa de ajuda para enviar alerta
        if (mem_centrais[id_indice].estado == ESTADO_ATIVO) {
            // Percorre todas as centrais para ver se há avisos pendentes
            for (int i = 0; i < total_centrais; i++) {
                
                //  Alguém entrou em manutenção
                if (mem_centrais[i].pendente_aviso_entrada == 1) {
                    if (central_que_anuncia(id_indice, total_centrais, mem_centrais)) {
                        char msg[BUFFER_SIZE];
                        
                        sprintf(msg, "Aviso: A central %d entrou em manutenção.\n", mem_centrais[i].pid);
                        write(fd_alerta, msg, strlen(msg));

                        if (errno == EPIPE) {
                            perror("O gestor fechou o pipe de alertas. A central irá terminar.\n");
                            break;
                        }
                        mem_centrais[i].pendente_aviso_entrada = 0; // Limpa o pedido
                        printf("[PID %d] Enviei alerta de falha da central %d\n", getpid(), mem_centrais[i].pid);
                    }
                }

                // Alguém saiu da manutenção
                if (mem_centrais[i].pendente_aviso_saida == 1) {
                    if (central_que_anuncia(id_indice, total_centrais, mem_centrais)) {
                        char msg[BUFFER_SIZE];
                        
                        sprintf(msg, "Aviso: A central %d terminou a manutenção.", mem_centrais[i].pid);
                        write(fd_alerta, msg, strlen(msg));
                        
                        if (errno == EPIPE) {
                            perror("O gestor fechou o pipe de alertas. A central irá terminar.\n");
                            break;
                        }

                        mem_centrais[i].pendente_aviso_saida = 0; // Limpa o pedido
                        printf("[PID %d] Enviei alerta de regresso da central %d\n", getpid(), mem_centrais[i].pid);
                    }
                }
            }
        }

        // Produção de energia
        if (mem_centrais[id_indice].estado == ESTADO_ATIVO) {
            DadosProducao d;
            d.energia = (rand() % 100) + 1; // 1 a 100 MWh
            d.potencia = (rand() % 500) + 100; // 100 a 600 kW

            // Escreve no pipe de energia com a struct que usamos
            write(fd_energia, &d, sizeof(DadosProducao));

            if (errno == EPIPE) {
                perror("O gestor fechou o pipe de alertas. A central irá terminar.\n");
                break;
            }
            printf("Central %d produziu.\n", getpid()); 
        }

        // Em Caso de Falha ou seja, entrar EM MANUTENÇÃO
        // Só pode falhar se estiver ativo 
        if (mem_centrais[id_indice].estado == ESTADO_ATIVO) {
            // Gera um numero entre 0 e 100, se for inferior ao definido em PROB_MANUTENCAO
            // a central entra em manutenção
            int chance = rand() % 100;

            if (chance < PROB_MANUTENCAO) {
                printf("Central %d (PID %d) vai entrar em manutenção!\n", id_indice+1, getpid());
                
                // Marca estado e altera o estado para saber que tem um envio de alerta pendente
                mem_centrais[id_indice].estado = ESTADO_MANUTENCAO;
                mem_centrais[id_indice].pendente_aviso_entrada = 1; 

                // Dorme o tempo que for definido em TEMPO_MANUTENCAO no utils.h
                sleep(TEMPO_MANUTENCAO);

                // Quando volta ao ativo
                printf("Central %d (PID %d) terminou a manutenção.\n", id_indice+1, getpid());
                mem_centrais[id_indice].estado = ESTADO_ATIVO;
                mem_centrais[id_indice].pendente_aviso_saida = 1; // Pede alerta para regressar
            }
        }

        sleep(CICLO_SIMULACAO); // Aguarda próximo ciclo de produção
    }

    close(fd_energia);
    close(fd_alerta);
}

// Função auxiliar para determinar se este processo é o "Eleito" 
// Definido no enunciado será o processo com o menor PID e com "ativo"
bool central_que_anuncia(int meu_indice, int total_centrais, EstadoCentral *mem_centrais) {
    pid_t meu_pid = mem_centrais[meu_indice].pid;

    // Verificar se eu estou ativo
    if (mem_centrais[meu_indice].estado != ESTADO_ATIVO) {
        return false;
    }
    // 2. Verificar se existe algum PID ATIVO menor que o meu
    for (int i = 0; i < total_centrais; i++) {
        // Ignora-se e as posições que estejam vazias
        if (i == meu_indice || mem_centrais[i].pid == 0) {
            continue;
        } 

        // Se encontrar alguém ATIVO com PID MENOR não é escolhido e sai do método
        if (mem_centrais[i].estado == ESTADO_ATIVO && mem_centrais[i].pid < meu_pid) {
            return false; 
        }
    }

    // Se chegar aqui, será o processo que anunciará a manutenção
    return true;
}