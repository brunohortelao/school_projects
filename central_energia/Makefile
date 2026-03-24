################################################
#           Makefile - Central de Energia      #
#                      ---                     #
#           Adriane Gonçalves - 240000004      #
#           Bruno Hortelão - 240001083         #
################################################
# Variáveis de Compilação
CC = gcc
CFLAGS = -Wall -Wextra -g
# -Wall -Wextra: Ativa avisos importantes para Clean Code
# -g: Adiciona informações de debug 

# Header
DEPS = utils.h

# Nomes dos executáveis
TARGETS = gestor_energia simulador_centrais

# Regra padrão
all: $(TARGETS)

# Regra para compilar o Gestor de Energia
gestor_energia: gestor_energia.c $(DEPS)
	$(CC) $(CFLAGS) gestor_energia.c -o gestor_energia

# Regra para compilar o Simulador de Centrais
# Agora o simulador depende de simulador_centrais.c E central.c
simulador_centrais: simulador_centrais.c central.c $(DEPS)
	$(CC) $(CFLAGS) simulador_centrais.c central.c -o simulador_centrais

# Regra de Limpeza
# Remove os executáveis
# Limpa também os pipes no caso de o programa crashar sem os encerrar
clean:
	rm -f $(TARGETS)
	rm -f /tmp/pipe_energia /tmp/pipe_alertas
	@echo "Limpeza concluída."

# Regra .PHONY para evitar conflitos se existirem arquivos com estes nomes
.PHONY: all clean