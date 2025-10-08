#include <stdio.h>      // Biblioteca padrão de entrada e saída: printf, scanf, fgets, fprintf, etc.
#include <string.h>     // Biblioteca para manipulação de strings: strcmp, strcspn, strlen, etc.
#include <stdlib.h>     // Biblioteca padrão: funções de conversão (atoi), system(), malloc(), free(), etc.
#include <locale.h>     // Biblioteca para configuração de localização (acentuação, moeda, datas, etc.)

#define MAX 100          // Define uma constante simbólica MAX com valor 100

// Estrutura para armazenar informações de um usuário
typedef struct {
    char usuario[50];    // Campo que armazena o nome do usuário
    char senha[50];      // Campo que armazena a senha do usuário
} Usuario;               // Define o tipo de dado "Usuario"

// Função para limpar a tela do console (funciona em Windows e Linux)
void limparTela() {
#ifdef _WIN32             // Diretiva que verifica se o sistema é Windows
    system("cls");        // Comando para limpar a tela no Windows
#else
    system("clear");      // Comando para limpar a tela no Linux/Unix
#endif
}

// Função para cadastrar um novo usuário
void cadastrarUsuario() {
    setlocale(LC_ALL, "Portuguese");             // Suporte à acentuação em português
    Usuario u;                                   // Cria uma variável 'u' do tipo Usuario

    FILE *f = fopen("usuarios.txt", "a");       // Abre o arquivo no modo append (adicionar ao final)
    if (!f) {                                   // Verifica se o arquivo abriu corretamente
        printf("Erro ao abrir o arquivo!\n");   // Mensagem de erro
        return;                                 // Sai da função
    }

    printf("=== Cadastro de Usuário ===\n");    // Exibe o cabeçalho
    printf("Digite o nome de usuário: ");       // Solicita o nome do usuário
    fgets(u.usuario, sizeof(u.usuario), stdin); // Lê o nome digitado
    u.usuario[strcspn(u.usuario, "\n")] = 0;    // Remove o '\n' do final

    printf("Digite a senha: ");                 // Solicita a senha
    fgets(u.senha, sizeof(u.senha), stdin);     // Lê a senha digitada
    u.senha[strcspn(u.senha, "\n")] = 0;        // Remove o '\n' do final

    // Grava no formato usuario:senha (mais seguro e fácil de ler)
    fprintf(f, "%s:%s\n", u.usuario, u.senha);

    fclose(f);                                  // Fecha o arquivo
    printf("Usuário cadastrado com sucesso!\n"); // Mensagem de confirmação
}

// Função para realizar login
int login() {
    setlocale(LC_ALL, "Portuguese");             // Suporte à acentuação
    char usuario[50], senha[50];                 // Armazena o que o usuário digitar
    char u[50], s[50];                           // Armazena o que vem do arquivo
    int logado = 0;                              // 0 = não logado, 1 = logado

    FILE *f = fopen("usuarios.txt", "r");        // Abre o arquivo em modo leitura
    if (!f) {                                    // Se o arquivo não existe
        printf("Nenhum usuário cadastrado. Cadastre primeiro.\n");
        return 0;                                // Retorna falha
    }

    printf("=== Login ===\n");
    printf("Digite o usuário: ");
    fgets(usuario, sizeof(usuario), stdin);      // Lê o usuário digitado
    usuario[strcspn(usuario, "\n")] = 0;         // Remove o '\n'

    printf("Digite a senha: ");
    fgets(senha, sizeof(senha), stdin);          // Lê a senha digitada
    senha[strcspn(senha, "\n")] = 0;             // Remove o '\n'

    char linha[200];                             // Buffer para cada linha do arquivo
    while (fgets(linha, sizeof(linha), f)) {     // Lê linha por linha até o fim
        // Separa a parte antes e depois do ':'
        sscanf(linha, "%[^:]:%[^\n]", u, s);     // Lê tudo até ':' em 'u' e o restante em 's'

        // Compara o que o usuário digitou com o que está no arquivo
        if (strcmp(usuario, u) == 0 && strcmp(senha, s) == 0) {
            logado = 1;                          // Marca login como bem-sucedido
            break;                               // Encerra o loop
        }
    }

    fclose(f);                                   // Fecha o arquivo após leitura

    if (logado) {                                // Se logado com sucesso
        printf("Login realizado com sucesso! Bem-vindo, %s\n", usuario);
        return 1;                                // Retorna sucesso
    } else {
        printf("Usuário ou senha incorretos.\n"); // Caso contrário, erro
        return 0;                                // Retorna falha
    }
}

// Função que exibe o menu principal
void menuPrincipal() {
    setlocale(LC_ALL, "Portuguese");             // Configura acentuação
    int opcao;                                   // Guarda a opção escolhida

    do {
        printf("\n==============================\n");
        printf("==== Bem Vindo ao EduGate ====\n");
        printf("==============================\n");
        printf("1. Login\n");
        printf("2. Cadastro\n");
        printf("3. Sair\n");
        printf("Escolha uma opção: ");

        char buffer[10];                         // Buffer temporário para entrada
        fgets(buffer, sizeof(buffer), stdin);    // Lê entrada como string
        opcao = atoi(buffer);                    // Converte para inteiro

        switch (opcao) {                         // Analisa a opção
            case 1:
                login();                         // Chama o login
                break;
            case 2:
                cadastrarUsuario();              // Chama o cadastro
                break;
            case 3:
                printf("Saindo do sistema...\n");// Mensagem de saída
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 3);                        // Repete até escolher sair
}

// Função principal do programa
int main() {
    setlocale(LC_ALL, "Portuguese");             // Suporte à acentuação
    limparTela();                                // Limpa a tela
    menuPrincipal();                             // Chama o menu principal
    return 0;                                    // Finaliza o programa com sucesso
}
