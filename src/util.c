#include "util.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* =========================================================
   Funções de configuração e sistema
   ========================================================= */

void configurarCodificacao() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    setlocale(LC_ALL, "pt_BR.UTF-8");
}

void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void voltarMenu() {
    printf("\nPressione Enter para voltar...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Limpa buffer
    limparTela();
}

/* Cria toda a estrutura de diretórios de dados */
void criarEstruturaPastas() {
#ifdef _WIN32
    system("mkdir dados 2>nul");
    system("mkdir dados\\cadastros 2>nul");
    system("mkdir dados\\alunos 2>nul");
    system("mkdir dados\\professores 2>nul");
    system("mkdir dados\\entregas 2>nul");
#else
    system("mkdir -p dados/cadastros 2>/dev/null");
    system("mkdir -p dados/alunos 2>/dev/null");
    system("mkdir -p dados/professores 2>/dev/null");
    system("mkdir -p dados/entregas 2>/dev/null");
#endif
}

/* =========================================================
   Funções auxiliares de arquivo / CSV
   ========================================================= */

/* Verifica se um arquivo existe */
int arquivoExiste(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    fclose(f);
    return 1;
}

/* Adiciona uma linha ao CSV (cria se não existir) */
int adicionaLinhaCSV(const char *path, const char *linha) {
    FILE *f = fopen(path, "a");
    if (!f) return 0;
    fprintf(f, "%s\n", linha);
    fclose(f);
    return 1;
}

/* Função auxiliar segura para duplicar strings */
static char *dupString(const char *src) {
    if (!src) return NULL;
    char *dup = malloc(strlen(src) + 1);
    if (dup) {
        strcpy(dup, src);
    }
    return dup;
}

/* Verifica se um usuário já existe no CSV (coluna 1 = usuario) */
int usuario_existe_csv(const char *path, const char *usuario) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;

    char buf[512];
    if (!fgets(buf, sizeof(buf), f)) { fclose(f); return 0; } // pula cabeçalho

    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0;
        char *line = dupString(buf);
        char *token = strtok(line, ";");
        if (token && strcmp(token, usuario) == 0) {
            free(line);
            fclose(f);
            return 1;
        }
        free(line);
    }

    fclose(f);
    return 0;
}

/* Verifica se uma turma existe no CSV de turmas */
int turma_existe_csv(const char *codigo) {
    FILE *f = fopen("dados/professores/turmas.csv", "r");
    if (!f) return 0;

    char buf[512];
    if (!fgets(buf, sizeof(buf), f)) { fclose(f); return 0; } // pula cabeçalho

    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0;
        char *line = dupString(buf);
        char *token = strtok(line, ";");
        if (token && strcmp(token, codigo) == 0) {
            free(line);
            fclose(f);
            return 1;
        }
        free(line);
    }

    fclose(f);
    return 0;
}

/* Verifica se uma atividade existe para uma turma */
int atividade_existe_csv(const char *codigo_turma, const char *titulo_atividade) {
    FILE *f = fopen("dados/professores/atividades.csv", "r");
    if (!f) return 0;

    char buf[512];
    if (!fgets(buf, sizeof(buf), f)) { fclose(f); return 0; } // pula cabeçalho

    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0;
        char *line = dupString(buf);
        char *codigo = strtok(line, ";");
        char *titulo = strtok(NULL, ";");
        
        if (codigo && titulo && strcmp(codigo, codigo_turma) == 0 && 
            strcmp(titulo, titulo_atividade) == 0) {
            free(line);
            fclose(f);
            return 1;
        }
        free(line);
    }

    fclose(f);
    return 0;
}

/* Verifica se aluno já está matriculado na turma */
int aluno_ja_matriculado(const char *usuario, const char *codigo_turma) {
    FILE *f = fopen("dados/alunos/alunos_turmas.csv", "r");
    if (!f) return 0;

    char buf[512];
    if (!fgets(buf, sizeof(buf), f)) { fclose(f); return 0; } // pula cabeçalho

    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0;
        char *line = dupString(buf);
        char *u = strtok(line, ";");
        char *codigo = strtok(NULL, ";");
        
        if (u && codigo && strcmp(u, usuario) == 0 && strcmp(codigo, codigo_turma) == 0) {
            free(line);
            fclose(f);
            return 1;
        }
        free(line);
    }

    fclose(f);
    return 0;
}

/* Verifica se existem turmas disponíveis */
int existemTurmasDisponiveis() {
    FILE *f = fopen("dados/professores/turmas.csv", "r");
    if (!f) return 0;
    
    char buf[512];
    fgets(buf, sizeof(buf), f); // cabeçalho
    int tem_turmas = (fgets(buf, sizeof(buf), f) != NULL);
    fclose(f);
    return tem_turmas;
}

/* Verifica se aluno tem turmas matriculadas */
int alunoTemTurmasMatriculadas(const char *usuario) {
    FILE *f = fopen("dados/alunos/alunos_turmas.csv","r");
    if (!f) return 0;

    char buf[512];
    char *line = NULL;
    char *u = NULL;
    char *codigoTurma = NULL;
    int count = 0;

    fgets(buf,sizeof(buf),f); // cabeçalho
    while (fgets(buf,sizeof(buf),f)) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;
        
        u = strtok(line, ";");
        codigoTurma = strtok(NULL, ";");

        if (u && codigoTurma && strcmp(u, usuario)==0) {
            count++;
        }
        free(line);
        line = NULL;
    }
    fclose(f);
    return count > 0;
}

/* Verifica se existem atividades para envio */
int existemAtividadesParaEnvio(const char *usuario) {
    FILE *fa = fopen("dados/professores/atividades.csv", "r");
    FILE *fm = fopen("dados/alunos/alunos_turmas.csv", "r");
    FILE *fe = fopen("dados/entregas/entregas.csv", "r");
    
    if (!fa || !fm) return 0;

    char buf[512];
    char *line = NULL;
    char *u = NULL;
    char *codigo = NULL;
    char turmas[20][100];
    int nt = 0;
    int atividades_encontradas = 0;

    /* Coletar turmas do aluno */
    fgets(buf, sizeof(buf), fm); // cabeçalho
    while (fgets(buf, sizeof(buf), fm) && nt < 20) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;
        
        u = strtok(line, ";");
        codigo = strtok(NULL, ";");
        
        if (u && codigo && strcmp(u, usuario) == 0) {
            strncpy(turmas[nt], codigo, sizeof(turmas[nt]) - 1);
            turmas[nt][sizeof(turmas[nt]) - 1] = '\0';
            nt++;
        }
        free(line);
        line = NULL;
    }
    fclose(fm);

    if (nt == 0) {
        fclose(fa);
        if (fe) fclose(fe);
        return 0;
    }

    /* Verificar entregas já feitas */
    char entregas_feitas[50][200];
    int ne = 0;
    if (fe) {
        fgets(buf, sizeof(buf), fe); // cabeçalho
        while (fgets(buf, sizeof(buf), fe) && ne < 50) {
            buf[strcspn(buf, "\n")] = 0;
            line = dupString(buf);
            if (!line) continue;
            
            char *user = strtok(line, ";");
            char *turma = strtok(NULL, ";");
            char *atividade = strtok(NULL, ";");
            
            if (user && turma && atividade && strcmp(user, usuario) == 0) {
                snprintf(entregas_feitas[ne], sizeof(entregas_feitas[ne]), "%s;%s", turma, atividade);
                ne++;
            }
            free(line);
            line = NULL;
        }
        fclose(fe);
    }

    /* Verificar se existem atividades disponíveis */
    fgets(buf, sizeof(buf), fa); // cabeçalho
    while (fgets(buf, sizeof(buf), fa)) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;

        codigo = strtok(line, ";");
        char *titulo = strtok(NULL, ";");

        if (codigo && titulo) {
            for (int i = 0; i < nt; ++i) {
                if (strcmp(codigo, turmas[i]) == 0) {
                    /* Verificar se já enviou esta atividade */
                    int ja_enviou = 0;
                    char chave[200];
                    snprintf(chave, sizeof(chave), "%s;%s", codigo, titulo);
                    
                    for (int j = 0; j < ne; j++) {
                        if (strcmp(chave, entregas_feitas[j]) == 0) {
                            ja_enviou = 1;
                            break;
                        }
                    }
                    
                    if (!ja_enviou) {
                        atividades_encontradas = 1;
                        break;
                    }
                }
            }
        }
        free(line);
        line = NULL;
        if (atividades_encontradas) break;
    }

    fclose(fa);
    return atividades_encontradas;
}

/* Valida login: CSV com colunas usuario;senha;... */
int validar_login(const char *path, const char *usuario, const char *senha) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;

    char buf[512];
    if (!fgets(buf, sizeof(buf), f)) { fclose(f); return 0; } // pula cabeçalho

    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0;
        char *line = dupString(buf);
        char *u = strtok(line, ";");
        char *s = strtok(NULL, ";");

        if (u && s && strcmp(u, usuario) == 0 && strcmp(s, senha) == 0) {
            free(line);
            fclose(f);
            return 1;
        }

        free(line);
    }

    fclose(f);
    return 0;
}

/* Obtém data atual no formato DD/MM/AAAA */
void obter_data_atual(char *buffer, size_t tamanho) {
#ifdef _WIN32
    SYSTEMTIME st;
    GetLocalTime(&st);
    snprintf(buffer, tamanho, "%02d/%02d/%04d", st.wDay, st.wMonth, st.wYear);
#else
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, tamanho, "%d/%m/%Y", tm_info);
#endif
}

/* Limpa o buffer de entrada */
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Exibe mensagem de erro */
void exibir_erro(const char *mensagem) {
    printf("\n[ERRO] %s\n", mensagem);
}

/* Exibe mensagem de sucesso */
void exibir_sucesso(const char *mensagem) {
    printf("\n[SUCESSO] %s\n", mensagem);
}

/* Exibe mensagem de informação */
void exibir_info(const char *mensagem) {
    printf("\n[INFO] %s\n", mensagem);
}

/* Valida formato de data DD/MM/AAAA */
int validar_data(const char *data) {
    int dia, mes, ano;
    if (sscanf(data, "%d/%d/%d", &dia, &mes, &ano) != 3) {
        return 0;
    }
    
    if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || ano < 1900 || ano > 2100) {
        return 0;
    }
    
    return 1;
}

/* Função para obter o prazo de uma atividade */
char* obterPrazoAtividade(const char *turma, const char *atividade) {
    static char prazo[20];
    prazo[0] = '\0';
    
    FILE *f = fopen("dados/professores/atividades.csv", "r");
    if (!f) return NULL;

    char buf[512];
    fgets(buf, sizeof(buf), f); // cabeçalho
    
    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0;
        char *line = dupString(buf);
        char *codigo = strtok(line, ";");
        char *titulo = strtok(NULL, ";");
        char *descricao = strtok(NULL, ";");
        char *prazo_atividade = strtok(NULL, ";");
        char *tipo = strtok(NULL, ";");
        char *valor = strtok(NULL, ";");
        
        if (codigo && titulo && 
            strcmp(codigo, turma) == 0 && 
            strcmp(titulo, atividade) == 0) {
            strncpy(prazo, prazo_atividade ? prazo_atividade : "", sizeof(prazo) - 1);
            free(line);
            fclose(f);
            return prazo;
        }
        free(line);
    }
    
    fclose(f);
    return NULL;
}

/* Função para comparar datas no formato DD/MM/AAAA */
int compararDatas(const char *data1, const char *data2) {
    int dia1, mes1, ano1;
    int dia2, mes2, ano2;
    
    if (sscanf(data1, "%d/%d/%d", &dia1, &mes1, &ano1) != 3) return -1;
    if (sscanf(data2, "%d/%d/%d", &dia2, &mes2, &ano2) != 3) return -1;
    
    if (ano1 != ano2) return ano1 - ano2;
    if (mes1 != mes2) return mes1 - mes2;
    return dia1 - dia2;
}