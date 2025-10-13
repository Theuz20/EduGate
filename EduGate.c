// ============================================================================
// BIBLIOTECAS UTILIZADAS
// ============================================================================

#include <stdio.h>      // Biblioteca padrão de entrada/saída — printf, fgets, fprintf, etc.
#include <string.h>     // Biblioteca para manipulação de strings — strcmp, strcspn, sscanf, etc.
#include <stdlib.h>     // Biblioteca padrão — funções de conversão (atoi), system(), malloc(), etc.
#include <locale.h>     // Biblioteca para suporte à acentuação e formatação regional
#include <windows.h>    // Biblioteca do Windows para configurar codificação UTF-8 no console

#define MAX 100         // Define uma constante simbólica chamada MAX com valor 100 (não usada aqui, mas útil como padrão)

// ============================================================================
// DEFINIÇÃO DE ESTRUTURA (TIPO DE DADO USUÁRIO)
// ============================================================================

// Estrutura que armazena informações básicas de um usuário do sistema
typedef struct {
    char usuario[50];   // Campo para o nome de usuário (até 49 caracteres + terminador '\0')
    char senha[50];     // Campo para a senha do usuário (até 49 caracteres + terminador '\0')
} Usuario;              // O nome do tipo de dado criado é "Usuario"

// ============================================================================
// CONFIGURAÇÃO DE CODIFICAÇÃO (UTF-8)
// ============================================================================

// Esta função garante que o console do Windows exiba acentuação corretamente (UTF-8)
void configurarCodificacao() {
#ifdef _WIN32                      // Diretiva de pré-processador: se o código estiver sendo compilado no Windows...
    SetConsoleOutputCP(65001);     // Define a codificação de saída do console como UTF-8 (para printf)
    SetConsoleCP(65001);           // Define a codificação de entrada do console como UTF-8 (para fgets)
#endif
    setlocale(LC_ALL, "pt_BR.UTF-8"); // Configura o idioma e formatação do sistema para português (Brasil) com suporte UTF-8
}

// ============================================================================
// FUNÇÃO PARA LIMPAR A TELA
// ============================================================================

// Limpa o console, independentemente do sistema operacional (Windows ou Linux)
void limparTela() {
#ifdef _WIN32          // Se o sistema for Windows
    system("cls");     // Usa o comando interno "cls" para limpar a tela
#else
    system("clear");   // Caso contrário (Linux, Mac), usa o comando "clear"
#endif
}

// ============================================================================
// FUNÇÃO PARA CADASTRAR USUÁRIO (ALUNO OU PROFESSOR)
// ============================================================================

// Recebe o nome do arquivo (onde será salvo) e o tipo ("Aluno" ou "Professor")
void cadastrarUsuario(const char *arquivo, const char *tipo) {
    Usuario u;                                 // Cria uma variável 'u' do tipo Usuario
    FILE *f = fopen(arquivo, "a");             // Abre o arquivo no modo append ('a' = adicionar sem apagar conteúdo anterior)

    if (!f) {                                  // Se o arquivo não foi aberto corretamente
        printf("Erro ao abrir o arquivo de %s!\n", tipo); // Exibe mensagem de erro indicando qual tipo falhou
        return;                                // Encerra a função
    }

    printf("=== Cadastro de %s ===\n", tipo);  // Cabeçalho informativo
    printf("Digite o nome de usuário: ");      // Solicita que o usuário digite um nome

    fgets(u.usuario, sizeof(u.usuario), stdin); // Lê a string digitada (inclui o '\n' no final)
    u.usuario[strcspn(u.usuario, "\n")] = 0;    // Substitui o '\n' (ENTER) por '\0', removendo-o da string

    printf("Digite a senha: ");                // Solicita a senha
    fgets(u.senha, sizeof(u.senha), stdin);    // Lê a senha digitada
    u.senha[strcspn(u.senha, "\n")] = 0;       // Remove o '\n' final da senha também

    // Escreve no arquivo no formato "usuario:senha", facilitando leitura posterior
    fprintf(f, "%s:%s\n", u.usuario, u.senha);

    fclose(f);                                 // Fecha o arquivo para salvar os dados
    printf("%s cadastrado(a) com sucesso!\n", tipo); // Confirma que o cadastro foi feito
}

// ============================================================================
// FUNÇÃO PARA LOGIN DE USUÁRIO (ALUNO OU PROFESSOR)
// ============================================================================

// Recebe o nome do arquivo (onde os dados estão salvos) e o tipo ("Aluno" ou "Professor")
int loginUsuario(const char *arquivo, const char *tipo) {
    Usuario u;                                 // Variável para armazenar usuário lido do arquivo
    char usuario[50], senha[50];               // Variáveis para armazenar o que o usuário digitará
    char linha[200];                           // Buffer para armazenar cada linha do arquivo
    int logado = 0;                            // Flag de controle (0 = não logado, 1 = logado)

    FILE *f = fopen(arquivo, "r");             // Abre o arquivo no modo leitura
    if (!f) {                                  // Se não conseguiu abrir (por exemplo, arquivo não existe ainda)
        printf("Nenhum %s cadastrado ainda.\n", tipo); // Mensagem de aviso
        return 0;                              // Retorna 0 (falha no login)
    }

    printf("=== Login de %s ===\n", tipo);     // Cabeçalho do login
    printf("Digite o usuário: ");              // Solicita o nome de usuário
    fgets(usuario, sizeof(usuario), stdin);    // Lê o nome digitado
    usuario[strcspn(usuario, "\n")] = 0;       // Remove o '\n'

    printf("Digite a senha: ");                // Solicita a senha
    fgets(senha, sizeof(senha), stdin);        // Lê a senha digitada
    senha[strcspn(senha, "\n")] = 0;           // Remove o '\n'

    // Loop para percorrer o arquivo linha por linha
    while (fgets(linha, sizeof(linha), f)) {   // Lê cada linha até o final (EOF)
        // Divide a linha em duas partes: antes e depois dos dois-pontos ':'
        sscanf(linha, "%[^:]:%[^\n]", u.usuario, u.senha); // Copia até ':' para u.usuario e depois para u.senha

        // Compara se o usuário e senha digitados coincidem com os do arquivo
        if (strcmp(usuario, u.usuario) == 0 && strcmp(senha, u.senha) == 0) {
            logado = 1;                        // Marca como logado
            break;                             // Sai do loop, pois já encontrou o usuário
        }
    }

    fclose(f);                                 // Fecha o arquivo após leitura

    if (logado) {                              // Se o login foi bem-sucedido
        printf("Login realizado com sucesso! Bem-vindo(a), %s (%s)\n", usuario, tipo);
        return 1;                              // Retorna 1 indicando sucesso
    } else {                                   // Caso contrário...
        printf("Usuário ou senha incorretos.\n");
        return 0;                              // Retorna 0 (falha)
    }
}

// ============================================================================
// MENU PRINCIPAL — ESCOLHA ENTRE ALUNO OU PROFESSOR
// ============================================================================

void menuTipoUsuario() {
    int opcao;                                 // Variável para armazenar a opção escolhida
    char buffer[10];                           // Buffer temporário para leitura da opção como string

    do {                                       // Loop que mantém o menu até o usuário decidir sair
        printf("\n==============================\n");
        printf("====== Portal EduGate ======\n");
        printf("==============================\n");
        printf("1. Login de Aluno\n");         // Opção 1 — Login de Aluno
        printf("2. Cadastro de Aluno\n");      // Opção 2 — Cadastro de Aluno
        printf("3. Login de Professor\n");     // Opção 3 — Login de Professor
        printf("4. Cadastro de Professor\n");  // Opção 4 — Cadastro de Professor
        printf("5. Sair\n");                   // Opção 5 — Encerrar o programa
        printf("Escolha uma opção: ");         // Solicita escolha

        fgets(buffer, sizeof(buffer), stdin);  // Lê a opção digitada como string
        opcao = atoi(buffer);                  // Converte para inteiro (ex: "2" → 2)

        limparTela();                          // Limpa a tela após cada escolha

        switch (opcao) {                       // Verifica qual opção foi escolhida
            case 1:
                loginUsuario("alunos.txt", "Aluno");       // Login de aluno
                break;
            case 2:
                cadastrarUsuario("alunos.txt", "Aluno");   // Cadastro de aluno
                break;
            case 3:
                loginUsuario("professores.txt", "Professor"); // Login de professor
                break;
            case 4:
                cadastrarUsuario("professores.txt", "Professor"); // Cadastro de professor
                break;
            case 5:
                printf("Saindo do sistema...\n");          // Mensagem de encerramento
                break;
            default:
                printf("Opção inválida! Tente novamente.\n"); // Mensagem de erro
        }
    } while (opcao != 5);                      // Repete o menu até que a opção 5 seja escolhida
}

// ============================================================================
// FUNÇÃO PRINCIPAL (main)
// ============================================================================

int main() {
    configurarCodificacao();                   // Garante que os acentos apareçam corretamente
    limparTela();                              // Limpa a tela antes de começar
    menuTipoUsuario();                         // Chama o menu principal (portal EduGate)
    return 0;                                  // Encerra o programa com código de sucesso
}
