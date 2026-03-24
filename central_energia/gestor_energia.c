/*
* +++ Gestor de Energia +++
* 
* Adriane Goncalves - 240000004
* Bruno Hortelao - 240001083
*
* Componente central que monitoriza a produção e recebe os alertas
*/
#include "utils.h"


// Variável global para controlo de saída no signal handler
// O tipo sig_atomic_t garante que a leitura e a escrita são feitas numa única instrução de processador
volatile sig_atomic_t running = 1;

// Função para lidar com CTRL+C como no enunciado
void handle_sigint() {
    printf("\nA encerrar o gestor de energia...\n");
    running = 0; // Quebra o while
}

int main() {

    // os 2 FD's um para ler os dados de produção e outro os alertas
    int fd_energia, fd_alerta;

    // fd_set - será a lista de fd's para o select
    fd_set read_fds;

    // servirá para armazenar o maior fd, para o select saber até onde procurar
    int max_fd;

    char buff[BUFFER_SIZE];
    
    // Variáveis para armazenar o total de energia produzida e para receber dados, com a struct que criámos
    int energia_total_acumulada = 0; 
    DadosProducao dados_recebidos;

    // 
    signal(SIGINT, handle_sigint);

    // Criação do Pipe para Energia
    if (mkfifo(ENERGIA, 0666) == -1) {
        // errno é o número do último erro que ocorreu
        if (errno != EEXIST) {
            perror("Erro ao criar pipe energia");
            exit(1);
        }
    }

    // Criação do Pipe para Alerta
    if (mkfifo(ALERTA, 0666) == -1) {

        if (errno != EEXIST) {
            perror("Erro ao criar pipe alertas");
            exit(1);
        }
    }

    // Abrir pipes 
    fd_energia = open(ENERGIA, O_RDWR);
    fd_alerta = open(ALERTA, O_RDWR);

    if (fd_energia == -1 || fd_alerta == -1) {
        // Verificamos os 2 porque não faz sentido continuar se algum falhar
        perror("Erro ao abrir pipes");
        unlink(ENERGIA); // Tenta limpar se falhar a abrir algum dos pipes, mas podemos fazer também através do makefile se crashar
        unlink(ALERTA);
        exit(1);
    }


    // O select precisa saber o maior número de descritor + 1, para saber onde parar de procurar
    max_fd = (fd_energia > fd_alerta ? fd_energia : fd_alerta) + 1;

    printf("Gestor de Energia iniciado. A monitorizar...\n");

    // Loop principal controlado pela variável running
    while (running) {

        // Tem que ser feito, se não, após o primeiro select
        // Só é vigiado o pipe que foi selecionado primeiro
        FD_ZERO(&read_fds);
        FD_SET(fd_energia, &read_fds);
        FD_SET(fd_alerta, &read_fds);

        // O select verifica quem tem dados
        // Os argumentos 3 e 4 são para verificação de escrita e excepções, mas só queremos ler
        // o último valor a NULL faz com que ele fique à espera até que alguém o chame em vez de ter um timeout
        int activity = select(max_fd, &read_fds, NULL, NULL, NULL);

        if (activity < 0) {
            // Se foi interrompido por sinal, continua para ir tratar dos pipes
            // EINTR  Interrupted function call
            if (errno == EINTR) {
				continue; 
			}
            perror("Erro no select");
            break;
        }

        // --- VERIFICAR PIPE DE ENERGIA (fd_energia) ---
        if (FD_ISSET(fd_energia, &read_fds)) {
            // Lemos a estrutura inteira (sizeof(DadosProducao))
            int nbytes = read(fd_energia, &dados_recebidos, sizeof(DadosProducao));
            
            if (nbytes == sizeof(DadosProducao)) {
                // Soma ao total acumulado 
                // Não é bem o valor real, visto que teriam que se fazer conversões, mas o enunciado diz
                // que é um valor aleatório
                energia_total_acumulada += dados_recebidos.energia;
                
                // Imprime a informação 
                printf("[DADOS] Energia Total Produzida: %d MWh | [DADOS] Potência Instantânea: %d kW\n", 
                       energia_total_acumulada, dados_recebidos.potencia);
            }
        }

        // --- VERIFICAR PIPE DE ALERTAS (fd_alerta) ---
        if (FD_ISSET(fd_alerta, &read_fds)) {
            int nbytes = read(fd_alerta, buff, sizeof(buff) - 1);
            
            if (nbytes > 0) {
                buff[nbytes] = '\0'; 
                // Imprime o alerta 
                printf("[ALERTA] %s\n", buff);
            }
        }
    }

    // --- LIMPEZA FINAL ---
    // Executado quando running = 0 após CTRL+C
    close(fd_energia);
    close(fd_alerta);

    // Remover os pipes
    unlink(ENERGIA);
    unlink(ALERTA); 
    printf("Gestor encerrado.\n");

    return 0;
}
