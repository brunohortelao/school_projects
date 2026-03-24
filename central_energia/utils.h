/*
 * *** Utilitários ***
 * 
 *
 * Adriane Goncalves - 240000004
 * Bruno Hortelao - 240001083
 */


#ifndef UTILS_H   // Verifica se UTILS_H ainda não foi definido
#define UTILS_H   // Define UTILS_H

// --- Bibliotecas ---
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h> // Para mmap (Memória Partilhada)
#include <fcntl.h> 
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <errno.h>
#include <signal.h> // Necessário para capturar CTRL+C


// --- Define's ----

#define ENERGIA "/tmp/pipe_energia"
#define ALERTA "/tmp/pipe_alertas"

#define BUFFER_SIZE 256


// Estrutura para enviar ou receber os dados de produção de forma organizada
typedef struct {
    int energia;   // Valor acumulado
    int potencia;  // Valor instantâneo
} DadosProducao;

#endif 