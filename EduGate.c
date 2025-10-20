// ============================================================================ 
// BIBLIOTECAS UTILIZADAS 
// ============================================================================ 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <windows.h>

void cadastrarTurma(); // Declaração antecipada da função
void cadastrarAlunoEmTurma(); // Nova declaração
void listarAlunosCadastrados(); // Nova função para listar alunos
void listarAlunosDaTurma(); // Declaração antecipada

#define MAX 100

// ============================================================================ 
// DEFINIÇÃO DE ESTRUTURAS
// ============================================================================ 
typedef struct {
    char usuario[50];
    char senha[50];
} Usuario;

typedef struct {
    char codigo[50];
    char nome[100];
} Turma;

typedef struct {
    char usuario[50];
    char codigoTurma[50];
} AlunoTurma;

// ============================================================================ 
// CONFIGURAÇÃO DE CODIFICAÇÃO (UTF-8) 
// ============================================================================ 
void configurarCodificacao() {
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    #endif
    setlocale(LC_ALL, "pt_BR.UTF-8");
}

// ============================================================================ 
// FUNÇÃO PARA LIMPAR A TELA 
// ============================================================================ 
void limparTela() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

// ============================================================================ 
// FUNÇÃO PARA PAUSAR E VOLTAR AO MENU
// ============================================================================ 
void voltarMenu() {
    printf("\nPressione Enter para voltar ao menu...");
    getchar(); // Aguarda o usuário pressionar Enter
    limparTela();
}

// ============================================================================ 
// FUNÇÕES AUXILIARES
// ============================================================================ 

// Função para verificar se um aluno existe no sistema
int alunoExiste(const char *usuarioAluno) {
    FILE *f = fopen("alunos.txt", "r");
    if (!f) return 0;
    
    char linha[200];
    Usuario u;
    int existe = 0;
    
    while (fgets(linha, sizeof(linha), f)) {
        sscanf(linha, "%[^:]:%[^\n]", u.usuario, u.senha);
        if (strcmp(usuarioAluno, u.usuario) == 0) {
            existe = 1;
            break;
        }
    }
    fclose(f);
    return existe;
}

// Função para listar todos os alunos cadastrados (apenas nomes)
void listarAlunosCadastrados() {
    FILE *f = fopen("alunos.txt", "r");
    if (!f) {
        printf("Nenhum aluno cadastrado no sistema.\n");
        voltarMenu();
        return;
    }
    
    printf("\n=== Alunos Cadastrados no Sistema ===\n");
    char linha[200];
    Usuario u;
    int count = 0;
    
    while (fgets(linha, sizeof(linha), f)) {
        sscanf(linha, "%[^:]:%[^\n]", u.usuario, u.senha);
        printf("%d. %s\n", ++count, u.usuario);
    }
    
    if (count == 0) {
        printf("Nenhum aluno encontrado.\n");
    } else {
        printf("\nTotal de alunos cadastrados: %d\n", count);
    }
    
    fclose(f);
    voltarMenu();
}

// Função para listar todas as turmas cadastradas
void listarTurmas() {
    FILE *f = fopen("turmas.txt", "r");
    if (!f) {
        printf("Nenhuma turma cadastrada.\n");
        return;
    }
    
    printf("\n=== Turmas Cadastradas ===\n");
    char linha[200];
    int count = 0;
    
    while (fgets(linha, sizeof(linha), f)) {
        printf("%d. %s", ++count, linha);
    }
    
    if (count == 0) {
        printf("Nenhuma turma encontrada.\n");
    }
    
    fclose(f);
}

// Função para verificar se uma turma existe
int turmaExiste(const char *codigoTurma) {
    FILE *f = fopen("turmas.txt", "r");
    if (!f) return 0;
    
    char linha[200];
    int existe = 0;
    
    while (fgets(linha, sizeof(linha), f)) {
        if (strstr(linha, codigoTurma) != NULL) {
            existe = 1;
            break;
        }
    }
    fclose(f);
    return existe;
}

// ============================================================================ 
// FUNÇÃO PARA CADASTRAR TURMA (USO DO PROFESSOR) 
// ============================================================================ 
void cadastrarTurma() {
    int opcao;
    char buffer[10];
    
    do {
        printf("\n=== Cadastro de Turma ===\n");
        printf("1. Nova turma\n");
        printf("2. Listar turmas existentes\n");
        printf("3. Voltar ao menu do professor\n");
        printf("Escolha uma opção: ");
        fgets(buffer, sizeof(buffer), stdin);
        opcao = atoi(buffer);
        
        switch (opcao) {
            case 1: {
                FILE *f = fopen("turmas.txt", "a");
                if (!f) {
                    printf("Erro ao abrir o arquivo de turmas!\n");
                    break;
                }
                
                char nomeTurma[100];
                char codigoTurma[50];
                
                printf("\n--- Nova Turma ---\n");
                printf("Digite o nome da turma: ");
                fgets(nomeTurma, sizeof(nomeTurma), stdin);
                nomeTurma[strcspn(nomeTurma, "\n")] = 0;
                
                printf("Digite o código da turma: ");
                fgets(codigoTurma, sizeof(codigoTurma), stdin);
                codigoTurma[strcspn(codigoTurma, "\n")] = 0;
                
                fprintf(f, "Código: %s | Nome: %s\n", codigoTurma, nomeTurma);
                fclose(f);
                
                printf("Turma cadastrada com sucesso!\n");
                voltarMenu();
                break;
            }
            case 2:
                limparTela();
                listarTurmas();
                voltarMenu();
                break;
            case 3:
                limparTela();
                printf("Voltando ao menu do professor...\n");
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 3);
}

// ============================================================================ 
// FUNÇÃO PARA CADASTRAR ALUNO EM TURMA
// ============================================================================ 
void cadastrarAlunoEmTurma() {
    int opcao;
    char buffer[10];
    
    do {
        printf("\n=== Cadastrar Aluno em Turma ===\n");
        printf("1. Vincular aluno a turma\n");
        printf("2. Listar turmas disponíveis\n");
        printf("3. Listar alunos cadastrados\n");
        printf("4. Voltar ao menu do professor\n");
        printf("Escolha uma opção: ");
        fgets(buffer, sizeof(buffer), stdin);
        opcao = atoi(buffer);
        
        switch (opcao) {
            case 1: {
                // Verificar se existem turmas cadastradas
                FILE *f_turmas = fopen("turmas.txt", "r");
                if (!f_turmas) {
                    printf("Erro: Nenhuma turma cadastrada. Cadastre uma turma primeiro.\n");
                    voltarMenu();
                    break;
                }
                fclose(f_turmas);
                
                // Verificar se existem alunos cadastrados
                FILE *f_alunos = fopen("alunos.txt", "r");
                if (!f_alunos) {
                    printf("Erro: Nenhum aluno cadastrado no sistema.\n");
                    voltarMenu();
                    break;
                }
                fclose(f_alunos);
                
                char usuarioAluno[50];
                char codigoTurma[50];
                
                // Listar turmas disponíveis
                listarTurmas();
                
                // Solicitar código da turma
                printf("\nDigite o código da turma: ");
                fgets(codigoTurma, sizeof(codigoTurma), stdin);
                codigoTurma[strcspn(codigoTurma, "\n")] = 0;
                
                // Verificar se a turma existe
                if (!turmaExiste(codigoTurma)) {
                    printf("Erro: Turma não encontrada!\n");
                    voltarMenu();
                    break;
                }
                
                // Mostrar alunos cadastrados para referência
                printf("\n");
                FILE *f_alunos_list = fopen("alunos.txt", "r");
                if (f_alunos_list) {
                    printf("=== Alunos Disponíveis ===\n");
                    char linha[200];
                    Usuario u;
                    int count = 0;
                    
                    while (fgets(linha, sizeof(linha), f_alunos_list)) {
                        sscanf(linha, "%[^:]:%[^\n]", u.usuario, u.senha);
                        printf("%d. %s\n", ++count, u.usuario);
                    }
                    fclose(f_alunos_list);
                }
                
                // Solicitar usuário do aluno
                printf("\nDigite o nome de usuário do aluno: ");
                fgets(usuarioAluno, sizeof(usuarioAluno), stdin);
                usuarioAluno[strcspn(usuarioAluno, "\n")] = 0;
                
                // Verificar se o aluno existe
                if (!alunoExiste(usuarioAluno)) {
                    printf("Erro: Aluno não encontrado no sistema!\n");
                    voltarMenu();
                    break;
                }
                
                // Verificar se o aluno já está cadastrado nesta turma
                FILE *f_relacao = fopen("alunos_turmas.txt", "r");
                if (f_relacao) {
                    char linha[200];
                    AlunoTurma at;
                    int jaCadastrado = 0;
                    
                    while (fgets(linha, sizeof(linha), f_relacao)) {
                        sscanf(linha, "%[^:]:%[^\n]", at.usuario, at.codigoTurma);
                        if (strcmp(at.usuario, usuarioAluno) == 0 && strcmp(at.codigoTurma, codigoTurma) == 0) {
                            jaCadastrado = 1;
                            break;
                        }
                    }
                    fclose(f_relacao);
                    
                    if (jaCadastrado) {
                        printf("Erro: Este aluno já está cadastrado nesta turma!\n");
                        voltarMenu();
                        break;
                    }
                }
                
                // Cadastrar aluno na turma
                f_relacao = fopen("alunos_turmas.txt", "a");
                if (!f_relacao) {
                    printf("Erro ao abrir arquivo de relação alunos-turmas!\n");
                    voltarMenu();
                    break;
                }
                
                fprintf(f_relacao, "%s:%s\n", usuarioAluno, codigoTurma);
                fclose(f_relacao);
                
                printf("Aluno '%s' cadastrado com sucesso na turma '%s'!\n", usuarioAluno, codigoTurma);
                voltarMenu();
                break;
            }
            case 2:
                limparTela();
                listarTurmas();
                voltarMenu();
                break;
            case 3:
                limparTela();
                listarAlunosCadastrados();
                break;
            case 4:
                limparTela();
                printf("Voltando ao menu do professor...\n");
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 4);
}

// ============================================================================ 
// FUNÇÃO PARA LISTAR ALUNOS DA TURMA
// ============================================================================ 
void listarAlunosDaTurma() {
    int opcao;
    char buffer[10];
    
    do {
        printf("\n=== Relação de Alunos por Turmas ===\n");
        printf("1. Listar alunos de uma turma específica\n");
        printf("2. Listar todas as turmas\n");
        printf("3. Voltar ao menu do professor\n");
        printf("Escolha uma opção: ");
        fgets(buffer, sizeof(buffer), stdin);
        opcao = atoi(buffer);
        
        switch (opcao) {
            case 1: {
                char codigoTurma[50];
                
                printf("\nDigite o código da turma: ");
                fgets(codigoTurma, sizeof(codigoTurma), stdin);
                codigoTurma[strcspn(codigoTurma, "\n")] = 0;
                
                if (!turmaExiste(codigoTurma)) {
                    printf("Erro: Turma não encontrada!\n");
                    voltarMenu();
                    break;
                }
                
                FILE *f = fopen("alunos_turmas.txt", "r");
                if (!f) {
                    printf("Nenhum aluno cadastrado em turmas.\n");
                    voltarMenu();
                    break;
                }
                
                printf("\n=== Alunos da Turma %s ===\n", codigoTurma);
                char linha[200];
                AlunoTurma at;
                int count = 0;
                
                while (fgets(linha, sizeof(linha), f)) {
                    sscanf(linha, "%[^:]:%[^\n]", at.usuario, at.codigoTurma);
                    if (strcmp(at.codigoTurma, codigoTurma) == 0) {
                        printf("%d. %s\n", ++count, at.usuario);
                    }
                }
                
                if (count == 0) {
                    printf("Nenhum aluno encontrado nesta turma.\n");
                } else {
                    printf("\nTotal de alunos na turma: %d\n", count);
                }
                
                fclose(f);
                voltarMenu();
                break;
            }
            case 2:
                limparTela();
                listarTurmas();
                voltarMenu();
                break;
            case 3:
                limparTela();
                printf("Voltando ao menu do professor...\n");
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 3);
}

// ============================================================================ 
// TELA DO PROFESSOR (PÓS-LOGIN) 
// ============================================================================ 
void menuProfessor() {
    int opcao;
    char buffer[10];
    
    do {
        printf("\n====================================\n");
        printf(" Portal do Professor - EduGate \n");
        printf("====================================\n");
        printf("1. Cadastrar turma\n");
        printf("2. Cadastrar aluno em turma\n");
        printf("3. Registrar aula (diário eletrônico)\n");
        printf("4. Enviar atividade\n");
        printf("5. Consultar atividades\n");
        printf("--\n");
        printf("6. Relação de Alunos por Turmas\n");
        printf("7. Relação de Alunos\n");
        printf("--\n");
        printf("8. Sair\n");
        printf("Escolha uma opção: ");
        fgets(buffer, sizeof(buffer), stdin);
        opcao = atoi(buffer);
        limparTela();
        
        switch (opcao) {
            case 1:
                cadastrarTurma();
                break;
            case 2:
                cadastrarAlunoEmTurma();
                break;
            case 3:
                printf("[Em desenvolvimento] Registro de aula (diário eletrônico).\n");
                break;
            case 4:
                printf("[Em desenvolvimento] Enviar Atividade.\n");
                break;
            case 5:
                printf("[Em desenvolvimento] Consultar Atividades.\n");
                break;
            case 6:
                listarAlunosDaTurma();
                break;
            case 7:
                listarAlunosCadastrados();
                break;
            case 8:
                printf("Saindo do Portal do Professor...\n");
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 8);
}
// ============================================================================ 
// FUNÇÃO PARA CADASTRAR USUÁRIO (ALUNO OU PROFESSOR) 
// ============================================================================ 
void cadastrarUsuario(const char *arquivo, const char *tipo) {
    Usuario u;
    FILE *f = fopen(arquivo, "a");
    if (!f) {
        printf("Erro ao abrir o arquivo de %s!\n", tipo);
        return;
    }
    
    printf("=== Cadastro de %s ===\n", tipo);
    printf("Digite o nome de usuário: ");
    fgets(u.usuario, sizeof(u.usuario), stdin);
    u.usuario[strcspn(u.usuario, "\n")] = 0;
    
    printf("Digite a senha: ");
    fgets(u.senha, sizeof(u.senha), stdin);
    u.senha[strcspn(u.senha, "\n")] = 0;
    
    fprintf(f, "%s:%s\n", u.usuario, u.senha);
    fclose(f);
    
    printf("%s cadastrado(a) com sucesso!\n", tipo);
}

// ============================================================================ 
// FUNÇÃO PARA LOGIN DE USUÁRIO (ALUNO OU PROFESSOR) 
// ============================================================================ 
int loginUsuario(const char *arquivo, const char *tipo) {
    Usuario u;
    char usuario[50], senha[50];
    char linha[200];
    int logado = 0;
    FILE *f = fopen(arquivo, "r");
    
    if (!f) {
        printf("Nenhum %s cadastrado ainda.\n", tipo);
        return 0;
    }
    
    printf("=== Login de %s ===\n", tipo);
    printf("Digite o usuário: ");
    fgets(usuario, sizeof(usuario), stdin);
    usuario[strcspn(usuario, "\n")] = 0;
    
    printf("Digite a senha: ");
    fgets(senha, sizeof(senha), stdin);
    senha[strcspn(senha, "\n")] = 0;
    
    while (fgets(linha, sizeof(linha), f)) {
        sscanf(linha, "%[^:]:%[^\n]", u.usuario, u.senha);
        if (strcmp(usuario, u.usuario) == 0 && strcmp(senha, u.senha) == 0) {
            logado = 1;
            break;
        }
    }
    fclose(f);
    
    if (logado) {
        printf("Login realizado com sucesso! Bem-vindo(a), %s (%s)\n", usuario, tipo);
        if (strcmp(tipo, "Professor") == 0) {
            menuProfessor();
        }
        return 1;
    } else {
        printf("Usuário ou senha incorretos.\n");
        return 0;
    }
}

// ============================================================================ 
// MENU PRINCIPAL — ESCOLHA ENTRE ALUNO OU PROFESSOR 
// ============================================================================ 
void menuTipoUsuario() {
    int opcao;
    char buffer[10];
    
    do {
        printf("\n==============================\n");
        printf("====== Portal EduGate ======\n");
        printf("==============================\n");
        printf("1. Login de Aluno\n");
        printf("2. Cadastro de Aluno\n");
        printf("3. Login de Professor\n");
        printf("4. Cadastro de Professor\n");
        printf("5. Sair\n");
        printf("Escolha uma opção: ");
        fgets(buffer, sizeof(buffer), stdin);
        opcao = atoi(buffer);
        limparTela();
        
        switch (opcao) {
            case 1:
                loginUsuario("alunos.txt", "Aluno");
                break;
            case 2:
                cadastrarUsuario("alunos.txt", "Aluno");
                break;
            case 3:
                loginUsuario("professores.txt", "Professor");
                break;
            case 4:
                cadastrarUsuario("professores.txt", "Professor");
                break;
            case 5:
                printf("Saindo do sistema...\n");
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 5);
}

// ============================================================================ 
// FUNÇÃO PRINCIPAL (main) 
// ============================================================================ 
int main() {
    configurarCodificacao();
    limparTela();
    menuTipoUsuario();
    return 0;
}