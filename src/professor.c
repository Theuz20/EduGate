#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

/* Função auxiliar segura para duplicar string */
static char* dupString(const char* src) {
    if (!src) return NULL;
    char *dup = malloc(strlen(src) + 1);
    if (dup) {
        strcpy(dup, src);
    }
    return dup;
}

static void listarTurmas(void);
static void cadastrarTurma(const char *usuario_professor);
static void enviarAtividade(void);
static void excluirAtividade(void);
static void corrigirAtividade(void);
static void verNotasAlunos(void);
static void registrarAula(void);
static void verDiario(void);
static void excluirAulaRegistrada(void);
static void removerEntregasDaAtividade(const char *codigoTurma, const char *tituloAtividade);
static int atualizarStatusEntrega(const char *usuario, const char *turma, const char *atividade);

/* Lista todas as turmas cadastradas */
static void listarTurmas() {
    FILE *f = fopen("dados/professores/turmas.csv", "r");
    if (!f) {
        printf("Nenhuma turma cadastrada.\n");
        return;
    }

    char buf[512];
    char *line = NULL;
    char *codigo = NULL;
    char *nome = NULL;
    char *professor = NULL;
    char *periodo = NULL;
    int c = 0;

    fgets(buf, sizeof(buf), f); // Pula cabeçalho

    printf("\n=== Turmas Cadastradas ===\n");
    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0; // remove \n
        line = dupString(buf);
        if (!line) continue;
        
        codigo = strtok(line, ";");
        nome = strtok(NULL, ";");
        professor = strtok(NULL, ";");
        periodo = strtok(NULL, ";");

        printf("%d. %s - %s\n", ++c, codigo ? codigo : "", nome ? nome : "");
        if (professor && periodo) {
            printf("   Professor: %s | Período: %s\n", professor, periodo);
        }
        printf("   --------------------\n");
        free(line);
        line = NULL;
    }

    if (c == 0)
        printf("Nenhuma turma cadastrada.\n");

    fclose(f);
}

/* Permite cadastrar nova turma */
static void cadastrarTurma(const char *usuario_professor) {
    char codigo[50], nome[100], periodo[20];

    printf("\n=== Cadastrar Nova Turma ===\n");
    printf("Código da turma: ");
    fgets(codigo, sizeof(codigo), stdin);
    codigo[strcspn(codigo, "\n")] = 0;

    if (strlen(codigo) == 0) {
        printf("Código não pode ser vazio.\n");
        return;
    }

    printf("Nome da turma: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = 0;

    printf("Período (ex: 2024.1): ");
    fgets(periodo, sizeof(periodo), stdin);
    periodo[strcspn(periodo, "\n")] = 0;

    char linha[256];
    snprintf(linha, sizeof(linha), "%s;%s;%s;%s", codigo, nome, usuario_professor, periodo);

    if (adicionaLinhaCSV("dados/professores/turmas.csv", linha))
        printf("Turma cadastrada com sucesso!\n");
    else
        printf("Erro ao cadastrar turma.\n");
}

/* Permite cadastrar atividades para uma turma */
static void enviarAtividade() {
    char codigo[50], titulo[100], descricao[200], prazo[20], tipo[50], valor[10];

    printf("\n=== Publicar Nova Atividade ===\n");
    
    /* Mostra as turmas disponíveis */
    printf("Turmas disponíveis:\n");
    listarTurmas();
    
    printf("Código da turma: ");
    fgets(codigo, sizeof(codigo), stdin);
    codigo[strcspn(codigo, "\n")] = 0;

    /* ✅ NOVA VALIDAÇÃO: Verificar se a turma existe */
    if (!turma_existe_csv(codigo)) {
        printf(" ERRO: Turma não encontrada! Verifique o código e tente novamente.\n");
        return;
    }

    printf("Título da atividade: ");
    fgets(titulo, sizeof(titulo), stdin);
    titulo[strcspn(titulo, "\n")] = 0;

    /* ✅ VALIDAÇÃO: Título não pode ser vazio */
    if (strlen(titulo) == 0) {
        printf(" ERRO: Título da atividade não pode ser vazio!\n");
        return;
    }

    printf("Descrição: ");
    fgets(descricao, sizeof(descricao), stdin);
    descricao[strcspn(descricao, "\n")] = 0;

    printf("Prazo (DD/MM/AAAA): ");
    fgets(prazo, sizeof(prazo), stdin);
    prazo[strcspn(prazo, "\n")] = 0;

    /* ✅ VALIDAÇÃO: Formato da data */
    if (!validar_data(prazo)) {
        printf(" ERRO: Data inválida! Use o formato DD/MM/AAAA.\n");
        return;
    }

    printf("Tipo (Prova/Trabalho/Exercício): ");
    fgets(tipo, sizeof(tipo), stdin);
    tipo[strcspn(tipo, "\n")] = 0;

    printf("Valor (pontos): ");
    fgets(valor, sizeof(valor), stdin);
    valor[strcspn(valor, "\n")] = 0;

    char linha[512];
    snprintf(linha, sizeof(linha), "%s;%s;%s;%s;%s;%s", 
             codigo, titulo, descricao, prazo, tipo, valor);

    if (adicionaLinhaCSV("dados/professores/atividades.csv", linha))
        printf(" Atividade publicada com sucesso!\n");
    else
        printf(" Erro ao publicar atividade.\n");
}

/* Excluir atividade */
static void excluirAtividade() {
    char codigoTurma[50];
    
    printf("\n=== Excluir Atividade ===\n");
    
    /* Mostra as turmas disponíveis */
    printf("Turmas disponíveis:\n");
    listarTurmas();
    
    printf("Digite o código da turma: ");
    fgets(codigoTurma, sizeof(codigoTurma), stdin);
    codigoTurma[strcspn(codigoTurma, "\n")] = 0;

    /* Validar se a turma existe */
    if (!turma_existe_csv(codigoTurma)) {
        printf("❌ ERRO: Turma não encontrada! Verifique o código e tente novamente.\n");
        return;
    }

    FILE *f = fopen("dados/professores/atividades.csv", "r");
    if (!f) {
        printf("Nenhuma atividade cadastrada.\n");
        return;
    }

    char buf[512];
    char *line = NULL;
    char *codigo = NULL;
    char *titulo = NULL;
    int count = 0;
    int opcao;

    printf("\n=== Atividades da Turma %s ===\n", codigoTurma);
    
    fgets(buf, sizeof(buf), f); // Pula cabeçalho
    
    /* Listar atividades da turma */
    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;
        
        codigo = strtok(line, ";");
        titulo = strtok(NULL, ";");

        if (codigo && strcmp(codigo, codigoTurma) == 0) {
            printf("%d. %s\n", ++count, titulo ? titulo : "?");
        }

        free(line);
        line = NULL;
    }
    fclose(f);

    if (count == 0) {
        printf("Nenhuma atividade encontrada para esta turma.\n");
        return;
    }

    printf("\nDigite o número da atividade para excluir (0 para cancelar): ");
    char input[10];
    fgets(input, sizeof(input), stdin);
    opcao = atoi(input);

    if (opcao < 1 || opcao > count) {
        printf("Operação cancelada.\n");
        return;
    }

    /* Reabre o arquivo para excluir a atividade */
    f = fopen("dados/professores/atividades.csv", "r");
    FILE *f_temp = fopen("dados/professores/temp_atividades.csv", "w");
    
    if (!f || !f_temp) {
        printf("Erro ao abrir arquivos.\n");
        if (f) fclose(f);
        if (f_temp) fclose(f_temp);
        return;
    }

    /* Copiar cabeçalho */
    fgets(buf, sizeof(buf), f);
    fprintf(f_temp, "%s", buf);
    
    /* Processar atividades */
    count = 0;
    int atividade_excluida = 0;
    char titulo_excluido[100] = "";
    
    while (fgets(buf, sizeof(buf), f)) {
        char *temp_line = dupString(buf);
        char *cod = strtok(temp_line, ";");
        char *tit = strtok(NULL, ";");
        
        if (cod && strcmp(cod, codigoTurma) == 0) {
            count++;
            if (count == opcao) {
                /* Esta é a atividade a ser excluída - não copia para o temp */
                strncpy(titulo_excluido, tit ? tit : "", sizeof(titulo_excluido) - 1);
                atividade_excluida = 1;
                printf(" Atividade excluída: %s\n", titulo_excluido);
            } else {
                /* Mantém as outras atividades da mesma turma */
                fprintf(f_temp, "%s", buf);
            }
        } else {
            /* Mantém atividades de outras turmas */
            fprintf(f_temp, "%s", buf);
        }
        free(temp_line);
    }

    fclose(f);
    fclose(f_temp);

    if (atividade_excluida) {
        /* Substitui o arquivo original pelo temporário */
        remove("dados/professores/atividades.csv");
        rename("dados/professores/temp_atividades.csv", "dados/professores/atividades.csv");
        
        /* Também remove entregas relacionadas a esta atividade */
        removerEntregasDaAtividade(codigoTurma, titulo_excluido);
        
        printf(" Atividade excluída com sucesso!\n");
    } else {
        remove("dados/professores/temp_atividades.csv");
        printf(" Erro: Atividade não encontrada.\n");
    }
}

static void excluirAulaRegistrada() {
    char codigoTurma[50];
    
    printf("\n=== Excluir Aula Registrada ===\n");
    
    /* Selecionar turma */
    printf("Turmas disponiveis:\n");
    listarTurmas();
    
    printf("Digite o codigo da turma: ");
    fgets(codigoTurma, sizeof(codigoTurma), stdin);
    codigoTurma[strcspn(codigoTurma, "\n")] = 0;

    /* Validar se a turma existe */
    if (!turma_existe_csv(codigoTurma)) {
        printf("ERRO: Turma nao encontrada! Verifique o codigo e tente novamente.\n");
        return;
    }

    /* Listar aulas da turma */
    FILE *fd = fopen("dados/professores/diario.csv", "r");
    if (!fd) {
        printf("Nenhuma aula registrada para esta turma.\n");
        return;
    }

    char buf[512];
    char *line = NULL;
    char *codigo = NULL;
    char *data = NULL;
    char *tema = NULL;
    int count_aulas = 0;
    int opcao_aula;

    printf("\n=== Aulas da Turma %s ===\n", codigoTurma);
    
    fgets(buf, sizeof(buf), fd); // cabecalho
    while (fgets(buf, sizeof(buf), fd)) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;
        
        codigo = strtok(line, ";");
        data = strtok(NULL, ";");
        tema = strtok(NULL, ";");

        if (codigo && strcmp(codigo, codigoTurma) == 0) {
            printf("%d. %s - %s\n", ++count_aulas, data ? data : "?", tema ? tema : "?");
        }

        free(line);
        line = NULL;
    }
    fclose(fd);

    if (count_aulas == 0) {
        printf("Nenhuma aula encontrada para esta turma.\n");
        return;
    }

    printf("\nDigite o numero da aula para excluir (0 para cancelar): ");
    char input[10];
    fgets(input, sizeof(input), stdin);
    opcao_aula = atoi(input);

    if (opcao_aula < 1 || opcao_aula > count_aulas) {
        printf("Operacao cancelada.\n");
        return;
    }

    /* Encontrar os dados da aula selecionada */
    fd = fopen("dados/professores/diario.csv", "r");
    count_aulas = 0;
    char data_aula[20] = "";
    char tema_aula[100] = "";
    
    fgets(buf, sizeof(buf), fd); // cabecalho
    while (fgets(buf, sizeof(buf), fd)) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;
        
        codigo = strtok(line, ";");
        data = strtok(NULL, ";");
        tema = strtok(NULL, ";");

        if (codigo && strcmp(codigo, codigoTurma) == 0) {
            count_aulas++;
            if (count_aulas == opcao_aula) {
                strncpy(data_aula, data ? data : "", sizeof(data_aula) - 1);
                strncpy(tema_aula, tema ? tema : "", sizeof(tema_aula) - 1);
                break;
            }
        }
        free(line);
        line = NULL;
    }
    fclose(fd);

    if (strlen(data_aula) == 0) {
        printf("ERRO: Aula nao encontrada.\n");
        return;
    }

    /* Confirmar exclusao */
    printf("\nTem certeza que deseja excluir a aula?\n");
    printf("Data: %s\n", data_aula);
    printf("Tema: %s\n", tema_aula);
    printf("Confirmar exclusao? (s/n): ");
    
    char confirmacao[10];
    fgets(confirmacao, sizeof(confirmacao), stdin);
    confirmacao[strcspn(confirmacao, "\n")] = 0;

    if (confirmacao[0] != 's' && confirmacao[0] != 'S') {
        printf("Exclusao cancelada.\n");
        return;
    }

    /* Excluir a aula do arquivo */
    fd = fopen("dados/professores/diario.csv", "r");
    FILE *f_temp = fopen("dados/professores/temp_diario.csv", "w");
    
    if (!fd || !f_temp) {
        printf("Erro ao abrir arquivos.\n");
        if (fd) fclose(fd);
        if (f_temp) fclose(f_temp);
        return;
    }

    /* Copiar cabecalho */
    fgets(buf, sizeof(buf), fd);
    fprintf(f_temp, "%s", buf);

    /* Processar aulas */
    int aula_excluida = 0;
    
    while (fgets(buf, sizeof(buf), fd)) {
        char *temp_line = dupString(buf);
        char *cod = strtok(temp_line, ";");
        char *dat = strtok(NULL, ";");
        char *tem = strtok(NULL, ";");
        
        /* Manter apenas as aulas que NAO sao a que sera excluida */
        if (!(cod && dat && tem && 
              strcmp(cod, codigoTurma) == 0 && 
              strcmp(dat, data_aula) == 0)) {
            fprintf(f_temp, "%s", buf);
        } else {
            aula_excluida = 1;
        }
        free(temp_line);
    }

    fclose(fd);
    fclose(f_temp);

    if (aula_excluida) {
        remove("dados/professores/diario.csv");
        rename("dados/professores/temp_diario.csv", "dados/professores/diario.csv");
        printf("Aula excluida com sucesso!\n");
    } else {
        remove("dados/professores/temp_diario.csv");
        printf("ERRO: Aula nao encontrada para exclusao.\n");
    }
}

/* Função auxiliar para remover entregas da atividade excluída */
static void removerEntregasDaAtividade(const char *codigoTurma, const char *tituloAtividade) {
    FILE *f_entregas = fopen("dados/entregas/entregas.csv", "r");
    if (!f_entregas) return;

    FILE *f_temp = fopen("dados/entregas/temp_entregas.csv", "w");
    if (!f_temp) {
        fclose(f_entregas);
        return;
    }

    char buf[512];
    
    /* Copiar cabeçalho */
    fgets(buf, sizeof(buf), f_entregas);
    fprintf(f_temp, "%s", buf);
    
    /* Processar entregas */
    int entregas_removidas = 0;
    
    while (fgets(buf, sizeof(buf), f_entregas)) {
        char *temp_line = dupString(buf);
        char *usuario = strtok(temp_line, ";");
        char *turma = strtok(NULL, ";");
        char *atividade = strtok(NULL, ";");
        
        /* Mantém apenas as entregas que NÃO são da atividade excluída */
        if (!(turma && atividade && 
              strcmp(turma, codigoTurma) == 0 && 
              strcmp(atividade, tituloAtividade) == 0)) {
            fprintf(f_temp, "%s", buf);
        } else {
            entregas_removidas++;
        }
        free(temp_line);
    }

    fclose(f_entregas);
    fclose(f_temp);

    if (entregas_removidas > 0) {
        remove("dados/entregas/entregas.csv");
        rename("dados/entregas/temp_entregas.csv", "dados/entregas/entregas.csv");
        printf(" %d entrega(s) relacionada(s) também foi/foram removida(s).\n", entregas_removidas);
    } else {
        remove("dados/entregas/temp_entregas.csv");
    }
}

/* Corrigir atividade - NOVO SISTEMA */
static void corrigirAtividade() {
    char codigoTurma[50];
    
    printf("\n=== Corrigir Atividades ===\n");
    
    /* Selecionar turma */
    printf("Turmas disponiveis:\n");
    listarTurmas();
    
    printf("Digite o codigo da turma: ");
    fgets(codigoTurma, sizeof(codigoTurma), stdin);
    codigoTurma[strcspn(codigoTurma, "\n")] = 0;

    /* Validar se a turma existe */
    if (!turma_existe_csv(codigoTurma)) {
        printf("ERRO: Turma nao encontrada! Verifique o codigo e tente novamente.\n");
        return;
    }

    /* Listar atividades da turma */
    FILE *fa = fopen("dados/professores/atividades.csv", "r");
    if (!fa) {
        printf("Nenhuma atividade cadastrada.\n");
        return;
    }

    char buf[512];
    char *line = NULL;
    char *codigo = NULL;
    char *titulo = NULL;
    int count_atividades = 0;
    int opcao_atividade;

    printf("\n=== Atividades da Turma %s ===\n", codigoTurma);
    
    fgets(buf, sizeof(buf), fa); // Pula cabecalho
    while (fgets(buf, sizeof(buf), fa)) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;
        
        codigo = strtok(line, ";");
        titulo = strtok(NULL, ";");

        if (codigo && strcmp(codigo, codigoTurma) == 0) {
            printf("%d. %s\n", ++count_atividades, titulo ? titulo : "?");
        }

        free(line);
        line = NULL;
    }
    fclose(fa);

    if (count_atividades == 0) {
        printf("Nenhuma atividade encontrada para esta turma.\n");
        return;
    }

    printf("\nDigite o numero da atividade para corrigir (0 para cancelar): ");
    char input[10];
    fgets(input, sizeof(input), stdin);
    opcao_atividade = atoi(input);

    if (opcao_atividade < 1 || opcao_atividade > count_atividades) {
        printf("Operacao cancelada.\n");
        return;
    }

    /* Encontrar o titulo da atividade selecionada */
    fa = fopen("dados/professores/atividades.csv", "r");
    char tituloAtividade[100] = "";
    count_atividades = 0;
    
    fgets(buf, sizeof(buf), fa); // Pula cabecalho
    while (fgets(buf, sizeof(buf), fa)) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;
        
        codigo = strtok(line, ";");
        titulo = strtok(NULL, ";");

        if (codigo && strcmp(codigo, codigoTurma) == 0) {
            count_atividades++;
            if (count_atividades == opcao_atividade) {
                strncpy(tituloAtividade, titulo ? titulo : "", sizeof(tituloAtividade) - 1);
                break;
            }
        }
        free(line);
        line = NULL;
    }
    fclose(fa);

    if (strlen(tituloAtividade) == 0) {
        printf("ERRO: Atividade nao encontrada.\n");
        return;
    }

    /* Listar alunos da turma com status de entrega */
    printf("\n=== Alunos da Turma %s - Atividade: %s ===\n", codigoTurma, tituloAtividade);
    
    FILE *fm = fopen("dados/alunos/alunos_turmas.csv", "r");
    FILE *fc = fopen("dados/cadastros/alunos.csv", "r");
    FILE *fn = fopen("dados/alunos/notas.csv", "r");  // ABRIR ARQUIVO DE NOTAS PARA VERIFICAR
    
    if (!fm || !fc) {
        printf("Dados insuficientes.\n");
        if (fm) fclose(fm);
        if (fc) fclose(fc);
        if (fn) fclose(fn);
        return;
    }

    int count_alunos = 0;
    int opcao_aluno;
    char alunos_usuario[50][64];
    char alunos_nome[50][100];
    char alunos_matricula[50][20];
    int alunos_ja_corrigidos[50] = {0};  // ARRAY PARA CONTROLAR ALUNOS JA CORRIGIDOS

    /* Listar alunos matriculados na turma */
    fgets(buf, sizeof(buf), fm); // cabecalho
    while (fgets(buf, sizeof(buf), fm) && count_alunos < 50) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;
        
        char *usuario = strtok(line, ";");
        char *turma = strtok(NULL, ";");

        if (usuario && turma && strcmp(turma, codigoTurma) == 0) {
            strncpy(alunos_usuario[count_alunos], usuario, sizeof(alunos_usuario[count_alunos]) - 1);
            
            /* Buscar informacoes do aluno */
            FILE *fc2 = fopen("dados/cadastros/alunos.csv", "r");
            if (fc2) {
                char buf2[512];
                fgets(buf2, sizeof(buf2), fc2); // cabecalho
                while (fgets(buf2, sizeof(buf2), fc2)) {
                    buf2[strcspn(buf2, "\n")] = 0;
                    char *line2 = dupString(buf2);
                    char *user = strtok(line2, ";");
                    char *senha = strtok(NULL, ";");
                    char *matricula = strtok(NULL, ";");
                    char *nome = strtok(NULL, ";");
                    char *email = strtok(NULL, ";");

                    if (user && strcmp(user, usuario) == 0) {
                        strncpy(alunos_nome[count_alunos], nome ? nome : "", sizeof(alunos_nome[count_alunos]) - 1);
                        strncpy(alunos_matricula[count_alunos], matricula ? matricula : "", sizeof(alunos_matricula[count_alunos]) - 1);
                        free(line2);
                        break;
                    }
                    free(line2);
                }
                fclose(fc2);
            }
            
            /* Verificar se entregou a atividade */
            FILE *fe = fopen("dados/entregas/entregas.csv", "r");
            int entregue = 0;
            if (fe) {
                char buf3[512];
                fgets(buf3, sizeof(buf3), fe); // cabecalho
                while (fgets(buf3, sizeof(buf3), fe)) {
                    buf3[strcspn(buf3, "\n")] = 0;
                    char *line3 = dupString(buf3);
                    char *u_entrega = strtok(line3, ";");
                    char *t_entrega = strtok(NULL, ";");
                    char *a_entrega = strtok(NULL, ";");
                    
                    if (u_entrega && t_entrega && a_entrega &&
                        strcmp(u_entrega, usuario) == 0 &&
                        strcmp(t_entrega, codigoTurma) == 0 &&
                        strcmp(a_entrega, tituloAtividade) == 0) {
                        entregue = 1;
                        free(line3);
                        break;
                    }
                    free(line3);
                }
                fclose(fe);
            }

            /* VERIFICAR SE JA FOI CORRIGIDO */
            int ja_corrigido = 0;
            if (fn) {
                fseek(fn, 0, SEEK_SET); // Voltar ao inicio do arquivo
                char buf4[512];
                fgets(buf4, sizeof(buf4), fn); // cabecalho
                while (fgets(buf4, sizeof(buf4), fn)) {
                    buf4[strcspn(buf4, "\n")] = 0;
                    char *line4 = dupString(buf4);
                    char *u_nota = strtok(line4, ";");
                    char *t_nota = strtok(NULL, ";");
                    char *a_nota = strtok(NULL, ";");
                    
                    if (u_nota && t_nota && a_nota &&
                        strcmp(u_nota, usuario) == 0 &&
                        strcmp(t_nota, codigoTurma) == 0 &&
                        strcmp(a_nota, tituloAtividade) == 0) {
                        ja_corrigido = 1;
                        free(line4);
                        break;
                    }
                    free(line4);
                }
            }
            
            alunos_ja_corrigidos[count_alunos] = ja_corrigido;
            
            char status_entrega[50];
            if (!entregue) {
                strcpy(status_entrega, "NAO ENTREGUE");
            } else if (ja_corrigido) {
                strcpy(status_entrega, "JA CORRIGIDO");
            } else {
                strcpy(status_entrega, "ENTREGUE");
            }
            
            printf("%d. %s - %s | %s\n", 
                   count_alunos + 1, 
                   alunos_matricula[count_alunos], 
                   alunos_nome[count_alunos],
                   status_entrega);
            
            count_alunos++;
        }
        free(line);
        line = NULL;
    }

    fclose(fm);
    fclose(fc);
    if (fn) fclose(fn);

    if (count_alunos == 0) {
        printf("Nenhum aluno matriculado nesta turma.\n");
        return;
    }

    printf("\nDigite o numero do aluno para corrigir (0 para cancelar): ");
    fgets(input, sizeof(input), stdin);
    opcao_aluno = atoi(input);

    if (opcao_aluno < 1 || opcao_aluno > count_alunos) {
        printf("Operacao cancelada.\n");
        return;
    }

    /* Verificar se o aluno selecionado ja foi corrigido */
    int indice_aluno = opcao_aluno - 1;
    if (alunos_ja_corrigidos[indice_aluno]) {
        printf("ERRO: Este aluno ja teve esta atividade corrigida!\n");
        printf("Nao e possivel atribuir uma nova nota para a mesma atividade.\n");
        return;
    }

    char *usuario_aluno = alunos_usuario[indice_aluno];
    char *nome_aluno = alunos_nome[indice_aluno];
    
    printf("\n=== Corrigindo Atividade ===\n");
    printf("Aluno: %s (%s)\n", nome_aluno, usuario_aluno);
    printf("Turma: %s | Atividade: %s\n", codigoTurma, tituloAtividade);
    
    /* Verificar informacoes da entrega */
    FILE *fe = fopen("dados/entregas/entregas.csv", "r");
    if (fe) {
        fgets(buf, sizeof(buf), fe); // cabecalho
        while (fgets(buf, sizeof(buf), fe)) {
            buf[strcspn(buf, "\n")] = 0;
            line = dupString(buf);
            if (!line) continue;
            
            char *usuario = strtok(line, ";");
            char *turma = strtok(NULL, ";");
            char *atividade = strtok(NULL, ";");
            char *arquivo = strtok(NULL, ";");
            char *data = strtok(NULL, ";");
            char *status = strtok(NULL, ";");

            if (usuario && turma && atividade &&
                strcmp(usuario, usuario_aluno) == 0 &&
                strcmp(turma, codigoTurma) == 0 &&
                strcmp(atividade, tituloAtividade) == 0) {
                
                printf("Arquivo enviado: %s\n", arquivo ? arquivo : "N/A");
                printf("Data de entrega: %s\n", data ? data : "N/A");
                printf("Status atual: %s\n", status ? status : "Enviado");
                break;
            }
            free(line);
            line = NULL;
        }
        fclose(fe);
    }

    /* Solicitar nota e observacao */
    char nota[10], observacao[200];
    
    printf("\nDigite a nota (0-10): ");
    fgets(nota, sizeof(nota), stdin);
    nota[strcspn(nota, "\n")] = 0;

    /* Validar nota */
    float nota_float = atof(nota);
    if (nota_float < 0 || nota_float > 10) {
        printf("ERRO: Nota deve estar entre 0 e 10!\n");
        return;
    }

    printf("Digite a observacao: ");
    fgets(observacao, sizeof(observacao), stdin);
    observacao[strcspn(observacao, "\n")] = 0;

    /* Atualizar status da entrega para "Corrigido" */
    int status_atualizado = atualizarStatusEntrega(usuario_aluno, codigoTurma, tituloAtividade);
    
    /* Registrar a nota */
    char data_correcao[20];
    obter_data_atual(data_correcao, sizeof(data_correcao));
    
    char linha_nota[512];
    snprintf(linha_nota, sizeof(linha_nota), "%s;%s;%s;%s;%s;%s", 
            usuario_aluno, codigoTurma, tituloAtividade, nota, data_correcao, observacao);
    
    if (adicionaLinhaCSV("dados/alunos/notas.csv", linha_nota)) {
        printf("\nAtividade corrigida com sucesso!\n");
        printf("Nota: %s | Aluno: %s\n", nota, nome_aluno);
        if (status_atualizado) {
            printf("Status da entrega atualizado para: Corrigido\n");
        }
    } else {
        printf("Erro ao registrar nota.\n");
    }
}

/* Função auxiliar para atualizar status da entrega */
static int atualizarStatusEntrega(const char *usuario, const char *turma, const char *atividade) {
    FILE *f_entregas = fopen("dados/entregas/entregas.csv", "r");
    if (!f_entregas) return 0;

    FILE *f_temp = fopen("dados/entregas/temp_correcao.csv", "w");
    if (!f_temp) {
        fclose(f_entregas);
        return 0;
    }

    char buf[512];
    int atualizado = 0;

    /* Copiar cabeçalho */
    fgets(buf, sizeof(buf), f_entregas);
    fprintf(f_temp, "%s", buf);

    /* Processar entregas */
    while (fgets(buf, sizeof(buf), f_entregas)) {
        char *temp_line = dupString(buf);
        char *user = strtok(temp_line, ";");
        char *t = strtok(NULL, ";");
        char *a = strtok(NULL, ";");
        char *arquivo = strtok(NULL, ";");
        char *data = strtok(NULL, ";");
        char *status = strtok(NULL, ";");
        
        if (user && t && a &&
            strcmp(user, usuario) == 0 &&
            strcmp(t, turma) == 0 &&
            strcmp(a, atividade) == 0) {
            
            /* Atualizar status para Corrigido */
            fprintf(f_temp, "%s;%s;%s;%s;%s;Corrigido\n", 
                   user, t, a, arquivo ? arquivo : "", data ? data : "");
            atualizado = 1;
        } else {
            /* Manter linha original */
            fprintf(f_temp, "%s", buf);
        }
        free(temp_line);
    }

    fclose(f_entregas);
    fclose(f_temp);

    if (atualizado) {
        remove("dados/entregas/entregas.csv");
        rename("dados/entregas/temp_correcao.csv", "dados/entregas/entregas.csv");
    } else {
        remove("dados/entregas/temp_correcao.csv");
    }

    return atualizado;
}

/* Ver notas dos alunos por turma - NOVO SISTEMA */
static void verNotasAlunos() {
    char codigoTurma[50];
    
    printf("\n=== Ver Notas dos Alunos ===\n");
    
    /* Selecionar turma */
    printf("Turmas disponiveis:\n");
    listarTurmas();
    
    printf("Digite o codigo da turma: ");
    fgets(codigoTurma, sizeof(codigoTurma), stdin);
    codigoTurma[strcspn(codigoTurma, "\n")] = 0;

    /* Validar se a turma existe */
    if (!turma_existe_csv(codigoTurma)) {
        printf("ERRO: Turma nao encontrada! Verifique o codigo e tente novamente.\n");
        return;
    }

    /* Listar alunos da turma */
    FILE *fm = fopen("dados/alunos/alunos_turmas.csv", "r");
    FILE *fc = fopen("dados/cadastros/alunos.csv", "r");
    
    if (!fm || !fc) {
        printf("Dados insuficientes.\n");
        if (fm) fclose(fm);
        if (fc) fclose(fc);
        return;
    }

    char buf[512];
    char *line = NULL;
    int count_alunos = 0;
    int opcao_aluno;

    printf("\n=== Alunos da Turma %s ===\n", codigoTurma);
    
    fgets(buf, sizeof(buf), fm); // cabecalho
    while (fgets(buf, sizeof(buf), fm)) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;
        
        char *usuario = strtok(line, ";");
        char *turma = strtok(NULL, ";");

        if (usuario && turma && strcmp(turma, codigoTurma) == 0) {
            /* Buscar informacoes do aluno */
            FILE *fc2 = fopen("dados/cadastros/alunos.csv", "r");
            if (fc2) {
                char buf2[512];
                fgets(buf2, sizeof(buf2), fc2); // cabecalho
                while (fgets(buf2, sizeof(buf2), fc2)) {
                    buf2[strcspn(buf2, "\n")] = 0;
                    char *line2 = dupString(buf2);
                    char *user = strtok(line2, ";");
                    char *senha = strtok(NULL, ";");
                    char *matricula = strtok(NULL, ";");
                    char *nome = strtok(NULL, ";");
                    char *email = strtok(NULL, ";");

                    if (user && strcmp(user, usuario) == 0) {
                        printf("%d. %s - %s\n", 
                               ++count_alunos, 
                               matricula ? matricula : "?", 
                               nome ? nome : "?");
                        break;
                    }
                    free(line2);
                }
                fclose(fc2);
            }
        }
        free(line);
        line = NULL;
    }

    fclose(fm);
    fclose(fc);

    if (count_alunos == 0) {
        printf("Nenhum aluno matriculado nesta turma.\n");
        return;
    }

    printf("\nDigite o numero do aluno para ver as notas (0 para cancelar): ");
    char input[10];
    fgets(input, sizeof(input), stdin);
    opcao_aluno = atoi(input);

    if (opcao_aluno < 1 || opcao_aluno > count_alunos) {
        printf("Operacao cancelada.\n");
        return;
    }

    /* Encontrar o aluno selecionado */
    fm = fopen("dados/alunos/alunos_turmas.csv", "r");
    fc = fopen("dados/cadastros/alunos.csv", "r");
    
    count_alunos = 0;
    char usuario_aluno[64] = "";
    char nome_aluno[100] = "";
    char matricula_aluno[20] = "";
    
    fgets(buf, sizeof(buf), fm); // cabecalho
    while (fgets(buf, sizeof(buf), fm)) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;
        
        char *usuario = strtok(line, ";");
        char *turma = strtok(NULL, ";");

        if (usuario && turma && strcmp(turma, codigoTurma) == 0) {
            count_alunos++;
            if (count_alunos == opcao_aluno) {
                strcpy(usuario_aluno, usuario);
                
                /* Buscar nome e matricula do aluno */
                FILE *fc2 = fopen("dados/cadastros/alunos.csv", "r");
                if (fc2) {
                    char buf2[512];
                    fgets(buf2, sizeof(buf2), fc2); // cabecalho
                    while (fgets(buf2, sizeof(buf2), fc2)) {
                        buf2[strcspn(buf2, "\n")] = 0;
                        char *line2 = dupString(buf2);
                        char *user = strtok(line2, ";");
                        char *senha = strtok(NULL, ";");
                        char *matricula = strtok(NULL, ";");
                        char *nome = strtok(NULL, ";");
                        char *email = strtok(NULL, ";");

                        if (user && strcmp(user, usuario) == 0) {
                            strcpy(nome_aluno, nome ? nome : "");
                            strcpy(matricula_aluno, matricula ? matricula : "");
                            free(line2);
                            break;
                        }
                        free(line2);
                    }
                    fclose(fc2);
                }
                break;
            }
        }
        free(line);
        line = NULL;
    }

    fclose(fm);
    fclose(fc);

    /* Mostrar notas do aluno */
    FILE *fn = fopen("dados/alunos/notas.csv", "r");
    if (!fn) {
        printf("Nenhuma nota registrada.\n");
        return;
    }

    printf("\n=== Notas de %s (%s) ===\n", nome_aluno, matricula_aluno);
    
    int count_notas = 0;
    float soma_notas = 0.0;
    
    fgets(buf, sizeof(buf), fn); // cabecalho
    while (fgets(buf, sizeof(buf), fn)) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;
        
        char *usuario = strtok(line, ";");
        char *turma = strtok(NULL, ";");
        char *atividade = strtok(NULL, ";");
        char *nota = strtok(NULL, ";");
        char *data = strtok(NULL, ";");
        char *observacao = strtok(NULL, ";");

        if (usuario && strcmp(usuario, usuario_aluno) == 0) {
            printf("%d. %s\n", ++count_notas, atividade ? atividade : "?");
            printf("   Nota: %s | Data: %s\n", nota ? nota : "?", data ? data : "?");
            
            /* Calcular soma para media */
            if (nota) {
                float nota_float = atof(nota);
                soma_notas += nota_float;
            }
            
            if (observacao && strlen(observacao) > 0) {
                printf("   Observacao: %s\n", observacao);
            }
            printf("   --------------------\n");
        }

        free(line);
        line = NULL;
    }

    if (count_notas == 0) {
        printf("Nenhuma nota encontrada para este aluno.\n");
    } else {
        /* Calcular e mostrar media */
        float media = soma_notas / count_notas;
        printf("MEDIA GERAL: %.1f\n", media);
        
        /* Classificacao baseada na media */
        if (media >= 9.0) {
            printf("CONCEITO: Excelente\n");
        } else if (media >= 7.0) {
            printf("CONCEITO: Bom\n");
        } else if (media >= 5.0) {
            printf("CONCEITO: Regular\n");
        } else {
            printf("CONCEITO: Insuficiente\n");
        }
    }

    fclose(fn);
}

/* Registrar uma nova aula - SEM LISTA DE PRESENÇA */
static void registrarAula() {
    char codigoTurma[50], data[20], tema[100], conteudo[500];

    printf("\n=== Registrar Nova Aula ===\n");
    
    /* Mostra as turmas disponíveis */
    printf("Turmas disponíveis:\n");
    listarTurmas();
    
    printf("\nDigite os dados da aula:\n");
    printf("Código da turma: ");
    fgets(codigoTurma, sizeof(codigoTurma), stdin);
    codigoTurma[strcspn(codigoTurma, "\n")] = 0;

    /* ✅ VALIDAÇÃO MELHORADA: Verificar se a turma existe */
    if (!turma_existe_csv(codigoTurma)) {
        printf(" ERRO: Turma não encontrada! Verifique o código e tente novamente.\n");
        return;
    }

    printf("Data da aula (DD/MM/AAAA): ");
    fgets(data, sizeof(data), stdin);
    data[strcspn(data, "\n")] = 0;

    /* ✅ VALIDAÇÃO: Formato da data */
    if (!validar_data(data)) {
        printf(" ERRO: Data inválida! Use o formato DD/MM/AAAA.\n");
        return;
    }

    printf("Tema da aula: ");
    fgets(tema, sizeof(tema), stdin);
    tema[strcspn(tema, "\n")] = 0;

    /* ✅ VALIDAÇÃO: Tema não pode ser vazio */
    if (strlen(tema) == 0) {
        printf(" ERRO: Tema da aula não pode ser vazio!\n");
        return;
    }

    printf("Conteúdo ministrado: ");
    fgets(conteudo, sizeof(conteudo), stdin);
    conteudo[strcspn(conteudo, "\n")] = 0;

    char linha[1024];
    snprintf(linha, sizeof(linha), "%s;%s;%s;%s;", 
             codigoTurma, data, tema, conteudo);

    if (adicionaLinhaCSV("dados/professores/diario.csv", linha)) {
        printf(" Aula registrada com sucesso!\n");
        printf(" Os alunos poderão confirmar presença em seus portais.\n");
    } else {
        printf(" Erro ao registrar aula.\n");
    }
}

/* Ver diário de aulas */
static void verDiario() {
    FILE *f = fopen("dados/professores/diario.csv", "r");
    if (!f) {
        printf("Nenhum registro no diário.\n");
        return;
    }

    char buf[512];
    char *line = NULL;
    char *codigo = NULL;
    char *data = NULL;
    char *tema = NULL;
    char *conteudo = NULL;
    char *presenca = NULL;
    int count = 0;

    printf("\n=== Diário de Aulas ===\n");
    
    fgets(buf, sizeof(buf), f); // Pula cabeçalho

    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0;
        line = dupString(buf);
        if (!line) continue;
        
        codigo = strtok(line, ";");
        data = strtok(NULL, ";");
        tema = strtok(NULL, ";");
        conteudo = strtok(NULL, ";");
        presenca = strtok(NULL, ";");

        printf("%d. Turma: %s | Data: %s\n", ++count, codigo ? codigo : "?", data ? data : "?");
        printf("   Tema: %s\n", tema ? tema : "?");
        printf("   Conteúdo: %s\n", conteudo ? conteudo : "?");
        printf("   --------------------\n");

        free(line);
        line = NULL;
    }

    if (count == 0) {
        printf("Nenhum registro encontrado.\n");
    }

    fclose(f);
}

/* Menu principal do professor */
void menuProfessor(const char *usuario) {
    int opc = 0;
    char buf[16];

    do {
        limparTela();
        printf("\n-------------------------------------------\n");
        printf("||     Portal do Professor - EduGate     ||\n");
        printf("-------------------------------------------\n");
        printf("Usuario: %s\n", usuario);
        printf("\n1. Cadastrar turma\n");
        printf("2. Listar turmas\n");
        printf("------------------------------\n");
        printf("3. Enviar atividade\n");
        printf("4. Excluir atividade\n");
        printf("5. Corrigir atividade\n");
        printf("6. Ver notas dos alunos\n");
        printf("------------------------------\n");
        printf("7. Registrar aula\n");
        printf("8. Ver diario de aulas\n");
        printf("9. Excluir aula registrada\n");  // NOVA OPÇÃO
        printf("------------------------------\n");
        printf("10. Sair\n");
        printf("\nEscolha: ");

        fgets(buf, sizeof(buf), stdin);
        opc = atoi(buf);

        switch (opc) {
            case 1:
                limparTela();
                cadastrarTurma(usuario);
                voltarMenu();
                break;

            case 2:
                limparTela();
                listarTurmas();
                voltarMenu();
                break;

            case 3:
                limparTela();
                enviarAtividade();
                voltarMenu();
                break;

            case 4:
                limparTela();
                excluirAtividade();
                voltarMenu();
                break;

            case 5:
                limparTela();
                corrigirAtividade();
                voltarMenu();
                break;

            case 6:
                limparTela();
                verNotasAlunos();
                voltarMenu();
                break;

            case 7:
                limparTela();
                registrarAula();
                voltarMenu();
                break;

            case 8:
                limparTela();
                verDiario();
                voltarMenu();
                break;

            case 9:
                limparTela();
                excluirAulaRegistrada();
                voltarMenu();
                break;

            case 10:
                printf("Saindo...\n");
                break;

            default:
                printf("Opcao invalida.\n");
        }
    } while (opc != 10);
}