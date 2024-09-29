#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

// GRUPO
// CAIO SOBRINHO DA SILVA
// IGOR RICCI CONSTANTINO
// MATHEUS YOSHIMITSU TAMASHIRO PIREZ LANZO


#define PORT_MANAGER 8080
#define NUM_WORKERS 8

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int result = 0;
    int values[NUM_WORKERS];

    // Gerar valores aleatórios
    srand(time(0));
    for (int i = 0; i < NUM_WORKERS; i++) {
        values[i] = rand() % 100; // Gerar valores aleatórios entre 0 e 99
        printf("Worker %d will use value: %d\n", i + 1, values[i]);
    }

    // Criar socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Definir opções do socket
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configurar endereço do servidor
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT_MANAGER);

    // Associar o socket ao endereço
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, NUM_WORKERS);

    printf("Manager is listening on port %d...\n", PORT_MANAGER);

    // Criar trabalhadores
    for (int i = 0; i < NUM_WORKERS; i++) {
        if (fork() == 0) {
            // Processo filho (trabalhador)
            int worker_id = i + 1;
            int sock = 0;
            struct sockaddr_in serv_addr;

            // Criar socket para o trabalhador
            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                printf("\n Socket creation error \n");
                return -1;
            }

            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(PORT_MANAGER);

            // Converter IPv4 e IPv6 endereços de texto para binário
            if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
                printf("\nInvalid address/ Address not supported \n");
                return -1;
            }

            // Conectar ao gerente
            if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
                printf("\nConnection Failed \n");
                return -1;
            }

            // Enviar o valor gerado para o gerente
            send(sock, &values[i], sizeof(values[i]), 0);
            close(sock);
            exit(0); // Finaliza o trabalhador
        }
    }

    // Receber resultados dos trabalhadores
    for (int i = 0; i < NUM_WORKERS; i++) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        int received_value;
        read(new_socket, &received_value, sizeof(received_value));
        printf("Received value from worker %d: %d\n", i + 1, received_value);
        result += received_value;
        close(new_socket);
    }

    printf("Final result (sum): %d\n", result);

    return 0;
}
