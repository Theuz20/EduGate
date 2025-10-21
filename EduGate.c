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
void excluirTurma();              // Nova função para excluir turma
void excluirAlunoDeTurma();       // Nova função para excluir aluno da turma
void registrarAula();       // Função para registra aula (diario eletronico)
void excluirRegistroAula(); // Função para excluir registros de aula
void enviarAtividade();      // Função principal de envio/listagem/exclusão de atividades
void excluirAtividade();     // Função para excluir uma atividade específica



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
// FUNÇÃO PARA EXCLUIR ATIVIDADE
// ============================================================================
void excluirAtividade() {
    FILE *f = fopen("atividades.txt", "r");
    if (!f) {
        printf("Nenhuma atividade cadastrada.\n");
        voltarMenu();
        return;
    }

    printf("\n=== Exclusão de Atividade ===\n");

    char linha[300];
    int count = 0;
    printf("\n--- Atividades Existentes ---\n");
    while (fgets(linha, sizeof(linha), f)) {
        printf("%d. %s", ++count, linha);
    }

    if (count == 0) {
        printf("Nenhuma atividade para excluir.\n");
        fclose(f);
        voltarMenu();
        return;
    }
    fclose(f);

    char codigoTurma[50];
    char titulo[100];

    printf("\nDigite o código da turma: ");
    fgets(codigoTurma, sizeof(codigoTurma), stdin);
    codigoTurma[strcspn(codigoTurma, "\n")] = 0;

    printf("Digite o título da atividade: ");
    fgets(titulo, sizeof(titulo), stdin);
    titulo[strcspn(titulo, "\n")] = 0;

    f = fopen("atividades.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!f || !temp) {
        printf("Erro ao abrir arquivos temporários!\n");
        if (f) fclose(f);
        if (temp) fclose(temp);
        voltarMenu();
        return;
    }

    int removida = 0;
    while (fgets(linha, sizeof(linha), f)) {
        if (strstr(linha, codigoTurma) && strstr(linha, titulo)) {
            removida = 1; // não copia essa linha
            continue;
        }
        fputs(linha, temp);
    }

    fclose(f);
    fclose(temp);
    remove("atividades.txt");
    rename("temp.txt", "atividades.txt");

    if (removida)
        printf("Atividade '%s' da turma '%s' excluída com sucesso!\n", titulo, codigoTurma);
    else
        printf("Atividade não encontrada.\n");

    voltarMenu();
}

// ============================================================================
// FUNÇÃO PARA ENVIAR, LISTAR OU EXCLUIR ATIVIDADE
// ============================================================================
void enviarAtividade() {
    int opcao;
    char buffer[10];

    do {
        printf("\n=== Gerenciamento de Atividades ===\n");
        printf("1. Enviar nova atividade\n");
        printf("2. Listar atividades cadastradas\n");
        printf("3. Excluir atividade\n");
        printf("4. Voltar ao menu do professor\n");
        printf("Escolha uma opção: ");
        fgets(buffer, sizeof(buffer), stdin);
        opcao = atoi(buffer);
        limparTela();

        switch (opcao) {
            case 1: {
                FILE *f_turmas = fopen("turmas.txt", "r");
                if (!f_turmas) {
                    printf("Nenhuma turma cadastrada. Cadastre uma turma primeiro.\n");
                    voltarMenu();
                    break;
                }
                fclose(f_turmas);

                char codigoTurma[50];
                char titulo[100];
                char descricao[200];
                char prazo[20];

                printf("\n=== Envio de Nova Atividade ===\n");
                listarTurmas();

                printf("\nDigite o código da turma: ");
                fgets(codigoTurma, sizeof(codigoTurma), stdin);
                codigoTurma[strcspn(codigoTurma, "\n")] = 0;

                if (!turmaExiste(codigoTurma)) {
                    printf("Erro: Turma não encontrada!\n");
                    voltarMenu();
                    break;
                }

                printf("Digite o título da atividade: ");
                fgets(titulo, sizeof(titulo), stdin);
                titulo[strcspn(titulo, "\n")] = 0;

                printf("Digite uma breve descrição: ");
                fgets(descricao, sizeof(descricao), stdin);
                descricao[strcspn(descricao, "\n")] = 0;

                printf("Digite o prazo de entrega (DD/MM/AAAA): ");
                fgets(prazo, sizeof(prazo), stdin);
                prazo[strcspn(prazo, "\n")] = 0;

                FILE *f = fopen("atividades.txt", "a");
                if (!f) {
                    printf("Erro ao abrir o arquivo de atividades!\n");
                    voltarMenu();
                    break;
                }

                fprintf(f, "Turma: %s | Título: %s | Descrição: %s | Prazo: %s\n",
                        codigoTurma, titulo, descricao, prazo);
                fclose(f);

                printf("\nAtividade enviada com sucesso!\n");
                voltarMenu();
                break;
            }

            case 2: {
                FILE *f = fopen("atividades.txt", "r");
                if (!f) {
                    printf("Nenhuma atividade cadastrada.\n");
                    voltarMenu();
                    break;
                }

                printf("\n=== Atividades Cadastradas ===\n");
                char linha[300];
                int count = 0;

                while (fgets(linha, sizeof(linha), f)) {
                    printf("%d. %s", ++count, linha);
                }

                if (count == 0)
                    printf("Nenhuma atividade registrada.\n");

                fclose(f);
                voltarMenu();
                break;
            }

            case 3:
                excluirAtividade();
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
// FUNÇÃO PARA EXCLUIR TURMA
// ============================================================================
void excluirTurma() {
    FILE *f = fopen("turmas.txt", "r");
    if (!f) {
        printf("Nenhuma turma cadastrada.\n");
        return;
    }

    listarTurmas(); // Mostra turmas antes de excluir
    char codigoExcluir[50];
    printf("\nDigite o código da turma a ser excluída: ");
    fgets(codigoExcluir, sizeof(codigoExcluir), stdin);
    codigoExcluir[strcspn(codigoExcluir, "\n")] = 0;

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        printf("Erro ao criar arquivo temporário!\n");
        fclose(f);
        return;
    }

    char linha[200];
    int excluida = 0;
    while (fgets(linha, sizeof(linha), f)) {
        if (strstr(linha, codigoExcluir) == NULL) {
            fputs(linha, temp); // Mantém linhas que não contêm o código
        } else {
            excluida = 1;
        }
    }

    fclose(f);
    fclose(temp);

    remove("turmas.txt");
    rename("temp.txt", "turmas.txt");

    // Remove vínculos da turma nos alunos_turmas.txt
    FILE *fa = fopen("alunos_turmas.txt", "r");
    FILE *ftemp = fopen("temp.txt", "w");
    if (fa && ftemp) {
        while (fgets(linha, sizeof(linha), fa)) {
            if (strstr(linha, codigoExcluir) == NULL)
                fputs(linha, ftemp);
        }
        fclose(fa);
        fclose(ftemp);
        remove("alunos_turmas.txt");
        rename("temp.txt", "alunos_turmas.txt");
    }

    if (excluida)
        printf("Turma '%s' excluída com sucesso!\n", codigoExcluir);
    else
        printf("Turma não encontrada.\n");
}

// ============================================================================
// FUNÇÃO PARA EXCLUIR ALUNO DE UMA TURMA
// ============================================================================
void excluirAlunoDeTurma() {
    FILE *f = fopen("alunos_turmas.txt", "r");
    if (!f) {
        printf("Nenhum aluno vinculado a turmas ainda.\n");
        return;
    }

    printf("\n=== Excluir Aluno de Turma ===\n");
    listarTurmas();

    char usuarioAluno[50];
    char codigoTurma[50];
    printf("\nDigite o nome do aluno: ");
    fgets(usuarioAluno, sizeof(usuarioAluno), stdin);
    usuarioAluno[strcspn(usuarioAluno, "\n")] = 0;

    printf("Digite o código da turma: ");
    fgets(codigoTurma, sizeof(codigoTurma), stdin);
    codigoTurma[strcspn(codigoTurma, "\n")] = 0;

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        printf("Erro ao criar arquivo temporário!\n");
        fclose(f);
        return;
    }

    char linha[200];
    AlunoTurma at;
    int removido = 0;

    while (fgets(linha, sizeof(linha), f)) {
        sscanf(linha, "%[^:]:%[^\n]", at.usuario, at.codigoTurma);
        if (strcmp(at.usuario, usuarioAluno) == 0 && strcmp(at.codigoTurma, codigoTurma) == 0) {
            removido = 1;
        } else {
            fputs(linha, temp);
        }
    }

    fclose(f);
    fclose(temp);

    remove("alunos_turmas.txt");
    rename("temp.txt", "alunos_turmas.txt");

    if (removido)
        printf("Aluno '%s' removido da turma '%s' com sucesso!\n", usuarioAluno, codigoTurma);
    else
        printf("Aluno ou turma não encontrados.\n");
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
        printf("3. Excluir turma\n");
        printf("4. Voltar ao menu do professor\n");
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
                excluirTurma();
                voltarMenu();
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 4);
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
        printf("4. Excluir aluno de turma\n");
        printf("5. Voltar ao menu do professor\n");
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
                excluirAlunoDeTurma();
                voltarMenu();
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 5);
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
// FUNÇÃO PARA REGISTRAR AULA (DIÁRIO ELETRÔNICO)
// ============================================================================
void registrarAula() {
    int opcao;
    char buffer[10];

    do {
        printf("\n=== Diário Eletrônico ===\n");
        printf("1. Registrar nova aula\n");
        printf("2. Listar aulas registradas\n");
        printf("3. Excluir registro de aula\n");
        printf("4. Voltar ao menu do professor\n");
        printf("Escolha uma opção: ");
        fgets(buffer, sizeof(buffer), stdin);
        opcao = atoi(buffer);
        limparTela();

        switch (opcao) {
            case 1: {
                FILE *f_turmas = fopen("turmas.txt", "r");
                if (!f_turmas) {
                    printf("Nenhuma turma cadastrada. Cadastre uma turma primeiro.\n");
                    voltarMenu();
                    break;
                }
                fclose(f_turmas);

                char codigoTurma[50];
                char data[20];
                char tema[100];
                char conteudo[200];

                printf("\n=== Registro de Aula ===\n");
                listarTurmas();

                printf("\nDigite o código da turma: ");
                fgets(codigoTurma, sizeof(codigoTurma), stdin);
                codigoTurma[strcspn(codigoTurma, "\n")] = 0;

                if (!turmaExiste(codigoTurma)) {
                    printf("Erro: Turma não encontrada!\n");
                    voltarMenu();
                    break;
                }

                printf("Digite a data da aula (DD/MM/AAAA): ");
                fgets(data, sizeof(data), stdin);
                data[strcspn(data, "\n")] = 0;

                printf("Digite o tema da aula: ");
                fgets(tema, sizeof(tema), stdin);
                tema[strcspn(tema, "\n")] = 0;

                printf("Digite o conteúdo abordado: ");
                fgets(conteudo, sizeof(conteudo), stdin);
                conteudo[strcspn(conteudo, "\n")] = 0;

                FILE *f = fopen("diario.txt", "a");
                if (!f) {
                    printf("Erro ao abrir o arquivo do diário!\n");
                    voltarMenu();
                    break;
                }

                fprintf(f, "Turma: %s | Data: %s | Tema: %s | Conteúdo: %s\n",
                        codigoTurma, data, tema, conteudo);
                fclose(f);

                printf("\nAula registrada com sucesso!\n");
                voltarMenu();
                break;
            }

            case 2: {
                FILE *f = fopen("diario.txt", "r");
                if (!f) {
                    printf("Nenhum registro encontrado.\n");
                    voltarMenu();
                    break;
                }

                printf("\n=== Aulas Registradas ===\n");
                char linha[300];
                int count = 0;
                while (fgets(linha, sizeof(linha), f)) {
                    printf("%d. %s", ++count, linha);
                }

                if (count == 0)
                    printf("Nenhuma aula registrada.\n");

                fclose(f);
                voltarMenu();
                break;
            }

            case 3:
                excluirRegistroAula();
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
// FUNÇÃO PARA EXCLUIR REGISTRO DE AULA
// ============================================================================
void excluirRegistroAula() {
    FILE *f = fopen("diario.txt", "r");
    if (!f) {
        printf("Nenhum registro de aula encontrado.\n");
        voltarMenu();
        return;
    }

    printf("\n=== Exclusão de Registro de Aula ===\n");

    // Mostrar os registros existentes
    char linha[300];
    int count = 0;
    printf("\n--- Registros Existentes ---\n");
    while (fgets(linha, sizeof(linha), f)) {
        printf("%d. %s", ++count, linha);
    }
    if (count == 0) {
        printf("Nenhum registro disponível.\n");
        fclose(f);
        voltarMenu();
        return;
    }
    fclose(f);

    char codigoTurma[50];
    char data[20];

    printf("\nDigite o código da turma da aula a excluir: ");
    fgets(codigoTurma, sizeof(codigoTurma), stdin);
    codigoTurma[strcspn(codigoTurma, "\n")] = 0;

    printf("Digite a data da aula (DD/MM/AAAA): ");
    fgets(data, sizeof(data), stdin);
    data[strcspn(data, "\n")] = 0;

    f = fopen("diario.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!f || !temp) {
        printf("Erro ao abrir arquivos!\n");
        if (f) fclose(f);
        if (temp) fclose(temp);
        voltarMenu();
        return;
    }

    int removido = 0;
    while (fgets(linha, sizeof(linha), f)) {
        if (strstr(linha, codigoTurma) && strstr(linha, data)) {
            removido = 1; // pula essa linha (não escreve no novo arquivo)
            continue;
        }
        fputs(linha, temp);
    }

    fclose(f);
    fclose(temp);

    remove("diario.txt");
    rename("temp.txt", "diario.txt");

    if (removido)
        printf("Registro da turma '%s' na data '%s' excluído com sucesso!\n", codigoTurma, data);
    else
        printf("Nenhum registro encontrado com essas informações.\n");

    voltarMenu();
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
                registrarAula();
                break;
            case 4:
                enviarAtividade();
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