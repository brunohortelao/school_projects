/*
 * +++ Simulador de Centrais +++
 * 
 *  
 *
 * Adriane Goncalves - 240000004
 * Bruno Hortelao - 240001083
 */

#include "utils.h"
#include "central.h"


// Protótipos para não sobrecarregar de código antes da main
void comportamento_central(int id_indice, int total_centrais, EstadoCentral *mem_centrais);
bool central_que_anuncia(int meu_indice, int total_centrais, EstadoCentral *mem_centrais);

int main(int argc, char *argv[]) {

    signal(SIGPIPE, SIG_IGN);
    // Se não for feita esta verificação e o utilizador invocar o programa sem argumentos, 
    // dá Segmentation fault porque argv[1] é NULL
    if (argc != 2) {
        fprintf(stderr, "Para iniciar o simulador: %s <número_centrais>\nNOTA: o número de centrais deve ser inteiro!\n", argv[0]);
        return 1;
    } 
    // Verificar se o argv[1] não é um número decimal
    char *arg_ver = argv[1];
    char *enc_char;

    // Tenta converter para long
    // strtol(string, ponteiro_para_o_fim, base)
    // O utilizador deve especificar o número de centrais
    strtol(arg_ver, &enc_char, 10);

    if (arg_ver == enc_char || *enc_char != '\0' ) {
        fprintf(stderr, "Para iniciar o simulador: %s <número_centrais>\nNOTA: o número de centrais deve ser inteiro!\n", argv[0]);
        return 1;
    }


    // atoi converte uma string para int - default o que vem dos argumentos é considerado string
    // argv[1] -> é o argumento a seguir ao nome do programa
    int n_centrais = atoi(argv[1]);

    if (n_centrais < 1) {

        fprintf(stderr, "Erro: O número de centrais deve ser um número inteiro >= 1.\n");
        return 1;
    }

    // Criação da Memória Partilhada
    // Usamos mmap com MAP_SHARED | MAP_ANONYMOUS para partilhar entre pai e filhos
    // size_t é um tipo de dados sem sinal usado para representar tamanhos de objetos em bytes
    size_t tam_mem_p = sizeof(EstadoCentral) * n_centrais;

    // PROT_READ | PROT_WRITE são flags para a protecção da memória - indica que o buffer poderá ser lido e escrito
    int protecao = PROT_READ | PROT_WRITE;
    // A Flag MAP_ANONYMOUS faz com que o SO garanta que a memória está limpa para não ler informações de processos anteriores
    int visibilidade = MAP_SHARED | MAP_ANONYMOUS;
    // Vamos fazer uma system call para reservar memória 
    EstadoCentral *mem_centrais = mmap(NULL, tam_mem_p, protecao, visibilidade, -1, 0);


    // Caso o mmap falhe
    if (mem_centrais == MAP_FAILED) {
        perror("Erro ao criar memória partilhada");
        return 1;
    }

    // Inicializar a memória - memset( *, 0, *) garante que a memória que
    // vamos utilizar estará "limpa"
    memset(mem_centrais, 0, tam_mem_p);

    printf("Simulador iniciado. A criar %d centrais...\n", n_centrais);

    // Criação dos Processos Filhos 
    for (int i = 0; i < n_centrais; i++) {
        
        pid_t pid = fork();

        // Erro na criação do fork()
        if (pid < 0) {
            perror("Erro no fork");
            exit(1);
        }

        if (pid == 0) {
            // Código do Filho
            comportamento_central(i, n_centrais, mem_centrais);
            exit(0); // Filho termina aqui
        } 
    }

    // Como os filhos correm num while(1), o pai ficará aqui até receber um sinal, 
    // no caso o SIGINT ou que o gestor termine e feche os pipes
    // sem isto o pai terminaria
    while (wait(NULL) > 0);

    // Limpeza (só acontece se os filhos terminarem)
    munmap(mem_centrais, tam_mem_p);
    printf("Simulação terminada.\n");

    return 0;
}

