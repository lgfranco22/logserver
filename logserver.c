#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#define PORT <PORT>
#define LOG_FILE "/var/lo/logserver.log"
#define PID_FILE "/var/run/logserver.pid"
#define BUFFER_SIZE 1024

// Função para escrever no arquivo de log
void write_to_log(const char *data) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL) {
        perror("Erro ao abrir o arquivo de log");
        exit(EXIT_FAILURE);
    }
    fprintf(log_file, "%s\n", data);
    fclose(log_file);
}

// Função para criar um daemon (executar em segundo plano)
void daemonize() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Erro ao criar o processo filho");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        // O processo pai sai, o filho continua em segundo plano
        exit(0);
    }

    // Definir o novo processo como líder de sessão (evitar que o processo seja vinculado a um terminal)
    if (setsid() < 0) {
        perror("Erro ao criar uma nova sessão");
        exit(EXIT_FAILURE);
    }

    // Garantir que o processo não tenha um terminal
    signal(SIGHUP, SIG_IGN);

    // Mudar o diretório de trabalho para o diretório raiz
    chdir("/");

    // Fechar as descrições de arquivo padrão
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Abrir os arquivos de log, se necessário
    open("/dev/null", O_RDWR);  // STDIN
    open("/dev/null", O_RDWR);  // STDOUT
    open("/dev/null", O_RDWR);  // STDERR
}

// Função para verificar se o servidor está rodando
int is_server_running() {
    FILE *pid_file = fopen(PID_FILE, "r");
    if (pid_file == NULL) {
        return 0;  // O servidor não está em execução
    }

    pid_t pid;
    if (fscanf(pid_file, "%d", &pid) != 1) {
        fclose(pid_file);
        return 0;
    }
    fclose(pid_file);

    // Verifica se o processo está rodando
    char path[256];
    sprintf(path, "/proc/%d", pid);
    return (access(path, F_OK) != -1);  // Se o diretório do processo existir, o servidor está em execução
}

// Função para salvar o PID do processo
void save_pid() {
    FILE *pid_file = fopen(PID_FILE, "w");
    if (pid_file == NULL) {
        perror("Erro ao abrir o arquivo PID");
        exit(EXIT_FAILURE);
    }
    fprintf(pid_file, "%d\n", getpid());
    fclose(pid_file);
}

// Função para excluir o arquivo PID
void remove_pid() {
    if (remove(PID_FILE) != 0) {
        perror("Erro ao remover o arquivo PID");
    }
}

// Função para parar o servidor
void stop_server() {
    if (!is_server_running()) {
        printf("Servidor não está em execução.\n");
        return;
    }

    FILE *pid_file = fopen(PID_FILE, "r");
    if (pid_file == NULL) {
        printf("Erro ao abrir o arquivo PID\n");
        return;
    }

    pid_t pid;
    fscanf(pid_file, "%d", &pid);
    fclose(pid_file);

    // Envia um sinal de término (SIGTERM) para o processo
    if (kill(pid, SIGTERM) == 0) {
        printf("Servidor parado com sucesso.\n");
        remove_pid();  // Remove o arquivo PID
    } else {
        printf("Erro ao parar o servidor.\n");
    }
}

// Função para configurar o servidor UDP
void start_server() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Cria o socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erro ao criar o socket");
        exit(EXIT_FAILURE);
    }

    // Preenche a estrutura de endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Aceita qualquer IP
    server_addr.sin_port = htons(PORT);       // Porta 514

    // Faz o bind do socket à porta
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erro ao fazer bind");
        exit(EXIT_FAILURE);
    }

    save_pid();  // Salva o PID do servidor no arquivo

    printf("Servidor UDP escutando na porta %d...\n", PORT);

    // Loop para receber e processar dados
    while (1) {
        int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        if (n < 0) {
            perror("Erro ao receber dados");
            exit(EXIT_FAILURE);
        }

        buffer[n] = '\0'; // Null-terminate the received data

        // Exibe e escreve os dados recebidos no log
        printf("Recebido de %s:%d: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);
        write_to_log(buffer);
    }

    close(sockfd);
}

// Função para tratar o sinal de interrupção (Ctrl+C)
void signal_handler(int sig) {
    printf("Servidor interrompido.\n");
    remove_pid();  // Remove o arquivo PID quando o servidor for interrompido
    exit(0);
}

// Função para exibir o status do servidor
void status_server() {
    if (is_server_running()) {
        printf("Servidor está em execução.\n");
    } else {
        printf("Servidor não está em execução.\n");
    }
}

int main(int argc, char *argv[]) {
    // Verificar os argumentos fornecidos (start, stop, status)
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <start|stop|status>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Registrar o manipulador de sinais
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    if (strcmp(argv[1], "start") == 0) {
        if (is_server_running()) {
            printf("Servidor já está em execução.\n");
            return EXIT_FAILURE;
        }
        daemonize();  // Cria o daemon
        start_server();
    } else if (strcmp(argv[1], "stop") == 0) {
        stop_server();
    } else if (strcmp(argv[1], "status") == 0) {
        status_server();
    } else {
        fprintf(stderr, "Comando inválido. Uso: %s <start|stop|status>\n", argv[0]);
        return EXIT_FAILURE;
    }

    return 0;
}
