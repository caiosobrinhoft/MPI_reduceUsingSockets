#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <limits.h>
#include <time.h>

#define PORT_MANAGER 8080
#define PORT_WORKER0 8081
#define PORT_WORKER1 8082
#define PORT_WORKER2 8083
#define PORT_WORKER3 8084
#define PORT_WORKER4 8085
#define PORT_WORKER5 8086
#define PORT_WORKER6 8087
#define PORT_WORKER7 8088

void manager() {
    int managerSocket, workerSocket;
    struct sockaddr_in managerAddress, workerAddress;
    int max_value = INT_MIN;
    int worker_values[8];
    
    // Configuração do socket do gerente
    managerSocket = socket(AF_INET, SOCK_STREAM, 0);
    managerAddress.sin_family = AF_INET;
    managerAddress.sin_port = htons(PORT_MANAGER);
    managerAddress.sin_addr.s_addr = INADDR_ANY;
    bind(managerSocket, (struct sockaddr *)&managerAddress, sizeof(managerAddress));
    listen(managerSocket, 8);
    
    // Receber valores dos trabalhadores
    for (int i = 0; i < 8; i++) {
        socklen_t addr_size = sizeof(workerAddress);
        workerSocket = accept(managerSocket, (struct sockaddr *)&workerAddress, &addr_size);
        recv(workerSocket, &worker_values[i], sizeof(int), 0);
        close(workerSocket);
        if (worker_values[i] > max_value) {
            max_value = worker_values[i];
        }
    }
    
    printf("Resultado final da redução (max): %d\n", max_value);
    close(managerSocket);
}

void worker(int worker_id) {
    int workerSocket;
    struct sockaddr_in managerAddress;
    int value = rand() % 100; // Gera um valor aleatório entre 0 e 99
    
    // Configuração do socket do trabalhador
    workerSocket = socket(AF_INET, SOCK_STREAM, 0);
    managerAddress.sin_family = AF_INET;
    managerAddress.sin_port = htons(PORT_MANAGER);
    managerAddress.sin_addr.s_addr = INADDR_ANY;
    
    connect(workerSocket, (struct sockaddr *)&managerAddress, sizeof(managerAddress));
    send(workerSocket, &value, sizeof(int), 0);
    close(workerSocket);
    
    printf("Trabalhador %d enviou valor: %d\n", worker_id, value);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <tipo>\n", argv[0]);
        exit(1);
    }
    
    int tipo = atoi(argv[1]);
    
    srand(time(NULL) + tipo); // Inicializa o gerador de números aleatórios com uma semente diferente para cada trabalhador
    
    if (tipo == 0) {
        manager();
    } else {
        worker(tipo - 1);
    }
    
    return 0;
}
