########################################################
#                                                      #
#            Centrais de Energia Elétrica              #
#                        e                             #
#                 Gestor de Energia                    #
#                                                      #
#                                                      #
#   Grupo :                                            #
#     Adriane Gonçalves - 240000004                    #
#     Bruno Hortelao - 240001083                       #
#                                                      #
#            UC - Sistemas Operativos                  #
########################################################

Objetivo: 

Este projeto simula uma rede de produção de energia em 
tempo real, focando-se em mecanismos de IPC (Inter-Process Communication).

Funcionalidades Principais:

Monitorização em Tempo Real: 
    O Gestor recebe leituras de produção (MWh) e potência instantânea.

Simulação de Avarias: 
    As centrais entram aleatoriamente em "Manutenção", parando a produção temporariamente.

Comunicação Robusta: 
    Uso de Named Pipes para transporte de dados entre programas distintos.

Sincronização de Processos: 
    Uso de Memória Partilhada para que as centrais saibam o estado umas das outras e coordenem o envio de alertas.

Tecnologias Usadas:

C (Linux)
fork() & wait()
Named Pipes (mkfifo)
Shared Memory (mmap)
I/O Multiplexing (select)
Signal Handling (signal)

