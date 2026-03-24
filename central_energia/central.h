/*
 * *** Utilitários da central ***
 * 
 *
 * Adriane Goncalves - 240000004
 * Bruno Hortelao - 240001083
 */



#ifndef CENTRAL_H
#define CENTRAL_H

#include "utils.h" // Precisa de conhecer as structs do projeto

// Estrutura que representa o estado de uma central
// Estrutura de memória partilhada para gerir o estado de todas as centrais
typedef struct {
    pid_t pid;
    int estado;              // 1 = Ativo, 0 = Manutenção
    int pendente_aviso_entrada; // 1 = Precisa avisar que entrou em manutenção
    int pendente_aviso_saida;   // 1 = Precisa avisar que saiu da manutenção
} EstadoCentral;

// Declaração das funções que a central vai usar
void comportamento_central(int id_indice, int total_centrais, EstadoCentral *mem_centrais);
bool central_que_anuncia(int meu_indice, int total_centrais, EstadoCentral *mem_centrais);

#endif