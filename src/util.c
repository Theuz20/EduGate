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
#include <math.h>

// Para funções de hash - versão simplificada sem OpenSSL
#include <ctype.h>

/* =========================================================
   Função auxiliar segura para duplicar strings
   ========================================================= */
static char* dupString(const char* src) {
    if (!src) return NULL;
    char *dup = malloc(strlen(src) + 1);
    if (dup) {
        strcpy(dup, src);
    }
    return dup;
}

/* =========================================================
   Funções de Hash para Senhas (versão simplificada)
   ========================================================= */

// Função simples de hash para demonstração
// Em produção, use uma biblioteca como OpenSSL
static unsigned int simple_hash(const char *str) {
    unsigned int hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return hash;
}

char* hash_senha(const char *senha) {
    /* Gera hash simples da senha (para demonstração) */
    if (!senha) return NULL;
    
    // Em produção, substitua por:
    // #include <openssl/sha.h>
    // e use SHA256
    
    unsigned int hash_val = simple_hash(senha);
    char *hash_str = malloc(65); // Para hexadecimal
    if (hash_str) {
        snprintf(hash_str, 65, "%08x%08x%08x%08x%08x%08x%08x%08x",
                hash_val, hash_val ^ 0x12345678, 
                hash_val * 3, hash_val + 0xABCDEF,
                hash_val ^ 0xFEDCBA, hash_val * 7,
                hash_val - 0x13579B, hash_val ^ 0x2468ACE);
    }
    return hash_str;
}

int verificar_senha(const char *senha, const char *senha_hash) {
    /* Verifica se a senha corresponde ao hash */
    if (!senha || !senha_hash) return 0;
    
    char *novo_hash = hash_senha(senha);
    int resultado = (strcmp(novo_hash, senha_hash) == 0);
    free(novo_hash);
    return resultado;
}

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

void pausarTela() {
    printf("\nPressione Enter para continuar...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Limpa buffer
}

void voltarMenu() {
    printf("\nPressione Enter para voltar...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Limpa buffer
    limparTela();
}

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

int arquivoExiste(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    fclose(f);
    return 1;
}

int adicionaLinhaCSV(const char *path, const char *linha) {
    FILE *f = fopen(path, "a");
    if (!f) return 0;
    fprintf(f, "%s\n", linha);
    fclose(f);
    return 1;
}

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

int validar_login(const char *path, const char *usuario, const char *senha) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;

    char buf[512];
    if (!fgets(buf, sizeof(buf), f)) { fclose(f); return 0; } // pula cabeçalho

    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0;
        char *line = dupString(buf);
        char *u = strtok(line, ";");
        char *s_hash = strtok(NULL, ";"); // Agora é o hash da senha

        if (u && s_hash && strcmp(u, usuario) == 0 && verificar_senha(senha, s_hash)) {
            free(line);
            fclose(f);
            return 1;
        }

        free(line);
    }

    fclose(f);
    return 0;
}

/* =========================================================
   Novas funções utilitárias
   ========================================================= */

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

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void exibir_erro(const char *mensagem) {
    printf("\n[ERRO] %s\n", mensagem);
}

void exibir_sucesso(const char *mensagem) {
    printf("\n[SUCESSO] %s\n", mensagem);
}

void exibir_info(const char *mensagem) {
    printf("\n[INFO] %s\n", mensagem);
}

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

/* =========================================================
   Funções de validação para aluno.c
   ========================================================= */

int existemTurmasDisponiveis() {
    FILE *f = fopen("dados/professores/turmas.csv", "r");
    if (!f) return 0;
    
    char buf[512];
    fgets(buf, sizeof(buf), f); // cabeçalho
    int tem_turmas = (fgets(buf, sizeof(buf), f) != NULL);
    fclose(f);
    return tem_turmas;
}

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

void verificar_estrutura_diario() {
    FILE *f = fopen("dados/professores/diario.csv", "r");
    if (!f) {
        printf("Arquivo diario.csv nao existe.\n");
        return;
    }
    
    char buf[512];
    printf("=== ESTRUTURA DO DIARIO.CSV ===\n");
    
    int linha = 0;
    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0;
        printf("Linha %d: %s\n", linha, buf);
        
        // Mostrar campos separados
        char *campos[5];
        char *line = dupString(buf);
        campos[0] = strtok(line, ";");
        campos[1] = strtok(NULL, ";");
        campos[2] = strtok(NULL, ";");
        campos[3] = strtok(NULL, ";");
        campos[4] = strtok(NULL, ";");
        
        printf("  Campos: ");
        for (int i = 0; i < 5; i++) {
            printf("[%s] ", campos[i] ? campos[i] : "NULL");
        }
        printf("\n");
        
        free(line);
        linha++;
    }
    
    fclose(f);
}

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

/* =========================================================
   Funções para verificação de prazo
   ========================================================= */

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

int compararDatas(const char *data1, const char *data2) {
    int dia1, mes1, ano1;
    int dia2, mes2, ano2;
    
    if (sscanf(data1, "%d/%d/%d", &dia1, &mes1, &ano1) != 3) return -1;
    if (sscanf(data2, "%d/%d/%d", &dia2, &mes2, &ano2) != 3) return -1;
    
    if (ano1 != ano2) return ano1 - ano2;
    if (mes1 != mes2) return mes1 - mes2;
    return dia1 - dia2;
}

void debug_diario_csv() {
    printf("\n=== DEBUG DIARIO.CSV ===\n");
    FILE *f = fopen("dados/professores/diario.csv", "r");
    if (!f) {
        printf("Arquivo diario.csv nao existe!\n");
        return;
    }
    
    char buf[512];
    int linha = 0;
    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0;
        printf("Linha %d: [%s]\n", linha, buf);
        
        // Mostrar cada campo separadamente
        char *campos[10];
        char *copy = dupString(buf);
        int campo_count = 0;
        
        campos[campo_count] = strtok(copy, ";");
        while (campos[campo_count] != NULL && campo_count < 9) {
            campo_count++;
            campos[campo_count] = strtok(NULL, ";");
        }
        
        printf("  Campos encontrados: %d\n", campo_count);
        for (int i = 0; i < campo_count; i++) {
            printf("  Campo %d: [%s]\n", i, campos[i]);
        }
        printf("  ---\n");
        free(copy);
        linha++;
    }
    fclose(f);
}

float calcularMediaAluno(const char *usuario) {
    float soma = 0.0;
    int count = 0;
    
    FILE *f = fopen("dados/alunos/notas.csv", "r");
    if (!f) return 0.0;
    
    char buf[512];
    fgets(buf, sizeof(buf), f); // cabeçalho
    
    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0;
        char *line = dupString(buf);
        char *user = strtok(line, ";");
        char *turma = strtok(NULL, ";");
        char *atividade = strtok(NULL, ";");
        char *nota_str = strtok(NULL, ";");
        
        if (user && strcmp(user, usuario) == 0 && nota_str) {
            float nota = atof(nota_str);
            soma += nota;
            count++;
        }
        free(line);
    }
    fclose(f);
    
    return (count > 0) ? (soma / count) : 0.0;
}

// Calcular média de uma turma específica
float calcularMediaTurma(const char *codigoTurma) {
    float soma = 0.0;
    int count = 0;
    
    FILE *f = fopen("dados/alunos/notas.csv", "r");
    if (!f) return 0.0;
    
    char buf[512];
    fgets(buf, sizeof(buf), f); // cabeçalho
    
    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0;
        char *line = dupString(buf);
        char *usuario = strtok(line, ";");
        char *turma = strtok(NULL, ";");
        char *atividade = strtok(NULL, ";");
        char *nota_str = strtok(NULL, ";");
        
        if (turma && strcmp(turma, codigoTurma) == 0 && nota_str) {
            float nota = atof(nota_str);
            soma += nota;
            count++;
        }
        free(line);
    }
    fclose(f);
    
    return (count > 0) ? (soma / count) : 0.0;
}

// Contar faltas consecutivas de um aluno
int contarFaltasConsecutivas(const char *usuario, const char *turma) {
    int faltas_consecutivas = 0;
    int max_faltas = 0;
    
    FILE *f = fopen("dados/professores/diario.csv", "r");
    if (!f) return 0;
    
    char buf[512];
    fgets(buf, sizeof(buf), f); // cabeçalho
    
    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0;
        char *line = dupString(buf);
        char *codigo = strtok(line, ";");
        char *data = strtok(NULL, ";");
        char *tema = strtok(NULL, ";");
        char *conteudo = strtok(NULL, ";");
        char *presenca = strtok(NULL, "\n");
        
        if (codigo && strcmp(codigo, turma) == 0 && presenca) {
            // Verificar se aluno está na lista de presença
            int presente = 0;
            char *lista = dupString(presenca);
            char *aluno = strtok(lista, ",");
            
            while (aluno) {
                if (strcmp(aluno, usuario) == 0) {
                    presente = 1;
                    break;
                }
                aluno = strtok(NULL, ",");
            }
            free(lista);
            
            if (!presente) {
                faltas_consecutivas++;
                if (faltas_consecutivas > max_faltas) {
                    max_faltas = faltas_consecutivas;
                }
            } else {
                faltas_consecutivas = 0;
            }
        }
        free(line);
    }
    fclose(f);
    
    return max_faltas;
}

// Detectar queda de desempenho
int detectarQuedaDesempenho(const char *usuario, const char *turma) {
    float notas[10];
    int count = 0;
    
    // Coletar últimas notas do aluno na turma
    FILE *f = fopen("dados/alunos/notas.csv", "r");
    if (!f) return 0;
    
    char buf[512];
    fgets(buf, sizeof(buf), f); // cabeçalho
    
    while (fgets(buf, sizeof(buf), f) && count < 10) {
        buf[strcspn(buf, "\n")] = 0;
        char *line = dupString(buf);
        char *user = strtok(line, ";");
        char *turma_nota = strtok(NULL, ";");
        char *atividade = strtok(NULL, ";");
        char *nota_str = strtok(NULL, ";");
        
        if (user && turma_nota && strcmp(user, usuario) == 0 && 
            strcmp(turma_nota, turma) == 0 && nota_str) {
            notas[count++] = atof(nota_str);
        }
        free(line);
    }
    fclose(f);
    
    // Se tem pelo menos 3 notas, verificar tendência
    if (count >= 3) {
        float media_inicial = (notas[0] + notas[1]) / 2;
        float media_recente = (notas[count-1] + notas[count-2]) / 2;
        
        // Queda significativa (mais de 30%)
        return (media_recente < media_inicial * 0.7);
    }
    
    return 0;
}

// Prever desempenho futuro baseado no histórico
void preverDesempenhoAluno(const char *usuario, const char *turma) {
    float notas[20];
    int count = 0;
    
    // Coletar histórico de notas do aluno
    FILE *f = fopen("dados/alunos/notas.csv", "r");
    if (!f) {
        printf("Historico insuficiente para previsao.\n");
        return;
    }
    
    char buf[512];
    fgets(buf, sizeof(buf), f); // cabecalho
    
    while (fgets(buf, sizeof(buf), f) && count < 20) {
        buf[strcspn(buf, "\n")] = 0;
        char *line = dupString(buf);
        char *user = strtok(line, ";");
        char *turma_nota = strtok(NULL, ";");
        char *atividade = strtok(NULL, ";");
        char *nota_str = strtok(NULL, ";");
        
        if (user && strcmp(user, usuario) == 0 && nota_str) {
            // Se for de turma específica ou de qualquer turma
            if (!turma || (turma_nota && strcmp(turma_nota, turma) == 0)) {
                notas[count] = atof(nota_str);
                count++;
            }
        }
        free(line);
    }
    fclose(f);
    
    if (count < 3) {
        printf("Dados insuficientes para previsao.\n");
        printf("Necessario pelo menos 3 notas no historico.\n");
        return;
    }
    
    // Cálculo da previsão usando regressão linear simples
    float soma_x = 0, soma_y = 0, soma_xy = 0, soma_x2 = 0;
    
    for (int i = 0; i < count; i++) {
        float x = i + 1;  // Tempo (atividades sequenciais)
        float y = notas[i]; // Nota
        
        soma_x += x;
        soma_y += y;
        soma_xy += x * y;
        soma_x2 += x * x;
    }
    
    float n = count;
    float media_x = soma_x / n;
    float media_y = soma_y / n;
    
    // Coeficientes da regressão: y = a + bx
    float b = (soma_xy - n * media_x * media_y) / (soma_x2 - n * media_x * media_x);
    float a = media_y - b * media_x;
    
    // Prever próxima nota
    float proxima_nota = a + b * (n + 1);
    
    // Limitar entre 0 e 10
    if (proxima_nota < 0) proxima_nota = 0;
    if (proxima_nota > 10) proxima_nota = 10;
    
    // Calcular tendência
    float tendencia = b; // Inclinação da reta
    
    // Análise da variabilidade (valor absoluto médio)
    float variabilidade = 0;
    for (int i = 0; i < count; i++) {
        float diferenca = notas[i] - media_y;
        if (diferenca < 0) diferenca = -diferenca; // Valor absoluto manual
        variabilidade += diferenca;
    }
    variabilidade /= n;
    
    // Buscar nome do aluno - CORRIGIDO
    char nome_aluno[100] = "";
    FILE *fc = fopen("dados/cadastros/alunos.csv", "r");
    if (fc) {
        char buf2[512];
        fgets(buf2, sizeof(buf2), fc); // cabecalho
        while (fgets(buf2, sizeof(buf2), fc)) {
            buf2[strcspn(buf2, "\n")] = 0;
            char *line2 = dupString(buf2);
            
            // ✅ CORREÇÃO: Parsing correto dos campos
            char *user = strtok(line2, ";");
            char *senha_hash = strtok(NULL, ";");  // Pular senha hash
            char *matricula = strtok(NULL, ";");   // Pular matrícula  
            char *nome = strtok(NULL, ";");        // Nome é o 4º campo
            char *email = strtok(NULL, ";");       // Pular email
            
            if (user && strcmp(user, usuario) == 0 && nome) {
                strcpy(nome_aluno, nome);
                free(line2);
                break;
            }
            free(line2);
        }
        fclose(fc);
    }
    
    // Mostrar resultados da previsão
    printf("\n==============================\n");
    printf("=== PREVISÃO DE DESEMPENHO ===\n");
    printf("==============================\n");
    
    if (strlen(nome_aluno) > 0) {
        printf("Aluno: %s (%s)\n", nome_aluno, usuario);
    } else {
        printf("Aluno: %s\n", usuario);
    }
    
    if (turma) {
        printf("Turma: %s\n", turma);
    }
    
    printf("Baseado em %d atividades anteriores\n\n", count);
    
    // Mostrar histórico recente
    printf("HISTORICO RECENTE:\n");
    int start = (count > 5) ? count - 5 : 0;
    for (int i = start; i < count; i++) {
        printf("   Atividade %d: %.1f\n", i + 1, notas[i]);
    }
    
    printf("\nPREVISÃO PARA PROXIMA ATIVIDADE:\n");
    printf("   Nota esperada: %.1f/10\n", proxima_nota);
    
    // Classificar a previsão
    if (proxima_nota >= 9.0) {
        printf("   Excelente desempenho esperado!\n");
    } else if (proxima_nota >= 7.0) {
        printf("   Bom desempenho esperado\n");
    } else if (proxima_nota >= 5.0) {
        printf("   Desempenho regular esperado\n");
    } else {
        printf("   Atencao: Desempenho abaixo do esperado\n");
    }
    
    // Análise da tendência
    printf("\nANALISE DA TENDENCIA:\n");
    if (tendencia > 0.5) {
        printf("   Tendencia: FORTE CRESCIMENTO\n");
        printf("   O aluno esta evoluindo rapidamente!\n");
    } else if (tendencia > 0.1) {
        printf("   Tendencia: CRESCIMENTO MODERADO\n");
        printf("   Evolucao positiva consistente\n");
    } else if (tendencia > -0.1) {
        printf("   Tendencia: ESTAVEL\n");
        printf("   Desempenho consistente\n");
    } else if (tendencia > -0.5) {
        printf("   Tendencia: QUEDA MODERADA\n");
        printf("   Atencao a evolucao\n");
    } else {
        printf("   Tendencia: FORTE QUEDA\n");
        printf("   Necessidade de intervenção\n");
    }
    
    // Análise de consistência
    printf("\nANALISE DE CONSISTENCIA:\n");
    if (variabilidade < 1.5) {
        printf("   Muito consistente\n");
        printf("   Previsoes sao altamente confiaveis\n");
    } else if (variabilidade < 3.0) {
        printf("   Moderadamente consistente\n");
        printf("   Previsoes com boa confiabilidade\n");
    } else {
        printf("   Pouco consistente\n");
        printf("   Desempenho varia muito - previsao limitada\n");
    }
    
    // Fatores de risco e oportunidades
    printf("\nFATORES CRITICOS:\n");
    if (notas[count-1] < 6.0 && tendencia < 0) {
        printf("   RISCO: Ultimo desempenho fraco com tendencia negativa\n");
    }
    if (variabilidade > 4.0) {
        printf("   RISCO: Alta inconsistencia no desempenho\n");
    }
    if (notas[count-1] > 8.0 && tendencia > 0.3) {
        printf("   OPORTUNIDADE: Excelente desempenho com crescimento\n");
    }
    
    printf("\n========================================================\n");
    printf("A previsão é baseada em analise estatistica do historico\n");
    printf("========================================================\n");
}