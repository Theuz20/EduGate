#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "util.h"
#include "aluno.h"
#include "professor.h"

/* Gera uma matrícula aleatória de 5 dígitos */
void gerarMatriculaAleatoria(char *matricula, size_t tamanho) {
    int ultima_matricula = 0;
    
    FILE *f = fopen("dados/cadastros/alunos.csv", "r");
    if (f) {
        char linha[256];
        fgets(linha, sizeof(linha), f); // Pular cabeçalho
        
        while (fgets(linha, sizeof(linha), f)) {
            char temp[256];
            strcpy(temp, linha);
            
            char *token = strtok(temp, ";"); // usuário
            token = strtok(NULL, ";");       // senha  
            token = strtok(NULL, ";");       // matrícula
            
            if (token) {
                int mat = atoi(token);
                if (mat > ultima_matricula) {
                    ultima_matricula = mat;
                }
            }
        }
        fclose(f);
    }
    
    snprintf(matricula, tamanho, "%05d", ultima_matricula + 1);
}

/* Função para executar o sistema administrativo */
void executarSistemaAdmin() {
    printf("\nAbrindo sistema administrativo...\n");
    
    // Verificar se o arquivo Python existe
    FILE *f = fopen("admin_edugate.py", "r");
    if (!f) {
        printf(" ERRO: Arquivo admin_edugate.py não encontrado!\n");
        printf("Certifique-se que está na mesma pasta do programa.\n");
        return;
    }
    fclose(f);
    
    printf("Executando sistema administrativo Python...\n");
    
#ifdef _WIN32
    int resultado = system("python admin_edugate.py");
#else
    int resultado = system("python3 admin_edugate.py");
#endif

    if (resultado != 0) {
        printf(" Erro ao executar sistema administrativo.\n");
        
        // Tentar com python3 se python falhou
#ifdef _WIN32
        resultado = system("python3 admin_edugate.py");
#else
        resultado = system("python admin_edugate.py");
#endif
        
        if (resultado != 0) {
            printf(" Erro definitivo no sistema administrativo.\n");
        }
    }
}

int main() {
    configurarCodificacao();
    criarEstruturaPastas();

    /* Garante que os CSVs existam com cabeçalho */
    if (!arquivoExiste("dados/cadastros/alunos.csv")) {
        adicionaLinhaCSV("dados/cadastros/alunos.csv", "usuario;senha;matricula;nome;email");
    }
    if (!arquivoExiste("dados/cadastros/professores.csv")) {
        adicionaLinhaCSV("dados/cadastros/professores.csv", "usuario;senha;nome;email");
    }
    if (!arquivoExiste("dados/professores/turmas.csv")) {
        adicionaLinhaCSV("dados/professores/turmas.csv", "codigo;nome;professor;periodo");
    }
    if (!arquivoExiste("dados/professores/atividades.csv")) {
        adicionaLinhaCSV("dados/professores/atividades.csv", "codigo;titulo;descricao;prazo;tipo;valor");
    }
    if (!arquivoExiste("dados/alunos/alunos_turmas.csv")) {
        adicionaLinhaCSV("dados/alunos/alunos_turmas.csv", "usuario;codigoTurma;dataMatricula");
    }
    if (!arquivoExiste("dados/alunos/notas.csv")) {
        adicionaLinhaCSV("dados/alunos/notas.csv", "usuario;codigoTurma;atividade;nota;data;observacao");
    }
    if (!arquivoExiste("dados/entregas/entregas.csv")) {
        adicionaLinhaCSV("dados/entregas/entregas.csv", "usuario;codigoTurma;atividade;arquivo;dataEntrega;status");
    }
    if (!arquivoExiste("dados/professores/diario.csv")) {
        adicionaLinhaCSV("dados/professores/diario.csv", "codigo;data;tema;conteudo;presenca");
    }

    int opc = 0;
    char buf[32];

    do {
        limparTela();
        printf("\n------------------------------------\n");
        printf("||       Bem-vindo ao EduGate     ||\n");
        printf("------------------------------------\n");
        printf("1. Login como Aluno\n");
        printf("2. Login como Professor\n");
        printf("3. Criar conta de Aluno\n");
        printf("4. Criar conta de Professor\n");
        printf("5. Sistema Administrativo\n");  // NOVA OPÇÃO
        printf("6. Sair\n");
        printf("\nEscolha: ");

        fgets(buf, sizeof(buf), stdin);
        opc = atoi(buf);

        switch (opc) {
            case 1: {                
                char usuario[64], senha[64];
                limparTela();
                printf("== Login de Aluno ==\n");
                printf("\nUsuário: ");
                fgets(usuario, sizeof(usuario), stdin);
                usuario[strcspn(usuario, "\n")] = 0;

                printf("Senha: ");
                fgets(senha, sizeof(senha), stdin);
                senha[strcspn(senha, "\n")] = 0;

                if (validar_login("dados/cadastros/alunos.csv", usuario, senha)) {
                    limparTela();
                    menuAluno(usuario);
                } else {
                    printf("Usuário/senha inválidos.\n");
                }
                break;
            }

            case 2: {
                char usuario[64], senha[64];
                limparTela();
                printf("== Login Professor ==\n");
                printf("\nUsuário: ");
                fgets(usuario, sizeof(usuario), stdin);
                usuario[strcspn(usuario, "\n")] = 0;

                printf("Senha: ");
                fgets(senha, sizeof(senha), stdin);
                senha[strcspn(senha, "\n")] = 0;

                if (validar_login("dados/cadastros/professores.csv", usuario, senha)) {
                    limparTela();
                    menuProfessor(usuario);
                } else {
                    printf("Usuário/senha inválidos.\n");
                }
                break;
            }

            case 3: {
                char usuario[64], senha[64], nome[100], email[100], matricula[20];
                printf("\n=== Cadastro de Aluno ===\n");
    
                printf("Usuario: ");
                fgets(usuario, sizeof(usuario), stdin);
                usuario[strcspn(usuario, "\n")] = 0;

                if (strlen(usuario) == 0) {
                    printf("Usuario nao pode ser vazio.\n");
                    break;
                }

                if (usuario_existe_csv("dados/cadastros/alunos.csv", usuario) ||
                    usuario_existe_csv("dados/cadastros/professores.csv", usuario)) {
                    printf("Usuario ja existe.\n");
                    break;
                }

                printf("Senha: ");
                fgets(senha, sizeof(senha), stdin);
                senha[strcspn(senha, "\n")] = 0;

                if (strlen(senha) == 0) {
                    printf("Senha nao pode ser vazia.\n");
                    break;
                }

                printf("Nome completo: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = 0;

                printf("Email: ");
                fgets(email, sizeof(email), stdin);
                email[strcspn(email, "\n")] = 0;

                /* Gera matrícula automática de 5 dígitos */
                gerarMatriculaAleatoria(matricula, sizeof(matricula));

                /* NOVO: Gerar hash da senha */
                char *senha_hash = hash_senha(senha);
                
                char linha[512];
                snprintf(linha, sizeof(linha), "%s;%s;%s;%s;%s", 
                        usuario, senha_hash, matricula, nome, email);

                 if (adicionaLinhaCSV("dados/cadastros/alunos.csv", linha)) {
                    printf("Conta de aluno criada com sucesso!\n");
                    printf("Sua matricula é: %s\n", matricula);  // ✅ MOSTRA A MATRÍCULA CORRETA
                    printf("Guarde esta matrícula para seu login!\n");
                } else {
                    printf("Erro ao criar conta.\n");
                }
                
                free(senha_hash);
                break;
            }

            case 4: {
                char usuario[64], senha[64], nome[100], email[100];
                printf("\n=== Cadastro de Professor ===\n");
                
                printf("Usuario: ");
                fgets(usuario, sizeof(usuario), stdin);
                usuario[strcspn(usuario, "\n")] = 0;

                if (strlen(usuario) == 0) {
                    printf("Usuario nao pode ser vazio.\n");
                    break;
                }

                if (usuario_existe_csv("dados/cadastros/professores.csv", usuario) ||
                    usuario_existe_csv("dados/cadastros/alunos.csv", usuario)) {
                    printf("Usuario ja existe.\n");
                    break;
                }

                printf("Senha: ");
                fgets(senha, sizeof(senha), stdin);
                senha[strcspn(senha, "\n")] = 0;

                if (strlen(senha) == 0) {
                    printf("Senha nao pode ser vazia.\n");
                    break;
                }

                printf("Nome completo: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = 0;

                printf("Email: ");
                fgets(email, sizeof(email), stdin);
                email[strcspn(email, "\n")] = 0;

                /* NOVO: Gerar hash da senha */
                char *senha_hash = hash_senha(senha);
                
                char linha[512];
                snprintf(linha, sizeof(linha), "%s;%s;%s;%s", 
                        usuario, senha_hash, nome, email);

                if (adicionaLinhaCSV("dados/cadastros/professores.csv", linha)) {
                    printf("Conta de professor criada com sucesso!\n");
                } else {
                    printf("Erro ao criar conta.\n");
                }
                
                free(senha_hash); // Liberar memória do hash
                break;
            }

            case 5:
                limparTela();
                executarSistemaAdmin();
                break;

            case 6:
                printf("Saindo do EduGate. Até mais!\n");
                break;

            default:
                printf("Opção inválida.\n");
                break;
        }

    } while (opc != 6);

    return 0;
}
