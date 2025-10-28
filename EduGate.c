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
void listarTurmasDoAluno(const char *usuarioAluno);
void sairDeTurma(const char *usuarioAluno);
void verAtividadesAluno(const char *usuarioAluno);
void verAulasAgendadas(const char *usuarioAluno);
void verNotasAluno(const char *usuarioAluno);
void matricularEmTurma(const char *usuarioAluno);
void menuAluno(const char *usuarioAluno);
void verEntregasAlunos();
int selecionarTurmaPorNumero(char *codigoTurmaSelecionada);
void criarEstruturaPastas();

#define MAX 100

// ============================================================================ 
// DEFINIÇÃO DE ESTRUTURAS
// ============================================================================ 
typedef struct {
    char usuario[50];
    char senha[50];
    char matricula[20];
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
// FUNÇÃO PARA CRIAR ESTRUTURA DE PASTAS
// ============================================================================
void criarEstruturaPastas() {
    #ifdef _WIN32
    // Windows
    system("mkdir dados 2>nul");
    system("mkdir dados\\professores 2>nul");
    system("mkdir dados\\alunos 2>nul"); 
    system("mkdir dados\\cadastros 2>nul");
    system("mkdir dados\\entregas 2>nul");
    #else
    // Linux/Mac
    system("mkdir -p dados 2>/dev/null");
    system("mkdir -p dados/professores 2>/dev/null");
    system("mkdir -p dados/alunos 2>/dev/null");
    system("mkdir -p dados/cadastros 2>/dev/null");
    system("mkdir -p dados/entregas 2>/dev/null");
    #endif
    
    printf("Estrutura de pastas verificada/criada!\n");
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
    FILE *f = fopen("dados/cadastros/alunos.csv", "r");
    if (!f) return 0;
    
    char linha[200];
    Usuario u;
    int existe = 0;
    
    // Pular cabeçalho se existir
    fgets(linha, sizeof(linha), f);
    
    while (fgets(linha, sizeof(linha), f)) {
        sscanf(linha, "%[^,],%[^,],%[^\n]", u.usuario, u.senha, u.matricula);
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
    FILE *f = fopen("dados/cadastros/alunos.csv", "r");
    if (!f) {
        printf("Nenhum aluno cadastrado no sistema.\n");
        voltarMenu();
        return;
    }
    
    printf("\n=== Alunos Cadastrados no Sistema ===\n");
    char linha[200];
    Usuario u;
    int count = 0;
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f);
    
    while (fgets(linha, sizeof(linha), f)) {
        sscanf(linha, "%[^,],%[^,],%[^\n]", u.usuario, u.senha, u.matricula); 
        printf("%d. %s (Matrícula: %s)\n", ++count, u.usuario, u.matricula);  
    }
    
    if (count == 0) {
        printf("Nenhum aluno encontrado.\n");
    } else {
        printf("\nTotal de alunos cadastrados: %d\n", count);
    }
    
    fclose(f);
    voltarMenu();
}

void listarTurmas() {
    FILE *f = fopen("dados/professores/turmas.csv", "r");
    if (!f) {
        printf("Nenhuma turma cadastrada.\n");
        return;
    }
    
    printf("\n=== Turmas Cadastradas ===\n");
    char linha[200];
    Turma t;
    int count = 0;
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f);
    
    while (fgets(linha, sizeof(linha), f)) {
        sscanf(linha, "%[^,],%[^\n]", t.codigo, t.nome);
        printf("%d. Código: %s | Nome: %s\n", ++count, t.codigo, t.nome);
    }
    
    if (count == 0) {
        printf("Nenhuma turma encontrada.\n");
    }
    
    fclose(f);
}

// Função para verificar se uma turma existe
int turmaExiste(const char *codigoTurma) {
    FILE *f = fopen("dados/professores/turmas.csv", "r");
    if (!f) return 0;
    
    char linha[200];
    Turma t;
    int existe = 0;
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f);
    
    while (fgets(linha, sizeof(linha), f)) {
        sscanf(linha, "%[^,],%[^\n]", t.codigo, t.nome);
        if (strcmp(codigoTurma, t.codigo) == 0) {
            existe = 1;
            break;
        }
    }
    fclose(f);
    return existe;
}

// ============================================================================
// FUNÇÃO PARA SELECIONAR TURMA POR NÚMERO
// ============================================================================
int selecionarTurmaPorNumero(char *codigoTurmaSelecionada) {
    FILE *f = fopen("dados/professores/turmas.csv", "r");
    if (!f) {
        printf("Nenhuma turma cadastrada.\n");
        return 0;
    }
    
    printf("\n=== Selecionar Turma ===\n");
    char linha[200];
    Turma t;
    Turma turmas[50];
    int count = 0;
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f);
    
    while (fgets(linha, sizeof(linha), f) && count < 50) {
        sscanf(linha, "%[^,],%[^\n]", t.codigo, t.nome);
        turmas[count] = t;
        printf("%d. %s - %s\n", count + 1, t.codigo, t.nome);
        count++;
    }
    fclose(f);
    
    if (count == 0) {
        printf("Nenhuma turma encontrada.\n");
        return 0;
    }
    
    char buffer[10];
    printf("\nDigite o número da turma: ");
    fgets(buffer, sizeof(buffer), stdin);
    int opcao = atoi(buffer);
    
    if (opcao < 1 || opcao > count) {
        printf("Opção inválida!\n");
        return 0;
    }
    
    strcpy(codigoTurmaSelecionada, turmas[opcao - 1].codigo);
    return 1;
}

// ============================================================================
// FUNÇÃO PARA LISTAR TURMAS DISPONÍVEIS PARA MATRÍCULA
// ============================================================================
void listarTurmasDisponiveis(const char *usuarioAluno) {
    FILE *f_turmas = fopen("dados/professores/turmas.csv", "r");
    if (!f_turmas) {
        printf("Nenhuma turma disponível no momento.\n");
        return;
    }
    
    printf("\n=== Turmas Disponíveis para Matrícula ===\n");
    char linha[200];
    Turma t;
    int countDisponiveis = 0;
    int countMatriculadas = 0;
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f_turmas);
    
    printf("\n--- Turmas Disponíveis ---\n");
    while (fgets(linha, sizeof(linha), f_turmas)) {
        sscanf(linha, "%[^,],%[^\n]", t.codigo, t.nome);
        
        // Verificar se o aluno já está matriculado
        FILE *f_matriculas = fopen("dados/alunos/alunos_turmas.csv", "r");
        int jaMatriculado = 0;
        
        if (f_matriculas) {
            char linha_mat[200];
            AlunoTurma at;
            fgets(linha_mat, sizeof(linha_mat), f_matriculas); // pular cabeçalho
            
            while (fgets(linha_mat, sizeof(linha_mat), f_matriculas)) {
                sscanf(linha_mat, "%[^,],%[^\n]", at.usuario, at.codigoTurma);
                if (strcmp(at.usuario, usuarioAluno) == 0 && strcmp(at.codigoTurma, t.codigo) == 0) {
                    jaMatriculado = 1;
                    countMatriculadas++;
                    break;
                }
            }
            fclose(f_matriculas);
        }
        
        if (!jaMatriculado) {
            printf("%d. Código: %s | Nome: %s\n", ++countDisponiveis, t.codigo, t.nome);
        }
    }
    
    if (countDisponiveis == 0) {
        printf("Nenhuma turma disponível ou você já está matriculado em todas.\n");
    }
    
    printf("\nTotal de turmas disponíveis: %d\n", countDisponiveis);
    printf("Total de turmas matriculadas: %d\n", countMatriculadas);
    
    fclose(f_turmas);
}

// ============================================================================
// FUNÇÃO PARA ALUNO SE MATRICULAR EM TURMA
// ============================================================================
void matricularEmTurma(const char *usuarioAluno) {
    printf("\n=== Matricular em Turma ===\n");
    
    // Listar turmas disponíveis
    listarTurmasDisponiveis(usuarioAluno);
    
    char codigoTurma[50];
    printf("\nDigite o código da turma que deseja se matricular: ");
    fgets(codigoTurma, sizeof(codigoTurma), stdin);
    codigoTurma[strcspn(codigoTurma, "\n")] = 0;
    
    // Verificar se a turma existe
    if (!turmaExiste(codigoTurma)) {
        printf("Erro: Turma não encontrada!\n");
        return;
    }
    
    // Verificar se já está matriculado
    FILE *f_matriculas = fopen("dados/alunos/alunos_turmas.csv", "r");
    if (f_matriculas) {
        char linha[200];
        AlunoTurma at;
        int jaMatriculado = 0;
        
        // Pular cabeçalho
        fgets(linha, sizeof(linha), f_matriculas);
        
        while (fgets(linha, sizeof(linha), f_matriculas)) {
            sscanf(linha, "%[^,],%[^\n]", at.usuario, at.codigoTurma);
            if (strcmp(at.usuario, usuarioAluno) == 0 && strcmp(at.codigoTurma, codigoTurma) == 0) {
                jaMatriculado = 1;
                break;
            }
        }
        fclose(f_matriculas);
        
        if (jaMatriculado) {
            printf("Erro: Você já está matriculado nesta turma!\n");
            return;
        }
    }
    
    // Fazer a matrícula
    FILE *f = fopen("dados/alunos/alunos_turmas.csv", "a");
    if (!f) {
        printf("Erro ao processar matrícula!\n");
        return;
    }
    
    // Verificar se é o primeiro registro para adicionar cabeçalho
    fseek(f, 0, SEEK_END);
    if (ftell(f) == 0) {
        fprintf(f, "Usuario,CodigoTurma\n");
    }
    
    fprintf(f, "%s,%s\n", usuarioAluno, codigoTurma);
    fclose(f);
    
    printf("Matrícula realizada com sucesso na turma '%s'!\n", codigoTurma);
}

// ============================================================================
// FUNÇÃO PARA LISTAR TURMAS DO ALUNO
// ============================================================================
void listarTurmasDoAluno(const char *usuarioAluno) {
    FILE *f = fopen("dados/alunos/alunos_turmas.csv", "r");
    if (!f) {
        printf("Você não está matriculado em nenhuma turma.\n");
        return;
    }
    
    printf("\n=== Suas Turmas ===\n");
    char linha[200];
    AlunoTurma at;
    Turma t;
    int count = 0;
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f);
    
    while (fgets(linha, sizeof(linha), f)) {
        sscanf(linha, "%[^,],%[^\n]", at.usuario, at.codigoTurma);
        if (strcmp(at.usuario, usuarioAluno) == 0) {
            // Buscar informações da turma
            FILE *f_turmas = fopen("dados/professores/turmas.csv", "r");
            if (f_turmas) {
                char linha_turma[200];
                // Pular cabeçalho
                fgets(linha_turma, sizeof(linha_turma), f_turmas);
                
                while (fgets(linha_turma, sizeof(linha_turma), f_turmas)) {
                    sscanf(linha_turma, "%[^,],%[^\n]", t.codigo, t.nome);
                    if (strcmp(t.codigo, at.codigoTurma) == 0) {
                        printf("%d. %s - %s\n", ++count, t.codigo, t.nome);
                        break;
                    }
                }
                fclose(f_turmas);
            }
        }
    }
    
    if (count == 0) {
        printf("Você não está matriculado em nenhuma turma.\n");
    }
    
    fclose(f);
}

// ============================================================================
// FUNÇÃO PARA ALUNO ENVIAR ATIVIDADE
// ============================================================================
void enviarAtividadeAluno(const char *usuarioAluno) {
    printf("\n=== Enviar Atividade ===\n");
    
    // Primeiro verificar se o aluno está em alguma turma
    FILE *f_turmas_aluno = fopen("dados/alunos/alunos_turmas.csv", "r");
    if (!f_turmas_aluno) {
        printf("Você não está em nenhuma turma.\n");
        return;
    }
    
    // Coletar turmas do aluno
    char linha[300];
    AlunoTurma at;
    Turma turmasAluno[10];
    int numTurmas = 0;
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f_turmas_aluno);
    
    while (fgets(linha, sizeof(linha), f_turmas_aluno) && numTurmas < 10) {
        sscanf(linha, "%[^,],%[^\n]", at.usuario, at.codigoTurma);
        if (strcmp(at.usuario, usuarioAluno) == 0) {
            // Buscar nome da turma
            FILE *f_turmas = fopen("dados/professores/turmas.csv", "r");
            if (f_turmas) {
                char linha_turma[200];
                Turma t;
                // Pular cabeçalho
                fgets(linha_turma, sizeof(linha_turma), f_turmas);
                
                while (fgets(linha_turma, sizeof(linha_turma), f_turmas)) {
                    sscanf(linha_turma, "%[^,],%[^\n]", t.codigo, t.nome);
                    if (strcmp(t.codigo, at.codigoTurma) == 0) {
                        turmasAluno[numTurmas] = t;
                        numTurmas++;
                        break;
                    }
                }
                fclose(f_turmas);
            }
        }
    }
    fclose(f_turmas_aluno);
    
    if (numTurmas == 0) {
        printf("Você não está em nenhuma turma.\n");
        return;
    }
    
    // Mostrar turmas do aluno
    printf("\n=== Suas Turmas ===\n");
    for (int i = 0; i < numTurmas; i++) {
        printf("%d. %s - %s\n", i + 1, turmasAluno[i].codigo, turmasAluno[i].nome);
    }
    
    // Selecionar turma
    char buffer[10];
    printf("\nDigite o número da turma: ");
    fgets(buffer, sizeof(buffer), stdin);
    int opcaoTurma = atoi(buffer);
    
    if (opcaoTurma < 1 || opcaoTurma > numTurmas) {
        printf("Opção inválida!\n");
        return;
    }
    
    char codigoTurma[50];
    strcpy(codigoTurma, turmasAluno[opcaoTurma - 1].codigo);
    
    // Listar atividades da turma selecionada
    FILE *f_atividades = fopen("dados/professores/atividades.csv", "r");
    if (!f_atividades) {
        printf("Nenhuma atividade disponível nesta turma.\n");
        return;
    }
    
    printf("\n=== Atividades Disponíveis ===\n");
    char atividades[20][100];
    int numAtividades = 0;
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f_atividades);
    
    while (fgets(linha, sizeof(linha), f_atividades) && numAtividades < 20) {
        char codigo[50], titulo[100], descricao[200], prazo[20];
        sscanf(linha, "%[^,],%[^,],%[^,],%[^\n]", codigo, titulo, descricao, prazo);
        
        if (strcmp(codigo, codigoTurma) == 0) {
            printf("%d. %s\n", numAtividades + 1, titulo);
            printf("   Descrição: %s\n", descricao);
            printf("   Prazo: %s\n", prazo);
            strcpy(atividades[numAtividades], titulo);
            numAtividades++;
        }
    }
    fclose(f_atividades);
    
    if (numAtividades == 0) {
        printf("Nenhuma atividade disponível nesta turma.\n");
        return;
    }
    
    // Selecionar atividade
    printf("\nDigite o número da atividade: ");
    fgets(buffer, sizeof(buffer), stdin);
    int opcaoAtividade = atoi(buffer);
    
    if (opcaoAtividade < 1 || opcaoAtividade > numAtividades) {
        printf("Opção inválida!\n");
        return;
    }
    
    char tituloAtividade[100];
    strcpy(tituloAtividade, atividades[opcaoAtividade - 1]);
    
    // Verificar se já enviou esta atividade
    FILE *f_entregas = fopen("dados/entregas/entregas.csv", "r");
    if (f_entregas) {
        char linha_entrega[300];
        char usuario[50], turma[50], atividade[100], arquivo[100], data[20];
        
        // Pular cabeçalho
        fgets(linha_entrega, sizeof(linha_entrega), f_entregas);
        
        while (fgets(linha_entrega, sizeof(linha_entrega), f_entregas)) {
            sscanf(linha_entrega, "%[^,],%[^,],%[^,],%[^,],%[^\n]", usuario, turma, atividade, arquivo, data);
            if (strcmp(usuario, usuarioAluno) == 0 && 
                strcmp(turma, codigoTurma) == 0 && 
                strcmp(atividade, tituloAtividade) == 0) {
                printf("Você já enviou esta atividade em %s!\n", data);
                fclose(f_entregas);
                return;
            }
        }
        fclose(f_entregas);
    }
    
    // Coletar dados da entrega
    char nomeArquivo[100];
    char dataEntrega[20];
    
    printf("\nDigite o nome do arquivo da atividade: ");
    fgets(nomeArquivo, sizeof(nomeArquivo), stdin);
    nomeArquivo[strcspn(nomeArquivo, "\n")] = 0;
    
    printf("Digite a data de entrega (DD/MM/AAAA): ");
    fgets(dataEntrega, sizeof(dataEntrega), stdin);
    dataEntrega[strcspn(dataEntrega, "\n")] = 0;
    
    // Registrar a entrega
    f_entregas = fopen("dados/entregas/entregas.csv", "a");
    if (!f_entregas) {
        printf("Erro ao registrar entrega!\n");
        return;
    }
    
    // Verificar se é o primeiro registro para adicionar cabeçalho
    fseek(f_entregas, 0, SEEK_END);
    if (ftell(f_entregas) == 0) {
        fprintf(f_entregas, "Usuario,CodigoTurma,Atividade,Arquivo,DataEntrega\n");
    }
    
    fprintf(f_entregas, "%s,%s,%s,%s,%s\n", 
            usuarioAluno, codigoTurma, tituloAtividade, nomeArquivo, dataEntrega);
    fclose(f_entregas);
    
    printf("\nAtividade enviada com sucesso!\n");
    printf("Turma: %s\n", codigoTurma);
    printf("Atividade: %s\n", tituloAtividade);
    printf("Arquivo: %s\n", nomeArquivo);
    printf("Data de entrega: %s\n", dataEntrega);
}

// ============================================================================
// FUNÇÃO PARA SAIR DE UMA TURMA (ALUNO)
// ============================================================================
void sairDeTurma(const char *usuarioAluno) {
    FILE *f = fopen("dados/alunos/alunos_turmas.csv", "r");
    if (!f) {
        printf("Você não está em nenhuma turma.\n");
        return;
    }
    
    printf("\n=== Sair de Turma ===\n");
    listarTurmasDoAluno(usuarioAluno);
    
    char codigoTurma[50];
    printf("\nDigite o código da turma que deseja sair: ");
    fgets(codigoTurma, sizeof(codigoTurma), stdin);
    codigoTurma[strcspn(codigoTurma, "\n")] = 0;
    
    FILE *temp = fopen("dados/alunos/temp.csv", "w");
    if (!temp) {
        printf("Erro ao processar solicitação!\n");
        fclose(f);
        return;
    }
    
    char linha[200];
    AlunoTurma at;
    int removido = 0;
    
    // Copiar cabeçalho
    fgets(linha, sizeof(linha), f);
    fputs(linha, temp);
    
    while (fgets(linha, sizeof(linha), f)) {
        sscanf(linha, "%[^,],%[^\n]", at.usuario, at.codigoTurma);
        if (strcmp(at.usuario, usuarioAluno) == 0 && strcmp(at.codigoTurma, codigoTurma) == 0) {
            removido = 1; // Não copia esta linha (remove o vínculo)
        } else {
            fputs(linha, temp);
        }
    }
    
    fclose(f);
    fclose(temp);
    
    remove("dados/alunos/alunos_turmas.csv");
    rename("dados/alunos/temp.csv", "dados/alunos/alunos_turmas.csv");
    
    if (removido) {
        printf("Você saiu da turma '%s' com sucesso!\n", codigoTurma);
    } else {
        printf("Turma não encontrada ou você não está matriculado nela.\n");
    }
}
// ============================================================================
// FUNÇÃO PARA DECLARAR NOTAS DOS ALUNOS
// ============================================================================
void declararNotas() {
    printf("\n=== Declarar Notas ===\n");
    
    // Selecionar turma por número
    char codigoTurma[50];
    if (!selecionarTurmaPorNumero(codigoTurma)) {
        return;
    }
    
    // Listar alunos da turma
    FILE *f_alunos_turma = fopen("dados/alunos/alunos_turmas.csv", "r");
    if (!f_alunos_turma) {
        printf("Nenhum aluno matriculado nesta turma.\n");
        return;
    }
    
    printf("\n=== Alunos da Turma %s ===\n", codigoTurma);
    char linha[200];
    AlunoTurma at;
    int count = 0;
    char alunosTurma[50][50];
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f_alunos_turma);
    
    while (fgets(linha, sizeof(linha), f_alunos_turma)) {
        sscanf(linha, "%[^,],%[^\n]", at.usuario, at.codigoTurma);
        if (strcmp(at.codigoTurma, codigoTurma) == 0) {
            printf("%d. %s\n", ++count, at.usuario);
            strcpy(alunosTurma[count-1], at.usuario);
        }
    }
    fclose(f_alunos_turma);
    
    if (count == 0) {
        printf("Nenhum aluno encontrado nesta turma.\n");
        return;
    }
    
    // Listar atividades da turma
    FILE *f_atividades = fopen("dados/professores/atividades.csv", "r");
    if (!f_atividades) {
        printf("Nenhuma atividade cadastrada para esta turma.\n");
        return;
    }
    
    printf("\n=== Atividades da Turma ===\n");
    char atividades[20][100];
    int numAtividades = 0;
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f_atividades);
    
    while (fgets(linha, sizeof(linha), f_atividades) && numAtividades < 20) {
        char codigo[50], titulo[100], descricao[200], prazo[20];
        sscanf(linha, "%[^,],%[^,],%[^,],%[^\n]", codigo, titulo, descricao, prazo);
        
        if (strcmp(codigo, codigoTurma) == 0) {
            printf("%d. %s (Prazo: %s)\n", numAtividades + 1, titulo, prazo);
            strcpy(atividades[numAtividades], titulo);
            numAtividades++;
        }
    }

    fclose(f_atividades);
}
// ============================================================================
// FUNÇÃO PARA VER AULAS AGENDADAS DAS TURMAS DO ALUNO
// ============================================================================
void verAulasAgendadas(const char *usuarioAluno) {
    // Primeiro, listar as turmas do aluno
    FILE *f_turmas_aluno = fopen("dados/alunos/alunos_turmas.csv", "r");
    if (!f_turmas_aluno) {
        printf("Você não está em nenhuma turma para ver aulas.\n");
        return;
    }
    
    printf("\n=== Aulas Agendadas das Suas Turmas ===\n");
    
    char linha[300];
    AlunoTurma at;
    int temTurmas = 0;
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f_turmas_aluno);
    
    // Coletar turmas do aluno
    char turmasAluno[10][50];
    int numTurmas = 0;
    
    while (fgets(linha, sizeof(linha), f_turmas_aluno) && numTurmas < 10) {
        sscanf(linha, "%[^,],%[^\n]", at.usuario, at.codigoTurma);
        if (strcmp(at.usuario, usuarioAluno) == 0) {
            strcpy(turmasAluno[numTurmas], at.codigoTurma);
            numTurmas++;
            temTurmas = 1;
        }
    }
    fclose(f_turmas_aluno);
    
    if (!temTurmas) {
        printf("Você não está em nenhuma turma.\n");
        return;
    }
    
    // Buscar aulas dessas turmas
    FILE *f_aulas = fopen("dados/professores/diario.csv", "r");
    if (!f_aulas) {
        printf("Nenhuma aula agendada.\n");
        return;
    }
    
    int count = 0;
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f_aulas);
    
    while (fgets(linha, sizeof(linha), f_aulas)) {
        char codigoTurma[50], data[20], tema[100], conteudo[200];
        
        // Ler dados da aula
        sscanf(linha, "%[^,],%[^,],%[^,],%[^\n]", codigoTurma, data, tema, conteudo);
        
        // Verificar se esta aula é de uma turma do aluno
        for (int i = 0; i < numTurmas; i++) {
            if (strcmp(codigoTurma, turmasAluno[i]) == 0) {
                printf("\n--- Aula %d ---\n", ++count);
                printf("Turma: %s\n", codigoTurma);
                printf("Data: %s\n", data);
                printf("Tema: %s\n", tema);
                printf("Conteúdo: %s\n", conteudo);
                break;
            }
        }
    }
    
    if (count == 0) {
        printf("Nenhuma aula agendada nas suas turmas.\n");
    } else {
        printf("\nTotal de aulas agendadas: %d\n", count);
    }
    
    fclose(f_aulas);
}

// ============================================================================
// FUNÇÃO PARA VER ATIVIDADES DAS TURMAS DO ALUNO
// ============================================================================
void verAtividadesAluno(const char *usuarioAluno) {
    // Primeiro, listar as turmas do aluno
    FILE *f_turmas_aluno = fopen("dados/alunos/alunos_turmas.csv", "r");
    if (!f_turmas_aluno) {
        printf("Você não está em nenhuma turma para ver atividades.\n");
        return;
    }
    
    printf("\n=== Atividades das Suas Turmas ===\n");
    
    char linha[300];
    AlunoTurma at;
    int temTurmas = 0;
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f_turmas_aluno);
    
    // Coletar turmas do aluno
    char turmasAluno[10][50];
    int numTurmas = 0;
    
    while (fgets(linha, sizeof(linha), f_turmas_aluno) && numTurmas < 10) {
        sscanf(linha, "%[^,],%[^\n]", at.usuario, at.codigoTurma);
        if (strcmp(at.usuario, usuarioAluno) == 0) {
            strcpy(turmasAluno[numTurmas], at.codigoTurma);
            numTurmas++;
            temTurmas = 1;
        }
    }
    fclose(f_turmas_aluno);
    
    if (!temTurmas) {
        printf("Você não está em nenhuma turma.\n");
        return;
    }
    
    // Agora buscar atividades dessas turmas
    FILE *f_atividades = fopen("dados/professores/atividades.csv", "r");
    if (!f_atividades) {
        printf("Nenhuma atividade disponível.\n");
        return;
    }
    
    int count = 0;
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f_atividades);
    
    while (fgets(linha, sizeof(linha), f_atividades)) {
        char codigoTurma[50], titulo[100], descricao[200], prazo[20];
        
        // Ler dados da atividade
        sscanf(linha, "%[^,],%[^,],%[^,],%[^\n]", codigoTurma, titulo, descricao, prazo);
        
        // Verificar se esta atividade é de uma turma do aluno
        for (int i = 0; i < numTurmas; i++) {
            if (strcmp(codigoTurma, turmasAluno[i]) == 0) {
                printf("\n--- Atividade %d ---\n", ++count);
                printf("Turma: %s\n", codigoTurma);
                printf("Título: %s\n", titulo);
                printf("Descrição: %s\n", descricao);
                printf("Prazo: %s\n", prazo);
                printf("Status: Pendente\n");
                break;
            }
        }
    }
    
    if (count == 0) {
        printf("Nenhuma atividade disponível nas suas turmas.\n");
    } else {
        printf("\nTotal de atividades: %d\n", count);
    }
    
    fclose(f_atividades);
}

// ============================================================================
// FUNÇÃO PARA VER NOTAS (AGORA COM IMPLEMENTAÇÃO REAL)
// ============================================================================
void verNotasAluno(const char *usuarioAluno) {
    FILE *f = fopen("dados/alunos/notas.csv", "r");
    if (!f) {
        printf("Nenhuma nota registrada para você.\n");
        return;
    }
    
    printf("\n=== Suas Notas ===\n");
    char linha[300];
    int count = 0;
    float somaNotas = 0;
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f);
    
    while (fgets(linha, sizeof(linha), f)) {
        char usuario[50], codigoTurma[50], atividade[100], nota[10], data[20];
        sscanf(linha, "%[^,],%[^,],%[^,],%[^,],%[^\n]", usuario, codigoTurma, atividade, nota, data);
        
        if (strcmp(usuario, usuarioAluno) == 0) {
            printf("\n--- Nota %d ---\n", ++count);
            printf("Turma: %s\n", codigoTurma);
            printf("Atividade: %s\n", atividade);
            printf("Nota: %s\n", nota);
            printf("Data: %s\n", data);
            
            somaNotas += atof(nota);
        }
    }
    
    fclose(f);
    
    if (count == 0) {
        printf("Nenhuma nota encontrada para você.\n");
    } else {
        printf("\n=== Resumo ===\n");
        printf("Total de atividades avaliadas: %d\n", count);
        printf("Média geral: %.1f\n", somaNotas / count);
    }
}
// ============================================================================
// FUNÇÃO PARA GERAR MATRÍCULA AUTOMÁTICA
// ============================================================================
void gerarMatricula(char *matricula, const char *arquivo) {
    int ultimaMatricula = 0;
    FILE *f = fopen(arquivo, "r");
    
    if (f) {
        char linha[200];
        Usuario u;
        
        // Pular cabeçalho
        fgets(linha, sizeof(linha), f);
        
        // Encontrar a última matrícula usada
        while (fgets(linha, sizeof(linha), f)) {
            sscanf(linha, "%[^,],%[^,],%[^\n]", u.usuario, u.senha, u.matricula);
            int num = atoi(u.matricula);
            if (num > ultimaMatricula) {
                ultimaMatricula = num;
            }
        }
        fclose(f);
    }
    
    // Gerar nova matrícula (sequencial)
    ultimaMatricula++;
    sprintf(matricula, "%08d", ultimaMatricula); // Formato: 00000001, 00000002, etc.
}

// ============================================================================
// FUNÇÃO PARA BUSCAR USUÁRIO POR MATRÍCULA
// ============================================================================
int buscarPorMatricula(const char *matricula, const char *arquivo, Usuario *usuarioEncontrado) {
    FILE *f = fopen(arquivo, "r");
    if (!f) return 0;
    
    char linha[200];
    int encontrado = 0;
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f);
    
    while (fgets(linha, sizeof(linha), f)) {
        sscanf(linha, "%[^,],%[^,],%[^\n]", usuarioEncontrado->usuario, 
               usuarioEncontrado->senha, usuarioEncontrado->matricula);
        if (strcmp(matricula, usuarioEncontrado->matricula) == 0) {
            encontrado = 1;
            break;
        }
    }
    fclose(f);
    return encontrado;
}

// ============================================================================
// FUNÇÃO PARA EXCLUIR ATIVIDADE
// ============================================================================
void excluirAtividade() {
    FILE *f = fopen("dados/professores/atividades.csv", "r");
    if (!f) {
        printf("Nenhuma atividade cadastrada.\n");
        voltarMenu();
        return;
    }

    printf("\n=== Exclusão de Atividade ===\n");

    char linha[300];
    int count = 0;
    printf("\n--- Atividades Existentes ---\n");
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f);
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

    f = fopen("dados/professores/atividades.csv", "r");
    FILE *temp = fopen("dados/professores/temp.csv", "w");
    if (!f || !temp) {
        printf("Erro ao abrir arquivos temporários!\n");
        if (f) fclose(f);
        if (temp) fclose(temp);
        voltarMenu();
        return;
    }

    // Copiar cabeçalho
    fgets(linha, sizeof(linha), f);
    fputs(linha, temp);

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
    remove("dados/professores/atividades.csv");
    rename("dados/professores/temp.csv", "dados/professores/atividades.csv");

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
                FILE *f_turmas = fopen("dados/professores/turmas.csv", "r");
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

                FILE *f = fopen("dados/professores/atividades.csv", "a");
                if (!f) {
                    printf("Erro ao abrir o arquivo de atividades!\n");
                    voltarMenu();
                    break;
                }

                // Verificar se é o primeiro registro para adicionar cabeçalho
                fseek(f, 0, SEEK_END);
                if (ftell(f) == 0) {
                    fprintf(f, "CodigoTurma,Titulo,Descricao,Prazo\n");
                }

                fprintf(f, "%s,%s,%s,%s\n", codigoTurma, titulo, descricao, prazo);
                fclose(f);

                printf("\nAtividade enviada com sucesso!\n");
                voltarMenu();
                break;
            }

            case 2: {
                FILE *f = fopen("dados/professores/atividades.csv", "r");
                if (!f) {
                    printf("Nenhuma atividade cadastrada.\n");
                    voltarMenu();
                    break;
                }

                printf("\n=== Atividades Cadastradas ===\n");
                char linha[300];
                int count = 0;

                // Pular cabeçalho
                fgets(linha, sizeof(linha), f);
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
    FILE *f = fopen("dados/professores/turmas.csv", "r");
    if (!f) {
        printf("Nenhuma turma cadastrada.\n");
        return;
    }

    listarTurmas(); // Mostra turmas antes de excluir
    char codigoExcluir[50];
    printf("\nDigite o código da turma a ser excluída: ");
    fgets(codigoExcluir, sizeof(codigoExcluir), stdin);
    codigoExcluir[strcspn(codigoExcluir, "\n")] = 0;

    FILE *temp = fopen("dados/professores/temp.csv", "w");
    if (!temp) {
        printf("Erro ao criar arquivo temporário!\n");
        fclose(f);
        return;
    }

    char linha[200];
    int excluida = 0;
    
    // Copiar cabeçalho
    fgets(linha, sizeof(linha), f);
    fputs(linha, temp);
    
    while (fgets(linha, sizeof(linha), f)) {
        Turma t;
        sscanf(linha, "%[^,],%[^\n]", t.codigo, t.nome);
        if (strcmp(t.codigo, codigoExcluir) != 0) {
            fputs(linha, temp); // Mantém linhas que não contêm o código
        } else {
            excluida = 1;
        }
    }

    fclose(f);
    fclose(temp);

    remove("dados/professores/turmas.csv");
    rename("dados/professores/temp.csv", "dados/professores/turmas.csv");

    // Remove vínculos da turma nos alunos_turmas.csv
    FILE *fa = fopen("dados/alunos/alunos_turmas.csv", "r");
    FILE *ftemp = fopen("dados/alunos/temp.csv", "w");
    if (fa && ftemp) {
        // Copiar cabeçalho
        fgets(linha, sizeof(linha), fa);
        fputs(linha, ftemp);
        
        while (fgets(linha, sizeof(linha), fa)) {
            AlunoTurma at;
            sscanf(linha, "%[^,],%[^\n]", at.usuario, at.codigoTurma);
            if (strcmp(at.codigoTurma, codigoExcluir) != 0)
                fputs(linha, ftemp);
        }
        fclose(fa);
        fclose(ftemp);
        remove("dados/alunos/alunos_turmas.csv");
        rename("dados/alunos/temp.csv", "dados/alunos/alunos_turmas.csv");
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
    FILE *f = fopen("dados/alunos/alunos_turmas.csv", "r");
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

    FILE *temp = fopen("dados/alunos/temp.csv", "w");
    if (!temp) {
        printf("Erro ao criar arquivo temporário!\n");
        fclose(f);
        return;
    }

    char linha[200];
    AlunoTurma at;
    int removido = 0;

    // Copiar cabeçalho
    fgets(linha, sizeof(linha), f);
    fputs(linha, temp);

    while (fgets(linha, sizeof(linha), f)) {
        sscanf(linha, "%[^,],%[^\n]", at.usuario, at.codigoTurma);
        if (strcmp(at.usuario, usuarioAluno) == 0 && strcmp(at.codigoTurma, codigoTurma) == 0) {
            removido = 1;
        } else {
            fputs(linha, temp);
        }
    }

    fclose(f);
    fclose(temp);

    remove("dados/alunos/alunos_turmas.csv");
    rename("dados/alunos/temp.csv", "dados/alunos/alunos_turmas.csv");

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
                FILE *f = fopen("dados/professores/turmas.csv", "a");
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
                
                // Verificar se é o primeiro registro para adicionar cabeçalho
                fseek(f, 0, SEEK_END);
                if (ftell(f) == 0) {
                    fprintf(f, "Codigo,Nome\n");
                }
                
                fprintf(f, "%s,%s\n", codigoTurma, nomeTurma);
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
                FILE *f_turmas = fopen("dados/professores/turmas.csv", "r");
                if (!f_turmas) {
                    printf("Erro: Nenhuma turma cadastrada. Cadestre uma turma primeiro.\n");
                    voltarMenu();
                    break;
                }
                fclose(f_turmas);
                
                // Verificar se existem alunos cadastrados
                FILE *f_alunos = fopen("dados/cadastros/alunos.csv", "r");
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
                FILE *f_alunos_list = fopen("dados/cadastros/alunos.csv", "r");
                if (f_alunos_list) {
                    printf("=== Alunos Disponíveis ===\n");
                    char linha[200];
                    Usuario u;
                    int count = 0;
                    
                    // Pular cabeçalho
                    fgets(linha, sizeof(linha), f_alunos_list);
                    
                    while (fgets(linha, sizeof(linha), f_alunos_list)) {
                        sscanf(linha, "%[^,],%[^,],%[^\n]", u.usuario, u.senha, u.matricula);
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
                FILE *f_relacao = fopen("dados/alunos/alunos_turmas.csv", "r");
                if (f_relacao) {
                    char linha[200];
                    AlunoTurma at;
                    int jaCadastrado = 0;
                    
                    // Pular cabeçalho
                    fgets(linha, sizeof(linha), f_relacao);
                    
                    while (fgets(linha, sizeof(linha), f_relacao)) {
                        sscanf(linha, "%[^,],%[^\n]", at.usuario, at.codigoTurma);
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
                f_relacao = fopen("dados/alunos/alunos_turmas.csv", "a");
                if (!f_relacao) {
                    printf("Erro ao abrir arquivo de relação alunos-turmas!\n");
                    voltarMenu();
                    break;
                }
                
                // Verificar se é o primeiro registro para adicionar cabeçalho
                fseek(f_relacao, 0, SEEK_END);
                if (ftell(f_relacao) == 0) {
                    fprintf(f_relacao, "Usuario,CodigoTurma\n");
                }
                
                fprintf(f_relacao, "%s,%s\n", usuarioAluno, codigoTurma);
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
                
                FILE *f = fopen("dados/alunos/alunos_turmas.csv", "r");
                if (!f) {
                    printf("Nenhum aluno cadastrado em turmas.\n");
                    voltarMenu();
                    break;
                }
                
                printf("\n=== Alunos da Turma %s ===\n", codigoTurma);
                char linha[200];
                AlunoTurma at;
                int count = 0;
                
                // Pular cabeçalho
                fgets(linha, sizeof(linha), f);
                
                while (fgets(linha, sizeof(linha), f)) {
                    sscanf(linha, "%[^,],%[^\n]", at.usuario, at.codigoTurma);
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
                FILE *f_turmas = fopen("dados/professores/turmas.csv", "r");
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

                FILE *f = fopen("dados/professores/diario.csv", "a");
                if (!f) {
                    printf("Erro ao abrir o arquivo do diário!\n");
                    voltarMenu();
                    break;
                }

                // Verificar se é o primeiro registro para adicionar cabeçalho
                fseek(f, 0, SEEK_END);
                if (ftell(f) == 0) {
                    fprintf(f, "CodigoTurma,Data,Tema,Conteudo\n");
                }

                fprintf(f, "%s,%s,%s,%s\n", codigoTurma, data, tema, conteudo);
                fclose(f);

                printf("\nAula registrada com sucesso!\n");
                voltarMenu();
                break;
            }

            case 2: {
                FILE *f = fopen("dados/professores/diario.csv", "r");
                if (!f) {
                    printf("Nenhum registro encontrado.\n");
                    voltarMenu();
                    break;
                }

                printf("\n=== Aulas Registradas ===\n");
                char linha[300];
                int count = 0;
                
                // Pular cabeçalho
                fgets(linha, sizeof(linha), f);
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
    FILE *f = fopen("dados/professores/diario.csv", "r");
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
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f);
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

    f = fopen("dados/professores/diario.csv", "r");
    FILE *temp = fopen("dados/professores/temp.csv", "w");
    if (!f || !temp) {
        printf("Erro ao abrir arquivos!\n");
        if (f) fclose(f);
        if (temp) fclose(temp);
        voltarMenu();
        return;
    }

    // Copiar cabeçalho
    fgets(linha, sizeof(linha), f);
    fputs(linha, temp);

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

    remove("dados/professores/diario.csv");
    rename("dados/professores/temp.csv", "dados/professores/diario.csv");

    if (removido)
        printf("Registro da turma '%s' na data '%s' excluído com sucesso!\n", codigoTurma, data);
    else
        printf("Nenhum registro encontrado com essas informações.\n");

    voltarMenu();
}

// ============================================================================ 
// TELA DO ALUNO (PÓS-LOGIN) 
// ============================================================================ 
void menuAluno(const char *usuarioAluno) {
    int opcao;
    char buffer[10];
    
    do {
        printf("\n====================================\n");
        printf(" Portal do Aluno - EduGate \n");
        printf(" Usuário: %s\n", usuarioAluno);
        printf("====================================\n");
        printf("1. Ver minhas turmas\n");
        printf("2. Matricular em nova turma\n");
        printf("3. Sair de uma turma\n");
        printf("4. Ver atividades\n");
        printf("5. Enviar atividade\n");  // NOVA OPÇÃO
        printf("6. Ver aulas agendadas\n");     
        printf("7. Ver notas\n");
        printf("--\n");
        printf("8. Sair\n");
        printf("Escolha uma opção: ");
        fgets(buffer, sizeof(buffer), stdin);
        opcao = atoi(buffer);
        limparTela();
        
        switch (opcao) {
            case 1:
                printf("\n=== Minhas Turmas ===\n");
                listarTurmasDoAluno(usuarioAluno);
                voltarMenu();
                break;
            case 2:
                matricularEmTurma(usuarioAluno);
                voltarMenu();
                break;
            case 3:
                sairDeTurma(usuarioAluno);
                voltarMenu();
                break;
            case 4:
                verAtividadesAluno(usuarioAluno);
                voltarMenu();
                break;
            case 5:
                enviarAtividadeAluno(usuarioAluno);  // NOVA FUNÇÃO
                voltarMenu();
                break;
            case 6:
                verAulasAgendadas(usuarioAluno); 
                voltarMenu();
                break;
            case 7:
                verNotasAluno(usuarioAluno);
                voltarMenu();
                break;
            case 8:
                printf("Saindo do Portal do Aluno...\n");
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 8);
}
// ============================================================================
// FUNÇÃO PARA PROFESSOR VER ENTREGAS DOS ALUNOS
// ============================================================================
void verEntregasAlunos() {
    printf("\n=== Entregas dos Alunos ===\n");
    
    // Selecionar turma por número
    char codigoTurma[50];
    if (!selecionarTurmaPorNumero(codigoTurma)) {
        return;
    }
    
    FILE *f_entregas = fopen("dados/entregas/entregas.csv", "r");
    if (!f_entregas) {
        printf("Nenhuma entrega registrada para esta turma.\n");
        return;
    }
    
    printf("\n=== Entregas da Turma %s ===\n", codigoTurma);
    char linha[300];
    int count = 0;
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f_entregas);
    
    while (fgets(linha, sizeof(linha), f_entregas)) {
        char usuario[50], turma[50], atividade[100], arquivo[100], data[20];
        sscanf(linha, "%[^,],%[^,],%[^,],%[^,],%[^\n]", usuario, turma, atividade, arquivo, data);
        
        if (strcmp(turma, codigoTurma) == 0) {
            printf("\n--- Entrega %d ---\n", ++count);
            printf("Aluno: %s\n", usuario);
            printf("Atividade: %s\n", atividade);
            printf("Arquivo: %s\n", arquivo);
            printf("Data de entrega: %s\n", data);
            
            // Verificar se já foi avaliada
            FILE *f_notas = fopen("dados/alunos/notas.csv", "r");
            if (f_notas) {
                char linha_nota[300];
                char user[50], turma_nota[50], atividade_nota[100], nota[10], data_avaliacao[20];
                
                // Pular cabeçalho
                fgets(linha_nota, sizeof(linha_nota), f_notas);
                
                int avaliada = 0;
                while (fgets(linha_nota, sizeof(linha_nota), f_notas)) {
                    sscanf(linha_nota, "%[^,],%[^,],%[^,],%[^,],%[^\n]", user, turma_nota, atividade_nota, nota, data_avaliacao);
                    if (strcmp(user, usuario) == 0 && 
                        strcmp(turma_nota, turma) == 0 && 
                        strcmp(atividade_nota, atividade) == 0) {
                        printf("AVALIADA - Nota: %s (Data: %s)\n", nota, data_avaliacao);
                        avaliada = 1;
                        break;
                    }
                }
                fclose(f_notas);
                
                if (!avaliada) {
                    printf("PENDENTE DE AVALIAÇÃO\n");
                }
            }
        }
    }
    
    fclose(f_entregas);
    
    if (count == 0) {
        printf("Nenhuma entrega encontrada para esta turma.\n");
    } else {
        printf("\nTotal de entregas: %d\n", count);
    }
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
        printf("2. Registrar aula (diário eletrônico)\n");  
        printf("3. Enviar atividade\n");                    
        printf("4. Declarar notas\n");
        printf("5. Ver entregas dos alunos\n");
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
                registrarAula();
                break;
            case 3:
                enviarAtividade();
                break;
            case 4:
                declararNotas();
                voltarMenu();
                break;
            case 5:
                verEntregasAlunos();  
                voltarMenu();
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
 
    char caminhoCompleto[200];
    sprintf(caminhoCompleto, "dados/cadastros/%s", arquivo);

    FILE *f = fopen(caminhoCompleto, "a");
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
    
    gerarMatricula(u.matricula, caminhoCompleto);
    
    // Verificar se é o primeiro registro para adicionar cabeçalho
    fseek(f, 0, SEEK_END);
    if (ftell(f) == 0) {
        fprintf(f, "Usuario,Senha,Matricula\n");  // CABEÇALHO ATUALIZADO
    }
    
    fprintf(f, "%s,%s,%s\n", u.usuario, u.senha, u.matricula);  // ESCRITA ATUALIZADA
    fclose(f);
    
    printf("%s cadastrado(a) com sucesso!\n", tipo);
    printf("Sua matrícula gerada automaticamente: %s\n", u.matricula);  // MOSTRAR MATRÍCULA GERADA
    printf("Guarde esta matrícula para futuros acessos!\n");
}

// ============================================================================ 
// FUNÇÃO PARA LOGIN DE USUÁRIO (ALUNO OU PROFESSOR) 
// ============================================================================ 
int loginUsuario(const char *arquivo, const char *tipo) {
    Usuario u;
    char usuario[50], senha[50];
    char linha[200];
    int logado = 0;
    
    // MODIFICAÇÃO: Criar caminho completo para a pasta cadastros
    char caminhoCompleto[200];
    sprintf(caminhoCompleto, "dados/cadastros/%s", arquivo);
    
    // MODIFICAÇÃO: Usar o caminho completo
    FILE *f = fopen(caminhoCompleto, "r");
    
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
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), f);
    
    while (fgets(linha, sizeof(linha), f)) {
        sscanf(linha, "%[^,],%[^,],%[^\n]", u.usuario, u.senha, u.matricula);
        if (strcmp(usuario, u.usuario) == 0 && strcmp(senha, u.senha) == 0) {
            logado = 1;
            break;
        }
    }
    fclose(f);
    
    if (logado) {
        printf("Login realizado com sucesso! Bem-vindo(a), %s (%s)\n", usuario, tipo);
        printf("Matrícula: %s\n", u.matricula);
        if (strcmp(tipo, "Professor") == 0) {
            menuProfessor();
        } else if (strcmp(tipo, "Aluno") == 0) {
            menuAluno(usuario);
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
        printf("====== Portal EduGate =======\n");
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
                loginUsuario("alunos.csv", "Aluno");
                break;
            case 2:
                cadastrarUsuario("alunos.csv", "Aluno");
                break;
            case 3:
                loginUsuario("professores.csv", "Professor");
                break;
            case 4:
                cadastrarUsuario("professores.csv", "Professor");
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
    criarEstruturaPastas();
    menuTipoUsuario();
    return 0;
}