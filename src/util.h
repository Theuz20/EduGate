#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void configurarCodificacao();
void limparTela();
void pausarTela();
void voltarMenu();
void criarEstruturaPastas();

/* CSV helpers (separator is ';') */
int arquivoExiste(const char *path);
int adicionaLinhaCSV(const char *path, const char *linha);
int usuario_existe_csv(const char *path, const char *usuario);
int turma_existe_csv(const char *codigo);
int atividade_existe_csv(const char *codigo_turma, const char *titulo_atividade);
int aluno_ja_matriculado(const char *usuario, const char *codigo_turma);
int validar_login(const char *path, const char *usuario, const char *senha);

/* Novas funções utilitárias */
void obter_data_atual(char *buffer, size_t tamanho);
void limparBuffer();
void exibir_erro(const char *mensagem);
void exibir_sucesso(const char *mensagem);
void exibir_info(const char *mensagem);
int validar_data(const char *data);

/* Funções de validação para aluno.c */
int existemTurmasDisponiveis(void);
int alunoTemTurmasMatriculadas(const char *usuario);
int existemAtividadesParaEnvio(const char *usuario);

/* NOVAS FUNÇÕES PARA VERIFICAÇÃO DE PRAZO */
char* obterPrazoAtividade(const char *turma, const char *atividade);
int compararDatas(const char *data1, const char *data2);

#endif // UTIL_H