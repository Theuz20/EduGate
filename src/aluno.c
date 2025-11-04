#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

/* função auxiliar segura para duplicar string */
static char* dupString(const char* src) {
    if (!src) return NULL;
    char *dup = malloc(strlen(src) + 1);
    if (dup) {
        strcpy(dup, src);
    }
    return dup;
}

/* =========================================================
   PROTÓTIPOS DAS FUNÇÕES STATIC
   ========================================================= */
static void listarMinhasTurmas(const char *usuario);
static void sairDaTurma(const char *usuario);
static void listarTurmasDisponiveis(void);  // MOVIDA PARA CIMA
static void matricularEmTurma(const char *usuario);
static void verAulasAgendadas(const char *usuario);
static void confirmarPresenca(const char *usuario, const char *codigoTurma, const char *dataAula);
static void verAtividades(const char *usuario);
static void listarAtividadesParaEnvio(const char *usuario);
static void enviarAtividade(const char *usuario);
static void verMinhasEntregas(const char *usuario);
static void cancelarEnvioAtividade(const char *usuario);
static void verNotas(const char *usuario);
static char* obterTipoAtividade(const char *turma, const char *atividade);

/* =========================================================
   FUNÇÃO: listarTurmasDisponiveis (AGORA NO INÍCIO)
   ========================================================= */

static void listarTurmasDisponiveis() {
    FILE *f = fopen("dados/professores/turmas.csv", "r");
    if (!f) {
        printf("Nenhuma turma disponivel.\n");
        return;
    }

    char buf[512];
    char *line = NULL;
    char *codigo = NULL;
    char *nome = NULL;
    char *professor = NULL;
    char *periodo = NULL;
    int count = 0;

    fgets(buf, sizeof(buf), f); // pula cabecalho

    printf("\n=== Turmas Disponiveis para Matricula ===\n");
    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;
        
        codigo = strtok(line, ";");
        nome = strtok(NULL, ";");
        professor = strtok(NULL, ";");
        periodo = strtok(NULL, ";");

        printf("%d. %s - %s\n", ++count, codigo ? codigo : "", nome ? nome : "");
        printf("   Professor: %s | Periodo: %s\n", professor ? professor : "?", periodo ? periodo : "?");
        printf("   --------------------\n");
        free(line);
        line = NULL;
    }

    if (count == 0)
        printf("Nenhuma turma cadastrada.\n");

    fclose(f);
}

/* =========================================================
   FUNÇÃO: listarMinhasTurmas
   ========================================================= */
static void listarMinhasTurmas(const char *usuario) {
    FILE *f = fopen("dados/alunos/alunos_turmas.csv","r");
    if (!f) { 
        printf("Nenhuma matricula encontrada.\n"); 
        return; 
    }

    char buf[512];
    char *line = NULL;
    char *u = NULL;
    char *codigoTurma = NULL;
    int count = 0;

    fgets(buf,sizeof(buf),f); // pula cabecalho

    printf("\n=== Minhas Turmas ===\n");
    while (fgets(buf,sizeof(buf),f)) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;
        
        u = strtok(line, ";");
        codigoTurma = strtok(NULL, ";");

        if (u && codigoTurma && strcmp(u, usuario)==0) {
            printf("%d. %s\n", ++count, codigoTurma);
        }

        free(line);
        line = NULL;
    }

    if (count == 0) {
        printf("Voce nao esta matriculado em nenhuma turma.\n");
    } else {
        printf("\n--- Opcoes ---\n");
        printf("Deseja sair de alguma turma? (s/n): ");
        char opcao[10];
        fgets(opcao, sizeof(opcao), stdin);
        opcao[strcspn(opcao, "\n")] = 0;
        
        if (opcao[0] == 's' || opcao[0] == 'S') {
            sairDaTurma(usuario);
        }
    }

    fclose(f);
}

/* =========================================================
   FUNÇÃO: matricularEmTurma
   ========================================================= */
static void matricularEmTurma(const char *usuario) {
    if (!existemTurmasDisponiveis()) {
        printf("Nao ha turmas disponiveis para matricula.\n");
        return;
    }

    char codigo[50];
    
    printf("\n=== Matricular em Turma ===\n");
    
    /* Primeiro mostra as turmas disponiveis */
    listarTurmasDisponiveis();
    
    printf("\nDigite o codigo da turma: ");
    fgets(codigo, sizeof(codigo), stdin);
    codigo[strcspn(codigo, "\n")] = 0;

    /* Verificar se a turma existe */
    if (!turma_existe_csv(codigo)) {
        printf("Turma nao encontrada!\n");
        return;
    }

    /* Verificar se ja esta matriculado */
    if (aluno_ja_matriculado(usuario, codigo)) {
        printf("Voce ja esta matriculado nesta turma!\n");
        return;
    }

    char data[20];
    obter_data_atual(data, sizeof(data));

    char linha[256];
    snprintf(linha, sizeof(linha), "%s;%s;%s", usuario, codigo, data);

    if (adicionaLinhaCSV("dados/alunos/alunos_turmas.csv", linha)) {
        printf("Matricula realizada com sucesso!\n");
    } else {
        printf("Erro ao matricular.\n");
    }
}

/* =========================================================
   FUNÇÃO: sairDaTurma
   ========================================================= */
static void sairDaTurma(const char *usuario) {
    if (!alunoTemTurmasMatriculadas(usuario)) {
        printf("Voce nao esta matriculado em nenhuma turma.\n");
        return;
    }

    char codigoTurma[50];
    
    printf("\n=== Sair de Turma ===\n");
    
    /* Primeiro mostra as turmas do aluno */
    listarMinhasTurmas(usuario);
    
    printf("\nDigite o codigo da turma que deseja sair: ");
    fgets(codigoTurma, sizeof(codigoTurma), stdin);
    codigoTurma[strcspn(codigoTurma, "\n")] = 0;

    /* Verificar se o aluno esta matriculado na turma */
    if (!aluno_ja_matriculado(usuario, codigoTurma)) {
        printf("Voce nao esta matriculado nesta turma!\n");
        return;
    }

    /* Confirmar saida */
    printf("Tem certeza que deseja sair da turma %s? (s/n): ", codigoTurma);
    char confirmacao[10];
    fgets(confirmacao, sizeof(confirmacao), stdin);
    confirmacao[strcspn(confirmacao, "\n")] = 0;

    if (confirmacao[0] != 's' && confirmacao[0] != 'S') {
        printf("Operacao cancelada.\n");
        return;
    }

    /* Remover a matricula do arquivo */
    FILE *f = fopen("dados/alunos/alunos_turmas.csv", "r");
    FILE *f_temp = fopen("dados/alunos/temp.csv", "w");
    
    if (!f || !f_temp) {
        printf("Erro ao abrir arquivos.\n");
        if (f) fclose(f);
        if (f_temp) fclose(f_temp);
        return;
    }

    char buf[512];
    int removido = 0;

    /* Copiar cabecalho */
    fgets(buf, sizeof(buf), f);
    fprintf(f_temp, "%s", buf);

    /* Processar linhas */
    while (fgets(buf, sizeof(buf), f)) {
        char *line = dupString(buf);
        char *user = strtok(line, ";");
        char *turma = strtok(NULL, ";");
        char *data = strtok(NULL, ";");
        
        if (user && turma && 
            (strcmp(user, usuario) != 0 || strcmp(turma, codigoTurma) != 0)) {
            /* Mantem a linha se nao for a matricula a ser removida */
            fprintf(f_temp, "%s", buf);
        } else {
            removido = 1;
        }
        free(line);
    }

    fclose(f);
    fclose(f_temp);

    if (removido) {
        remove("dados/alunos/alunos_turmas.csv");
        rename("dados/alunos/temp.csv", "dados/alunos/alunos_turmas.csv");
        printf("Voce saiu da turma %s com sucesso!\n", codigoTurma);
    } else {
        remove("dados/alunos/temp.csv");
        printf("Erro ao remover matricula.\n");
    }
}

/* =========================================================
   FUNÇÃO: confirmarPresenca
   ========================================================= */
static void confirmarPresenca(const char *usuario, const char *codigoTurma, const char *dataAula) {
    FILE *f = fopen("dados/professores/diario.csv", "r");
    FILE *f_temp = fopen("dados/professores/temp.csv", "w");
    
    if (!f || !f_temp) {
        printf("Erro ao abrir arquivos.\n");
        if (f) fclose(f);
        if (f_temp) fclose(f_temp);
        return;
    }

    char buf[512];
    int encontrou = 0;

    /* Copiar cabecalho */
    fgets(buf, sizeof(buf), f);
    fprintf(f_temp, "%s", buf);

    /* Processar linhas */
    while (fgets(buf, sizeof(buf), f)) {
        char *line = dupString(buf);
        char *codigo = strtok(line, ";");
        char *data = strtok(NULL, ";");
        char *tema = strtok(NULL, ";");
        char *conteudo = strtok(NULL, ";");
        char *presenca = strtok(NULL, ";");
        
        if (codigo && data && 
            strcmp(codigo, codigoTurma) == 0 && 
            strcmp(data, dataAula) == 0) {
            
            /* Aula encontrada - atualizar presenca */
            encontrou = 1;
            char nova_presenca[512];
            
            if (presenca && strlen(presenca) > 0) {
                /* Verificar se ja esta na lista */
                char *lista_temp = dupString(presenca);
                char *aluno_p = strtok(lista_temp, ",");
                int ja_presente = 0;
                
                while (aluno_p) {
                    if (strcmp(aluno_p, usuario) == 0) {
                        ja_presente = 1;
                        break;
                    }
                    aluno_p = strtok(NULL, ",");
                }
                free(lista_temp);
                
                if (!ja_presente) {
                    /* Adicionar a lista existente */
                    snprintf(nova_presenca, sizeof(nova_presenca), "%s,%s", presenca, usuario);
                } else {
                    /* Ja esta presente */
                    strcpy(nova_presenca, presenca);
                    printf("Voce ja confirmou presenca nesta aula.\n");
                }
            } else {
                /* Primeira confirmacao de presenca */
                snprintf(nova_presenca, sizeof(nova_presenca), "%s", usuario);
            }
            
            fprintf(f_temp, "%s;%s;%s;%s;%s\n", codigo, data, tema, conteudo, nova_presenca);
            printf("Presenca confirmada na aula de %s!\n", data);
            
        } else {
            /* Manter linha original */
            fprintf(f_temp, "%s", buf);
        }
        free(line);
    }

    fclose(f);
    fclose(f_temp);

    if (encontrou) {
        remove("dados/professores/diario.csv");
        rename("dados/professores/temp.csv", "dados/professores/diario.csv");
    } else {
        remove("dados/professores/temp.csv");
        printf("Aula nao encontrada.\n");
    }
}

/* =========================================================
   FUNÇÃO: verAulasAgendadas
   ========================================================= */
static void verAulasAgendadas(const char *usuario) {
    if (!alunoTemTurmasMatriculadas(usuario)) {
        printf("Voce nao esta matriculado em nenhuma turma.\n");
        return;
    }

    FILE *fd = fopen("dados/professores/diario.csv", "r");
    FILE *fm = fopen("dados/alunos/alunos_turmas.csv", "r");
    
    if (!fd || !fm) {
        printf("Dados insuficientes.\n");
        if (fd) fclose(fd);
        if (fm) fclose(fm);
        return;
    }

    char buf[512];
    char *line = NULL;
    char *u = NULL;
    char *codigoTurma = NULL;
    char turmas[20][100];
    int nt = 0;
    int count = 0;
    int i = 0;

    /* Coletar turmas do aluno */
    fgets(buf, sizeof(buf), fm); // cabecalho
    while (fgets(buf, sizeof(buf), fm) && nt < 20) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;
        
        u = strtok(line, ";");
        codigoTurma = strtok(NULL, ";");
        
        if (u && codigoTurma && strcmp(u, usuario) == 0) {
            strncpy(turmas[nt], codigoTurma, sizeof(turmas[nt]) - 1);
            turmas[nt][sizeof(turmas[nt]) - 1] = '\0';
            nt++;
        }
        free(line);
        line = NULL;
    }
    fclose(fm);

    if (nt == 0) {
        printf("Voce nao esta em nenhuma turma.\n");
        fclose(fd);
        return;
    }

    /* Verificar se existem aulas registradas */
    fgets(buf, sizeof(buf), fd); // cabecalho
    if (!fgets(buf, sizeof(buf), fd)) {
        printf("Nenhuma aula registrada pelos professores.\n");
        fclose(fd);
        return;
    }
    fseek(fd, 0, SEEK_SET); // volta pro inicio
    fgets(buf, sizeof(buf), fd); // cabecalho novamente

    /* Listar aulas das turmas do aluno */
    printf("\n=== Aulas Agendadas ===\n");
    while (fgets(buf, sizeof(buf), fd)) {
        buf[strcspn(buf, "\n")] = 0;

        line = dupString(buf);
        if (!line) continue;

        char *codigo = strtok(line, ";");
        char *data = strtok(NULL, ";");
        char *tema = strtok(NULL, ";");
        char *conteudo = strtok(NULL, ";");
        char *presenca = strtok(NULL, ";");

        if (codigo) {
            for (i = 0; i < nt; ++i) {
                if (strcmp(codigo, turmas[i]) == 0) {
                    printf("%d. [%s] %s\n", ++count, codigo, data ? data : "?");
                    printf("   Tema: %s\n", tema ? tema : "(sem tema)");
                    printf("   Conteudo: %s\n", conteudo ? conteudo : "(sem conteudo)");
                    
                    /* Verificar se o aluno estava presente */
                    if (presenca && strlen(presenca) > 0) {
                        int presente = 0;
                        char *lista_presenca = dupString(presenca);
                        char *aluno_presenca = strtok(lista_presenca, ",");
                        
                        while (aluno_presenca) {
                            if (strcmp(aluno_presenca, usuario) == 0) {
                                presente = 1;
                                break;
                            }
                            aluno_presenca = strtok(NULL, ",");
                        }
                        
                        if (presente) {
                            printf("   Voce esteve presente nesta aula\n");
                        } else {
                            printf("   Voce nao esteve presente nesta aula\n");
                        }
                        free(lista_presenca);
                    } else {
                        printf("   Lista de presenca nao registrada\n");
                    }
                    printf("   --------------------\n");
                    break;
                }
            }
        }
        free(line);
        line = NULL;
    }

    if (count == 0) {
        printf("Nenhuma aula encontrada para suas turmas.\n");
    } else {
        /* OPCAO: Confirmar presenca */
        printf("\n--- Opcoes ---\n");
        printf("Deseja confirmar presenca em alguma aula? (s/n): ");
        char opcao[10];
        fgets(opcao, sizeof(opcao), stdin);
        opcao[strcspn(opcao, "\n")] = 0;
        
        if (opcao[0] == 's' || opcao[0] == 'S') {
            char codigo[50], data[20];
            printf("Digite o codigo da turma: ");
            fgets(codigo, sizeof(codigo), stdin);
            codigo[strcspn(codigo, "\n")] = 0;
            
            printf("Digite a data da aula (DD/MM/AAAA): ");
            fgets(data, sizeof(data), stdin);
            data[strcspn(data, "\n")] = 0;
            
            confirmarPresenca(usuario, codigo, data);
        }
    }

    fclose(fd);
}

/* =========================================================
   FUNÇÃO: verAtividades
   ========================================================= */
static void verAtividades(const char *usuario) {
    if (!alunoTemTurmasMatriculadas(usuario)) {
        printf("Voce nao esta matriculado em nenhuma turma.\n");
        return;
    }

    FILE *ft = NULL, *fa = NULL, *fm = NULL;
    char buf[512];
    char turmas[20][100];
    
    char *line = NULL;
    char *u = NULL;
    char *codigo = NULL;
    char *titulo = NULL;
    char *descricao = NULL;
    char *prazo = NULL;
    char *tipo = NULL;
    char *valor = NULL;
    int nt = 0;
    int count = 0;
    int i = 0;

    /* Abrir arquivos */
    ft = fopen("dados/professores/turmas.csv", "r");
    fa = fopen("dados/professores/atividades.csv", "r");
    fm = fopen("dados/alunos/alunos_turmas.csv", "r");

    if (!ft || !fa || !fm) {
        printf("Dados insuficientes.\n");
        if (ft) fclose(ft);
        if (fa) fclose(fa);
        if (fm) fclose(fm);
        return;
    }

    /* Coletar turmas do aluno */
    fgets(buf, sizeof(buf), fm); // cabecalho
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
        printf("Voce nao esta em nenhuma turma.\n");
        fclose(ft);
        fclose(fa);
        return;
    }

    /* Verificar se existem atividades */
    fgets(buf, sizeof(buf), fa); // cabecalho
    if (!fgets(buf, sizeof(buf), fa)) {
        printf("Nenhuma atividade publicada pelos professores.\n");
        fclose(ft);
        fclose(fa);
        return;
    }
    fseek(fa, 0, SEEK_SET); // volta pro inicio
    fgets(buf, sizeof(buf), fa); // cabecalho novamente

    /* Listar atividades das turmas do aluno */
    printf("\n=== Atividades para suas turmas ===\n");
    while (fgets(buf, sizeof(buf), fa)) {
        buf[strcspn(buf, "\n")] = 0;

        line = dupString(buf);
        if (!line) continue;

        codigo = strtok(line, ";");
        titulo = strtok(NULL, ";");
        descricao = strtok(NULL, ";");
        prazo = strtok(NULL, ";");
        tipo = strtok(NULL, ";");
        valor = strtok(NULL, ";");

        if (codigo) {
            for (i = 0; i < nt; ++i) {
                if (strcmp(codigo, turmas[i]) == 0) {
                    printf("%d. [%s] %s\n", ++count, codigo, titulo ? titulo : "(sem titulo)");
                    printf("   Descricao: %s\n", descricao ? descricao : "(sem descricao)");
                    printf("   Prazo: %s | Tipo: %s | Valor: %s pontos\n", 
                           prazo ? prazo : "-", 
                           tipo ? tipo : "-", 
                           valor ? valor : "-");
                    printf("   --------------------\n");
                    break;
                }
            }
        }
        free(line);
        line = NULL;
    }

    if (count == 0) {
        printf("Nenhuma atividade encontrada para suas turmas.\n");
    }

    fclose(ft);
    fclose(fa);
}

/* =========================================================
   FUNÇÃO: listarAtividadesParaEnvio
   ========================================================= */
static void listarAtividadesParaEnvio(const char *usuario) {
    if (!alunoTemTurmasMatriculadas(usuario)) {
        printf("Voce nao esta matriculado em nenhuma turma.\n");
        return;
    }

    FILE *fa = fopen("dados/professores/atividades.csv", "r");
    FILE *fm = fopen("dados/alunos/alunos_turmas.csv", "r");
    FILE *fe = fopen("dados/entregas/entregas.csv", "r");
    
    if (!fa || !fm) {
        printf("Dados insuficientes.\n");
        if (fa) fclose(fa);
        if (fm) fclose(fm);
        if (fe) fclose(fe);
        return;
    }

    char buf[512];
    char *line = NULL;
    char *u = NULL;
    char *codigo = NULL;
    char turmas[20][100];
    int nt = 0;
    int count = 0;
    int i = 0;

    /* Coletar turmas do aluno */
    fgets(buf, sizeof(buf), fm); // cabecalho
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
        printf("Voce nao esta em nenhuma turma.\n");
        fclose(fa);
        fclose(fe);
        return;
    }

    /* Verificar entregas ja feitas */
    char entregas_feitas[50][200];
    int ne = 0;
    if (fe) {
        fgets(buf, sizeof(buf), fe); // cabecalho
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

    /* Listar atividades disponiveis para envio */
    printf("\n=== Atividades Disponiveis para Envio ===\n");
    fgets(buf, sizeof(buf), fa); // cabecalho

    while (fgets(buf, sizeof(buf), fa)) {
        buf[strcspn(buf, "\n")] = 0;

        line = dupString(buf);
        if (!line) continue;

        codigo = strtok(line, ";");
        char *titulo = strtok(NULL, ";");
        char *descricao = strtok(NULL, ";");
        char *prazo = strtok(NULL, ";");
        char *tipo = strtok(NULL, ";");
        char *valor = strtok(NULL, ";");

        if (codigo && titulo) {
            for (i = 0; i < nt; ++i) {
                if (strcmp(codigo, turmas[i]) == 0) {
                    /* Verificar se ja enviou esta atividade */
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
                        printf("%d. [%s] %s\n", ++count, codigo, titulo);
                        printf("   Descricao: %s\n", descricao ? descricao : "(sem descricao)");
                        printf("   Prazo: %s | Tipo: %s | Valor: %s pontos\n", 
                               prazo ? prazo : "-", 
                               tipo ? tipo : "-", 
                               valor ? valor : "-");
                        printf("   --------------------\n");
                    }
                    break;
                }
            }
        }
        free(line);
        line = NULL;
    }

    if (count == 0) {
        printf("Nenhuma atividade disponivel para envio.\n");
    }

    fclose(fa);
}

/* =========================================================
   FUNÇÃO: enviarAtividade
   ========================================================= */
static void enviarAtividade(const char *usuario) {
    if (!alunoTemTurmasMatriculadas(usuario)) {
        printf("Voce nao esta matriculado em nenhuma turma.\n");
        return;
    }

    if (!existemAtividadesParaEnvio(usuario)) {
        printf("Nao ha atividades disponiveis para envio.\n");
        return;
    }

    char codigoTurma[50], tituloAtividade[100], arquivo[100];
    
    printf("\n=== Enviar Atividade ===\n");
    
    /* Primeiro mostra as atividades disponiveis */
    listarAtividadesParaEnvio(usuario);
    
    printf("\nDigite os dados para envio:\n");
    printf("Codigo da turma: ");
    fgets(codigoTurma, sizeof(codigoTurma), stdin);
    codigoTurma[strcspn(codigoTurma, "\n")] = 0;

    printf("Titulo da atividade: ");
    fgets(tituloAtividade, sizeof(tituloAtividade), stdin);
    tituloAtividade[strcspn(tituloAtividade, "\n")] = 0;

    /* Verificar se a atividade existe */
    if (!atividade_existe_csv(codigoTurma, tituloAtividade)) {
        printf("Atividade nao encontrada!\n");
        return;
    }

    /* Verificar se o aluno esta matriculado na turma */
    if (!aluno_ja_matriculado(usuario, codigoTurma)) {
        printf("Voce nao esta matriculado nesta turma!\n");
        return;
    }

    printf("Nome do arquivo: ");
    fgets(arquivo, sizeof(arquivo), stdin);
    arquivo[strcspn(arquivo, "\n")] = 0;

    /* Verificar se ja enviou esta atividade */
    FILE *fe = fopen("dados/entregas/entregas.csv", "r");
    if (fe) {
        char buf[512];
        fgets(buf, sizeof(buf), fe); // cabecalho
        while (fgets(buf, sizeof(buf), fe)) {
            buf[strcspn(buf, "\n")] = 0;
            char *line = dupString(buf);
            char *user = strtok(line, ";");
            char *turma = strtok(NULL, ";");
            char *atividade = strtok(NULL, ";");
            
            if (user && turma && atividade && 
                strcmp(user, usuario) == 0 && 
                strcmp(turma, codigoTurma) == 0 && 
                strcmp(atividade, tituloAtividade) == 0) {
                printf("Voce ja enviou esta atividade!\n");
                free(line);
                fclose(fe);
                return;
            }
            free(line);
        }
        fclose(fe);
    }

    char data[20];
    obter_data_atual(data, sizeof(data));

    char linha[512];
    snprintf(linha, sizeof(linha), "%s;%s;%s;%s;%s;Enviado", 
             usuario, codigoTurma, tituloAtividade, arquivo, data);

    if (adicionaLinhaCSV("dados/entregas/entregas.csv", linha)) {
        printf("Atividade enviada com sucesso!\n");
    } else {
        printf("Erro ao enviar atividade.\n");
    }
}

/* =========================================================
   FUNÇÃO: cancelarEnvioAtividade
   ========================================================= */
static void cancelarEnvioAtividade(const char *usuario) {
    FILE *f = fopen("dados/entregas/entregas.csv","r");
    if (!f) { 
        printf("Nenhuma entrega registrada.\n"); 
        return; 
    }

    char buf[512];
    char *line = NULL;
    char *u = NULL;
    char *turma = NULL;
    char *atividade = NULL;
    char *arquivo = NULL;
    char *data = NULL;
    char *status = NULL;
    int c = 0;
    int opcao;
    int entregas_validas[50] = {0};

    printf("\n=== Minhas Entregas para Cancelamento ===\n");
    fgets(buf,sizeof(buf),f); // cabecalho

    while (fgets(buf,sizeof(buf),f)) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;
        
        u = strtok(line,";");
        turma = strtok(NULL,";");
        atividade = strtok(NULL,";");
        arquivo = strtok(NULL,";");
        data = strtok(NULL,";");
        status = strtok(NULL,";");

        if (u && strcmp(u, usuario)==0) {
            /* Verificar se pode cancelar */
            int pode_cancelar = 1;
            char motivo[100] = "";
            
            /* Verificar se status é "Corrigido" */
            if (status && strcmp(status, "Corrigido") == 0) {
                pode_cancelar = 0;
                strcpy(motivo, " (JA CORRIGIDA - NAO PODE CANCELAR)");
            } else {
                /* Verificar se passou do prazo */
                char data_atual[20];
                obter_data_atual(data_atual, sizeof(data_atual));
                
                /* Buscar prazo da atividade */
                char *prazo_atividade = obterPrazoAtividade(turma, atividade);
                if (prazo_atividade && compararDatas(data_atual, prazo_atividade) > 0) {
                    pode_cancelar = 0;
                    strcpy(motivo, " (PRAZO EXPIRADO - NAO PODE CANCELAR)");
                }
            }
            
            if (pode_cancelar) {
                entregas_validas[c] = 1;
                printf("%d. %s - %s\n", c + 1, turma ? turma : "?", atividade ? atividade : "?");
                printf("   Arquivo: %s | Data: %s | Status: %s\n", 
                       arquivo ? arquivo : "?", data ? data : "?", status ? status : "?");
            } else {
                entregas_validas[c] = 0;
                printf("%d. %s - %s%s\n", c + 1, turma ? turma : "?", atividade ? atividade : "?", motivo);
                printf("   Arquivo: %s | Data: %s | Status: %s\n", 
                       arquivo ? arquivo : "?", data ? data : "?", status ? status : "?");
            }
            printf("   --------------------\n");
            c++;
        }

        free(line);
        line = NULL;
    }

    fclose(f);

    if (c == 0) {
        printf("Nenhuma entrega encontrada para cancelamento.\n");
        return;
    }

    printf("\nDigite o numero da entrega para cancelar (0 para cancelar): ");
    char input[10];
    fgets(input, sizeof(input), stdin);
    opcao = atoi(input);

    if (opcao < 1 || opcao > c) {
        printf("Operacao cancelada.\n");
        return;
    }

    /* Verificar se a entrega selecionada pode ser cancelada */
    if (!entregas_validas[opcao - 1]) {
        printf("ERRO: Esta entrega nao pode ser cancelada (ja corrigida ou prazo expirado).\n");
        return;
    }

    /* Reabrir arquivo para remover a entrega selecionada */
    f = fopen("dados/entregas/entregas.csv", "r");
    FILE *f_temp = fopen("dados/entregas/temp.csv", "w");
    
    if (!f || !f_temp) {
        printf("Erro ao abrir arquivos.\n");
        if (f) fclose(f);
        if (f_temp) fclose(f_temp);
        return;
    }

    c = 0;
    fgets(buf, sizeof(buf), f); // cabecalho
    fprintf(f_temp, "%s", buf);

    while (fgets(buf, sizeof(buf), f)) {
        char *temp_line = dupString(buf);
        char *user = strtok(temp_line, ";");
        
        if (user && strcmp(user, usuario) == 0) {
            c++;
            if (c != opcao) {
                /* Mantem as outras entregas */
                fprintf(f_temp, "%s", buf);
            } else {
                /* Remove a entrega selecionada */
                char *t = strtok(NULL, ";");
                char *a = strtok(NULL, ";");
                printf("Entrega cancelada: %s - %s\n", t ? t : "?", a ? a : "?");
            }
        } else {
            /* Mantem entregas de outros usuarios */
            fprintf(f_temp, "%s", buf);
        }
        free(temp_line);
    }

    fclose(f);
    fclose(f_temp);

    remove("dados/entregas/entregas.csv");
    rename("dados/entregas/temp.csv", "dados/entregas/entregas.csv");
    
    printf("Entrega cancelada com sucesso! Voce pode reenviar com os dados corretos.\n");
}

/* =========================================================
   FUNÇÃO: verMinhasEntregas
   ========================================================= */
static void verMinhasEntregas(const char *usuario) {
    if (!alunoTemTurmasMatriculadas(usuario)) {
        printf("Voce nao esta matriculado em nenhuma turma.\n");
        return;
    }

    FILE *f = fopen("dados/entregas/entregas.csv","r");
    if (!f) { 
        printf("Nenhuma entrega registrada.\n"); 
        return; 
    }

    char buf2[512];
    char *line = NULL;
    char *u = NULL;
    char *turma = NULL;
    char *atividade = NULL;
    char *arquivo = NULL;
    char *data = NULL;
    char *status = NULL;
    int c = 0;

    printf("\n=== Minhas Entregas ===\n");
    fgets(buf2,sizeof(buf2),f); // cabecalho

    while (fgets(buf2,sizeof(buf2),f)) {
        buf2[strcspn(buf2, "\n")] = 0;
        line = dupString(buf2);
        if (!line) continue;
        
        u = strtok(line,";");
        turma = strtok(NULL,";");
        atividade = strtok(NULL,";");
        arquivo = strtok(NULL,";");
        data = strtok(NULL,";");
        status = strtok(NULL,";");

        if (u && strcmp(u, usuario)==0) {
            printf("%d. %s - %s\n", ++c, turma ? turma : "?", atividade ? atividade : "?");
            printf("   Arquivo: %s | Data: %s\n", arquivo ? arquivo : "?", data ? data : "?");
            printf("   Status: %s\n", status ? status : "?");
            printf("   --------------------\n");
        }

        free(line);
        line = NULL;
    }

    if (c == 0) {
        printf("Nenhuma entrega encontrada.\n");
    } else {
        /* OPCAO: Cancelar envio */
        printf("\n--- Opcoes ---\n");
        printf("Deseja cancelar alguma entrega? (s/n): ");
        char opcao[10];
        fgets(opcao, sizeof(opcao), stdin);
        opcao[strcspn(opcao, "\n")] = 0;
        
        if (opcao[0] == 's' || opcao[0] == 'S') {
            cancelarEnvioAtividade(usuario);
        }
    }

    fclose(f);
}

/* =========================================================
   FUNÇÃO: obterTipoAtividade (auxiliar)
   ========================================================= */
static char* obterTipoAtividade(const char *turma, const char *atividade) {
    static char tipo[50];
    tipo[0] = '\0';
    
    FILE *f = fopen("dados/professores/atividades.csv", "r");
    if (!f) return NULL;

    char buf[512];
    fgets(buf, sizeof(buf), f); // cabecalho
    
    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0;
        char *line = dupString(buf);
        char *codigo = strtok(line, ";");
        char *titulo = strtok(NULL, ";");
        char *descricao = strtok(NULL, ";");
        char *prazo = strtok(NULL, ";");
        char *tipo_atividade = strtok(NULL, ";");
        char *valor = strtok(NULL, ";");
        
        if (codigo && titulo && 
            strcmp(codigo, turma) == 0 && 
            strcmp(titulo, atividade) == 0) {
            strncpy(tipo, tipo_atividade ? tipo_atividade : "", sizeof(tipo) - 1);
            free(line);
            fclose(f);
            return tipo;
        }
        free(line);
    }
    
    fclose(f);
    return NULL;
}

/* =========================================================
   FUNÇÃO: verNotas (NOVA VERSÃO)
   ========================================================= */
static void verNotas(const char *usuario) {
    if (!alunoTemTurmasMatriculadas(usuario)) {
        printf("Voce nao esta matriculado em nenhuma turma.\n");
        return;
    }

    FILE *f = fopen("dados/alunos/notas.csv","r");
    if (!f) { 
        printf("Nenhuma nota registrada.\n"); 
        return; 
    }

    char buf[512];
    char *line = NULL;
    char *u = NULL;
    char *turma = NULL;
    char *atividade = NULL;
    char *nota_str = NULL;
    char *data = NULL;
    char *observacao = NULL;

    /* Estrutura para armazenar notas por turma */
    typedef struct {
        char codigo_turma[50];
        float nota_prova;
        float soma_atividades;
        int count_atividades;
        int tem_prova;
    } TurmaNotas;

    TurmaNotas turmas[20];
    int num_turmas = 0;

    /* Inicializar estrutura */
    for (int i = 0; i < 20; i++) {
        turmas[i].codigo_turma[0] = '\0';
        turmas[i].nota_prova = 0.0;
        turmas[i].soma_atividades = 0.0;
        turmas[i].count_atividades = 0;
        turmas[i].tem_prova = 0;
    }

    printf("\n=== Minhas Notas ===\n");
    fgets(buf,sizeof(buf),f); // cabecalho

    /* Processar todas as notas */
    while (fgets(buf,sizeof(buf),f)) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;
        
        u = strtok(line,";");
        turma = strtok(NULL,";");
        atividade = strtok(NULL,";");
        nota_str = strtok(NULL,";");
        data = strtok(NULL,";");
        observacao = strtok(NULL,";");

        if (u && strcmp(u, usuario)==0 && turma && nota_str) {
            float nota = atof(nota_str);
            
            /* Encontrar ou criar entrada para esta turma */
            int index = -1;
            for (int i = 0; i < num_turmas; i++) {
                if (strcmp(turmas[i].codigo_turma, turma) == 0) {
                    index = i;
                    break;
                }
            }
            
            if (index == -1 && num_turmas < 20) {
                index = num_turmas;
                strncpy(turmas[num_turmas].codigo_turma, turma, sizeof(turmas[num_turmas].codigo_turma) - 1);
                num_turmas++;
            }
            
            if (index != -1) {
                /* Verificar tipo da atividade */
                char *tipo_atividade = obterTipoAtividade(turma, atividade);
                if (tipo_atividade && strcasecmp(tipo_atividade, "Prova") == 0) {
                    turmas[index].nota_prova = nota;
                    turmas[index].tem_prova = 1;
                } else {
                    turmas[index].soma_atividades += nota;
                    turmas[index].count_atividades++;
                }
            }
        }

        free(line);
        line = NULL;
    }

    fclose(f);

    /* Exibir notas formatadas */
    if (num_turmas == 0) {
        printf("Nenhuma nota encontrada.\n");
        return;
    }

    printf("\n%-15s %-8s %-12s %-8s\n", "TURMA", "PROVA", "ATIVIDADES", "MEDIA");
    printf("------------------------------------------------\n");

    for (int i = 0; i < num_turmas; i++) {
        float media_atividades = 0.0;
        if (turmas[i].count_atividades > 0) {
            media_atividades = turmas[i].soma_atividades / turmas[i].count_atividades;
        }
        
        /* CALCULAR MEDIA FINAL COM LIMITE 0-10 */
        float media_final = 0.0;
        if (turmas[i].tem_prova) {
            media_final = (turmas[i].nota_prova + media_atividades) / 2.0;
        } else if (turmas[i].count_atividades > 0) {
            media_final = media_atividades;
        }
        
        /* LIMITAR MEDIA FINAL ENTRE 0 E 10 */
        if (media_final > 10.0) {
            media_final = 10.0;
        } else if (media_final < 0.0) {
            media_final = 0.0;
        }
        
        /* LIMITAR MEDIA DAS ATIVIDADES ENTRE 0 E 10 */
        if (media_atividades > 10.0) {
            media_atividades = 10.0;
        } else if (media_atividades < 0.0) {
            media_atividades = 0.0;
        }
        
        printf("%-15s ", turmas[i].codigo_turma);
        
        /* Exibir prova (0.0 se não tem prova) */
        if (turmas[i].tem_prova) {
            printf("%-8.1f ", turmas[i].nota_prova);
        } else {
            printf("%-8s ", "-");
        }
        
        /* Exibir média das atividades */
        if (turmas[i].count_atividades > 0) {
            printf("%-12.1f ", media_atividades);
        } else {
            printf("%-12s ", "-");
        }
        
        /* Exibir média final */
        printf("%-8.1f\n", media_final);
    }
}

/* =========================================================
   FUNÇÃO: menuAluno (ATUALIZADA)
   ========================================================= */
void menuAluno(const char *usuario) {
    int opc = 0;
    char buf[10];

    do {
        limparTela();
        printf("\n-----------------------------------------\n");
        printf("||     Portal do Aluno - EduGate       ||\n");
        printf("-----------------------------------------\n");
        printf("Usuario: %s\n", usuario);
        printf("\n1. Ver minhas turmas\n");
        printf("2. Matricular em turma\n");
        printf("------------------------------\n");
        printf("3. Ver aulas agendadas\n");
        printf("4. Ver atividades\n");
        printf("5. Enviar atividade\n");
        printf("6. Ver minhas entregas\n");
        printf("7. Ver minhas notas\n");
        printf("------------------------------\n");
        printf("8. Sair\n");
        printf("\nEscolha: ");
        
        fgets(buf, sizeof(buf), stdin);
        opc = atoi(buf);

        switch(opc) {
            case 1:
                limparTela();
                listarMinhasTurmas(usuario);
                voltarMenu();
                break;

            case 2:
                limparTela();
                matricularEmTurma(usuario);
                voltarMenu();
                break;

            case 3:
                limparTela();
                verAulasAgendadas(usuario);
                voltarMenu();
                break;

            case 4:
                limparTela();
                verAtividades(usuario);
                voltarMenu();
                break;

            case 5:
                limparTela();
                enviarAtividade(usuario);
                voltarMenu();
                break;

            case 6:
                limparTela();
                verMinhasEntregas(usuario);
                voltarMenu();
                break;

            case 7:
                limparTela();
                verNotas(usuario);
                voltarMenu();
                break;

            case 8:
                printf("Saindo...\n");
                break;

            default:
                printf("Opcao invalida.\n");
        }
    } while(opc != 8);
}