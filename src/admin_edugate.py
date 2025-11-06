import csv
import os
import getpass
from datetime import datetime
import hashlib
import sys

def limpar_tela():
    """Limpa a tela do terminal"""
    os.system('cls' if os.name == 'nt' else 'clear')

class AdminEduGate:
    def __init__(self):
        # Define o caminho absoluto para a pasta dados
        self.dados_path = "dados"
        self.admin_file = os.path.join(self.dados_path, "cadastros", "admin.csv")
        self.criar_estrutura_admin()
    
    def criar_estrutura_admin(self):
        """Cria a estrutura de pastas e arquivo de administradores"""
        # Garante que a pasta existe
        os.makedirs(os.path.dirname(self.admin_file), exist_ok=True)
        
        # Criar arquivo de administradores se não existir
        if not os.path.exists(self.admin_file):
            with open(self.admin_file, 'w', encoding='utf-8') as f:
                f.write("usuario;senha_hash;nome;email;data_criacao\n")
    
    def hash_senha(self, senha):
        """Gera hash SHA-256 da senha"""
        return hashlib.sha256(senha.encode()).hexdigest()
    
    def verificar_senha(self, senha, senha_hash):
        """Verifica se a senha corresponde ao hash"""
        return self.hash_senha(senha) == senha_hash
    
    def carregar_admins(self):
        """Carrega a lista de administradores"""
        if not os.path.exists(self.admin_file):
            return []
        
        try:
            admins = []
            with open(self.admin_file, 'r', encoding='utf-8') as file:
                leitor = csv.reader(file, delimiter=';')
                next(leitor)  # Pula cabecalho
                for linha in leitor:
                    if linha and len(linha) >= 3:
                        admins.append(linha)
            return admins
        except Exception as e:
            print(f"ERRO: Nao foi possivel carregar administradores: {e}")
            return []
    
    def cadastrar_admin(self):
        """Cadastra um novo administrador"""
        limpar_tela()
        print("\n" + "="*50)
        print("||         CADASTRO DE ADMINISTRADOR         ||")
        print("="*50)
        
        # Verificar se já existe algum admin
        admins = self.carregar_admins()
        primeiro_cadastro = len(admins) == 0
        
        if primeiro_cadastro:
            print("Primeiro cadastro de administrador!")
        else:
            # Para cadastros subsequentes, requer login de admin existente
            print("E necessario fazer login como administrador para cadastrar novos admins.")
            if not self.fazer_login():
                return
        
        usuario = input("Nome de usuario: ").strip()
        if not usuario:
            print("ERRO: Usuario nao pode estar vazio!")
            input("\nPressione Enter para continuar...")
            return
        
        # Verificar se usuário já existe
        if any(admin[0] == usuario for admin in admins):
            print("ERRO: Este usuario ja esta cadastrado!")
            input("\nPressione Enter para continuar...")
            return
        
        senha = getpass.getpass("Senha: ")
        if len(senha) < 4:
            print("ERRO: A senha deve ter pelo menos 4 caracteres!")
            input("\nPressione Enter para continuar...")
            return
        
        confirmar_senha = getpass.getpass("Confirmar senha: ")
        if senha != confirmar_senha:
            print("ERRO: As senhas nao coincidem!")
            input("\nPressione Enter para continuar...")
            return
        
        nome = input("Nome completo: ").strip()
        if not nome:
            print("ERRO: Nome nao pode estar vazio!")
            input("\nPressione Enter para continuar...")
            return
        
        email = input("Email: ").strip()
        
        # Gerar hash da senha
        senha_hash = self.hash_senha(senha)
        data_criacao = datetime.now().strftime("%d/%m/%Y %H:%M")
        
        # Salvar no arquivo
        try:
            with open(self.admin_file, 'a', encoding='utf-8') as f:
                f.write(f"{usuario};{senha_hash};{nome};{email};{data_criacao}\n")
            
            print(f"SUCESSO: Administrador {usuario} cadastrado com sucesso!")
            
        except Exception as e:
            print(f"ERRO: Nao foi possivel salvar o administrador: {e}")
        
        if primeiro_cadastro:
            print("Este e o primeiro administrador do sistema.")
        
        input("\nPressione Enter para continuar...")
    
    def fazer_login(self):
        """Sistema de login para administradores cadastrados"""
        limpar_tela()
        print("\n" + "="*45)
        print("||          LOGIN ADMINISTRATIVO           ||")
        print("="*45)
        
        admins = self.carregar_admins()
        
        if not admins:
            print("ERRO: Nenhum administrador cadastrado!")
            print("Por favor, cadastre o primeiro administrador.")
            input("\nPressione Enter para continuar...")
            return False
        
        usuario = input("Usuario: ").strip()
        senha = getpass.getpass("Senha: ")
        
        # Verificar credenciais
        for admin in admins:
            if admin[0] == usuario and self.verificar_senha(senha, admin[1]):
                print("SUCESSO: Login realizado com sucesso!")
                print(f"Bem-vindo, {admin[2]}!")
                input("\nPressione Enter para continuar...")
                return True
        
        print("ERRO: Usuario ou senha incorretos!")
        input("\nPressione Enter para continuar...")
        return False
    
    def listar_administradores(self):
        """Lista todos os administradores cadastrados"""
        limpar_tela()
        print("\n" + "="*52)
        print("||            ADMINISTRADORES CADASTRADOS         ||")
        print("="*52)
        
        admins = self.carregar_admins()
        
        if not admins:
            print("Nenhum administrador cadastrado.")
            return
        
        print(f"{'USUARIO':<15} {'NOME':<25} {'EMAIL':<20} {'DATA CADASTRO':<15}")
        print("-" * 80)
        
        for admin in admins:
            if len(admin) >= 5:
                usuario = admin[0]
                nome = admin[2] if len(admin) > 2 else "N/A"
                email = admin[3] if len(admin) > 3 else "N/A"
                data = admin[4] if len(admin) > 4 else "N/A"
                
                print(f"{usuario:<15} {nome:<25} {email:<20} {data:<15}")
    
    def remover_administrador(self):
        """Remove um administrador (apenas para outros admins)"""
        limpar_tela()
        print("\n" + "="*50)
        print("||         REMOVER ADMINISTRADOR           ||")
        print("="*50)
        
        admins = self.carregar_admins()
        
        if len(admins) <= 1:
            print("ERRO: Nao e possivel remover o unico administrador do sistema!")
            input("\nPressione Enter para continuar...")
            return
        
        self.listar_administradores()
        
        usuario_remover = input("\nDigite o usuario do administrador a ser removido: ").strip()
        
        if not usuario_remover:
            print("Operacao cancelada.")
            input("\nPressione Enter para continuar...")
            return
        
        # Verificar se é o próprio usuário
        print("ATENCAO: Esta acao nao pode ser desfeita!")
        confirmacao = input(f"Tem certeza que deseja remover {usuario_remover}? (s/N): ").strip().lower()
        
        if confirmacao != 's':
            print("Operacao cancelada.")
            input("\nPressione Enter para continuar...")
            return
        
        # Remover do arquivo
        admins_restantes = [admin for admin in admins if admin[0] != usuario_remover]
        
        if len(admins_restantes) == len(admins):
            print("ERRO: Administrador nao encontrado!")
            input("\nPressione Enter para continuar...")
            return
        
        # Reescrever arquivo
        try:
            with open(self.admin_file, 'w', encoding='utf-8') as f:
                f.write("usuario;senha_hash;nome;email;data_criacao\n")
                for admin in admins_restantes:
                    f.write(";".join(admin) + "\n")
            
            print(f"SUCESSO: Administrador {usuario_remover} removido com sucesso!")
        except Exception as e:
            print(f"ERRO: Nao foi possivel remover o administrador: {e}")
        
        input("\nPressione Enter para continuar...")
    
    def carregar_dados_csv(self, arquivo):
        """Carrega dados de um arquivo CSV"""
        caminho = os.path.join(self.dados_path, arquivo)
        if not os.path.exists(caminho):
            return []
        
        dados = []
        with open(caminho, 'r', encoding='utf-8') as file:
            leitor = csv.reader(file, delimiter=';')
            next(leitor)  # Pula cabecalho
            for linha in leitor:
                if linha:  # Verifica se a linha não está vazia
                    dados.append(linha)
        return dados
    
    def salvar_dados_csv(self, arquivo, dados):
        """Salva dados em um arquivo CSV"""
        caminho = os.path.join(self.dados_path, arquivo)
        try:
            with open(caminho, 'w', encoding='utf-8') as file:
                # Escreve cabeçalho baseado no arquivo
                if "alunos.csv" in arquivo:
                    file.write("usuario;senha;matricula;nome;email\n")
                elif "professores.csv" in arquivo:
                    file.write("usuario;senha;nome;email\n")
                
                for linha in dados:
                    file.write(";".join(linha) + "\n")
            return True
        except Exception as e:
            print(f"ERRO: Nao foi possivel salvar o arquivo: {e}")
            return False
    
    def excluir_aluno(self):
        """Exclui um aluno do sistema"""
        limpar_tela()
        print("\n" + "="*44)
        print("||            EXCLUIR ALUNO               ||")
        print("="*44)
        
        alunos = self.carregar_dados_csv("cadastros/alunos.csv")
        
        if not alunos:
            print("Nenhum aluno cadastrado no sistema.")
            input("\nPressione Enter para continuar...")
            return
        
        # Listar alunos
        print(f"{'#':<3} {'MATRICULA':<12} {'NOME':<30} {'USUARIO':<15}")
        print("-" * 65)
        
        for i, aluno in enumerate(alunos, 1):
            if len(aluno) >= 5:
                matricula = aluno[2]
                nome = aluno[3]
                usuario = aluno[0]
                print(f"{i:<3} {matricula:<12} {nome:<30} {usuario:<15}")
        
        try:
            opcao = int(input("\nDigite o numero do aluno a ser excluido (0 para cancelar): "))
            if opcao == 0:
                print("Operacao cancelada.")
                input("\nPressione Enter para continuar...")
                return
            
            if opcao < 1 or opcao > len(alunos):
                print("ERRO: Numero invalido!")
                input("\nPressione Enter para continuar...")
                return
            
            aluno_excluir = alunos[opcao - 1]
            usuario_aluno = aluno_excluir[0]
            nome_aluno = aluno_excluir[3]
            
            print(f"\nALUNO SELECIONADO: {nome_aluno} (Usuario: {usuario_aluno})")
            print("ATENCAO: Esta acao removera TODOS os dados do aluno!")
            print("         - Matriculas em turmas")
            print("         - Entregas de atividades")
            print("         - Notas registradas")
            
            confirmacao = input("\nTem certeza que deseja excluir este aluno? (s/N): ").strip().lower()
            
            if confirmacao != 's':
                print("Operacao cancelada.")
                input("\nPressione Enter para continuar...")
                return
            
            # Remover aluno da lista
            alunos_restantes = [aluno for i, aluno in enumerate(alunos) if i != opcao - 1]
            
            # Salvar lista atualizada
            if self.salvar_dados_csv("cadastros/alunos.csv", alunos_restantes):
                # Remover matriculas do aluno
                self.remover_matriculas_aluno(usuario_aluno)
                # Remover entregas do aluno
                self.remover_entregas_aluno(usuario_aluno)
                # Remover notas do aluno
                self.remover_notas_aluno(usuario_aluno)
                
                print(f"SUCESSO: Aluno {nome_aluno} excluido com sucesso!")
            else:
                print("ERRO: Nao foi possivel excluir o aluno.")
                
        except ValueError:
            print("ERRO: Digite um numero valido!")
        except Exception as e:
            print(f"ERRO: {e}")
        
        input("\nPressione Enter para continuar...")
    
    def excluir_professor(self):
        """Exclui um professor do sistema"""
        limpar_tela()
        print("\n" + "="*45)
        print("||           EXCLUIR PROFESSOR             ||")
        print("="*45)
        
        professores = self.carregar_dados_csv("cadastros/professores.csv")
        
        if not professores:
            print("Nenhum professor cadastrado no sistema.")
            input("\nPressione Enter para continuar...")
            return
        
        # Listar professores
        print(f"{'#':<3} {'USUARIO':<15} {'NOME':<30} {'EMAIL':<25}")
        print("-" * 75)
        
        for i, professor in enumerate(professores, 1):
            if len(professor) >= 4:
                usuario = professor[0]
                nome = professor[2]
                email = professor[3]
                print(f"{i:<3} {usuario:<15} {nome:<30} {email:<25}")
        
        try:
            opcao = int(input("\nDigite o numero do professor a ser excluido (0 para cancelar): "))
            if opcao == 0:
                print("Operacao cancelada.")
                input("\nPressione Enter para continuar...")
                return
            
            if opcao < 1 or opcao > len(professores):
                print("ERRO: Numero invalido!")
                input("\nPressione Enter para continuar...")
                return
            
            professor_excluir = professores[opcao - 1]
            usuario_professor = professor_excluir[0]
            nome_professor = professor_excluir[2]
            
            print(f"\nPROFESSOR SELECIONADO: {nome_professor} (Usuario: {usuario_professor})")
            print("ATENCAO: Esta acao removera TODOS os dados do professor!")
            print("         - Turmas criadas")
            print("         - Atividades publicadas")
            print("         - Aulas registradas")
            
            confirmacao = input("\nTem certeza que deseja excluir este professor? (s/N): ").strip().lower()
            
            if confirmacao != 's':
                print("Operacao cancelada.")
                input("\nPressione Enter para continuar...")
                return
            
            # Remover professor da lista
            professores_restantes = [prof for i, prof in enumerate(professores) if i != opcao - 1]
            
            # Salvar lista atualizada
            if self.salvar_dados_csv("cadastros/professores.csv", professores_restantes):
                # Transferir turmas para outro professor ou removê-las
                self.remover_turmas_professor(usuario_professor)
                # Remover atividades do professor
                self.remover_atividades_professor(usuario_professor)
                # Remover aulas do professor
                self.remover_aulas_professor(usuario_professor)
                
                print(f"SUCESSO: Professor {nome_professor} excluido com sucesso!")
            else:
                print("ERRO: Nao foi possivel excluir o professor.")
                
        except ValueError:
            print("ERRO: Digite um numero valido!")
        except Exception as e:
            print(f"ERRO: {e}")
        
        input("\nPressione Enter para continuar...")
    
    def remover_matriculas_aluno(self, usuario_aluno):
        """Remove todas as matriculas de um aluno"""
        matriculas = self.carregar_dados_csv("alunos/alunos_turmas.csv")
        matriculas_restantes = [mat for mat in matriculas if len(mat) >= 2 and mat[0] != usuario_aluno]
        self.salvar_dados_csv("alunos/alunos_turmas.csv", matriculas_restantes)
    
    def remover_entregas_aluno(self, usuario_aluno):
        """Remove todas as entregas de um aluno"""
        entregas = self.carregar_dados_csv("entregas/entregas.csv")
        entregas_restantes = [ent for ent in entregas if len(ent) >= 2 and ent[0] != usuario_aluno]
        self.salvar_dados_csv("entregas/entregas.csv", entregas_restantes)
    
    def remover_notas_aluno(self, usuario_aluno):
        """Remove todas as notas de um aluno"""
        notas = self.carregar_dados_csv("alunos/notas.csv")
        notas_restantes = [nota for nota in notas if len(nota) >= 2 and nota[0] != usuario_aluno]
        self.salvar_dados_csv("alunos/notas.csv", notas_restantes)
    
    def remover_turmas_professor(self, usuario_professor):
        """Remove todas as turmas de um professor"""
        turmas = self.carregar_dados_csv("professores/turmas.csv")
        turmas_restantes = [turma for turma in turmas if len(turma) >= 3 and turma[2] != usuario_professor]
        self.salvar_dados_csv("professores/turmas.csv", turmas_restantes)
    
    def remover_atividades_professor(self, usuario_professor):
        """Remove todas as atividades de um professor"""
        # Para atividades, precisamos verificar pelas turmas do professor
        turmas_professor = self.carregar_dados_csv("professores/turmas.csv")
        codigos_turmas = [turma[0] for turma in turmas_professor if len(turma) >= 3 and turma[2] == usuario_professor]
        
        atividades = self.carregar_dados_csv("professores/atividades.csv")
        atividades_restantes = [atv for atv in atividades if len(atv) >= 1 and atv[0] not in codigos_turmas]
        self.salvar_dados_csv("professores/atividades.csv", atividades_restantes)
    
    def remover_aulas_professor(self, usuario_professor):
        """Remove todas as aulas de um professor"""
        # Para aulas, precisamos verificar pelas turmas do professor
        turmas_professor = self.carregar_dados_csv("professores/turmas.csv")
        codigos_turmas = [turma[0] for turma in turmas_professor if len(turma) >= 3 and turma[2] == usuario_professor]
        
        aulas = self.carregar_dados_csv("professores/diario.csv")
        aulas_restantes = [aula for aula in aulas if len(aula) >= 1 and aula[0] not in codigos_turmas]
        self.salvar_dados_csv("professores/diario.csv", aulas_restantes)
    
    def estatisticas_gerais(self):
        """Mostra estatísticas gerais do sistema"""
        limpar_tela()
        print("\n" + "="*46)
        print("||            ESTATISTICAS GERAIS           ||")
        print("="*46)
        
        # Carregar dados
        alunos = self.carregar_dados_csv("cadastros/alunos.csv")
        professores = self.carregar_dados_csv("cadastros/professores.csv")
        turmas = self.carregar_dados_csv("professores/turmas.csv")
        matriculas = self.carregar_dados_csv("alunos/alunos_turmas.csv")
        notas = self.carregar_dados_csv("alunos/notas.csv")
        atividades = self.carregar_dados_csv("professores/atividades.csv")
        
        # Estatísticas
        total_alunos = len(alunos)
        total_professores = len(professores)
        total_turmas = len(turmas)
        total_atividades = len(atividades)
        
        # Calcular média de alunos por turma
        turmas_alunos = {}
        for matricula in matriculas:
            if len(matricula) >= 2:
                turma = matricula[1]
                if turma in turmas_alunos:
                    turmas_alunos[turma] += 1
                else:
                    turmas_alunos[turma] = 1
        
        media_alunos_turma = sum(turmas_alunos.values()) / len(turmas_alunos) if turmas_alunos else 0
        
        # Calcular média geral de notas
        soma_notas = 0
        count_notas = 0
        for nota in notas:
            if len(nota) >= 4 and nota[3].replace('.', '').isdigit():
                soma_notas += float(nota[3])
                count_notas += 1
        
        media_geral = soma_notas / count_notas if count_notas > 0 else 0
        
        print(f"Total de Alunos: {total_alunos}")
        print(f"Total de Professores: {total_professores}")
        print(f"Total de Turmas: {total_turmas}")
        print(f"Total de Atividades: {total_atividades}")
        print(f"Media de Alunos por Turma: {media_alunos_turma:.1f}")
        print(f"Media Geral de Notas: {media_geral:.1f}")
        print(f"Data da Consulta: {datetime.now().strftime('%d/%m/%Y %H:%M')}")
    
    def relacao_alunos(self):
        """Mostra relação completa de alunos"""
        limpar_tela()
        print("\n" + "="*52)
        print("||                RELAÇÃO DE ALUNOS               ||")
        print("="*52)
        
        alunos = self.carregar_dados_csv("cadastros/alunos.csv")
        matriculas = self.carregar_dados_csv("alunos/alunos_turmas.csv")
        
        if not alunos:
            print("Nenhum aluno cadastrado no sistema.")
            return
        
        print(f"{'MATRICULA':<12} {'NOME':<30} {'EMAIL':<25} {'TURMAS':<10}")
        print("-" * 80)
        
        for aluno in alunos:
            if len(aluno) >= 5:
                matricula = aluno[2]
                nome = aluno[3]
                email = aluno[4]
                
                # Contar turmas do aluno
                turmas_aluno = sum(1 for mat in matriculas if len(mat) >= 2 and mat[0] == aluno[0])
                
                print(f"{matricula:<12} {nome:<30} {email:<25} {turmas_aluno:<10}")
    
    def relacao_professores(self):
        """Mostra relação completa de professores"""
        limpar_tela()
        print("\n" + "="*51)
        print("||             RELAÇÃO DE PROFESSORES            ||")
        print("="*51)
        
        professores = self.carregar_dados_csv("cadastros/professores.csv")
        turmas = self.carregar_dados_csv("professores/turmas.csv")
        
        if not professores:
            print("Nenhum professor cadastrado no sistema.")
            return
        
        print(f"{'USUARIO':<15} {'NOME':<30} {'EMAIL':<25} {'TURMAS':<10}")
        print("-" * 80)
        
        for professor in professores:
            if len(professor) >= 4:
                usuario = professor[0]
                nome = professor[2]
                email = professor[3]
                
                # Contar turmas do professor
                turmas_professor = sum(1 for turma in turmas if len(turma) >= 3 and turma[2] == usuario)
                
                print(f"{usuario:<15} {nome:<30} {email:<25} {turmas_professor:<10}")
    
    def relacao_turmas(self):
        """Mostra relação completa de turmas"""
        limpar_tela()
        print("\n" + "="*56)
        print("||                  RELAÇÃO DE TURMAS                 ||")
        print("="*56)
        
        turmas = self.carregar_dados_csv("professores/turmas.csv")
        matriculas = self.carregar_dados_csv("alunos/alunos_turmas.csv")
        atividades = self.carregar_dados_csv("professores/atividades.csv")
        
        if not turmas:
            print("Nenhuma turma cadastrada no sistema.")
            return
        
        print(f"{'CODIGO':<10} {'NOME':<25} {'PROFESSOR':<20} {'ALUNOS':<8} {'ATIVIDADES':<10}")
        print("-" * 80)
        
        for turma in turmas:
            if len(turma) >= 4:
                codigo = turma[0]
                nome = turma[1]
                professor = turma[2]
                
                # Contar alunos na turma
                alunos_turma = sum(1 for mat in matriculas if len(mat) >= 2 and mat[1] == codigo)
                
                # Contar atividades da turma
                atividades_turma = sum(1 for atv in atividades if len(atv) >= 1 and atv[0] == codigo)
                
                print(f"{codigo:<10} {nome:<25} {professor:<20} {alunos_turma:<8} {atividades_turma:<10}")
    
    def desempenho_alunos(self):
        """Mostra desempenho dos alunos por turma"""
        limpar_tela()
        print("\n" + "="*52)
        print("||              DESEMPENHO DOS ALUNOS             ||")
        print("="*52)
        
        turmas = self.carregar_dados_csv("professores/turmas.csv")
        notas = self.carregar_dados_csv("alunos/notas.csv")
        
        if not turmas:
            print("Nenhuma turma cadastrada no sistema.")
            return
        
        # Dicionário para armazenar médias por turma
        medias_turmas = {}
        
        for turma in turmas:
            if len(turma) >= 1:
                codigo_turma = turma[0]
                
                # Filtrar notas da turma
                notas_turma = [n for n in notas if len(n) >= 3 and n[1] == codigo_turma]
                
                if notas_turma:
                    # Calcular média da turma
                    soma = 0
                    count = 0
                    for nota in notas_turma:
                        if len(nota) >= 4 and nota[3].replace('.', '').isdigit():
                            soma += float(nota[3])
                            count += 1
                    
                    media_turma = soma / count if count > 0 else 0
                    medias_turmas[codigo_turma] = {
                        'nome': turma[1] if len(turma) > 1 else 'N/A',
                        'media': media_turma,
                        'total_notas': count
                    }
        
        if not medias_turmas:
            print("Nenhuma nota registrada no sistema.")
            return
        
        print(f"{'TURMA':<10} {'NOME':<25} {'MEDIA':<8} {'NOTAS':<8}")
        print("-" * 60)
        
        for codigo, dados in medias_turmas.items():
            print(f"{codigo:<10} {dados['nome']:<25} {dados['media']:<8.1f} {dados['total_notas']:<8}")
    
    def top_desempenho(self):
        """Mostra os alunos com melhor desempenho"""
        limpar_tela()
        print("\n" + "="*51)
        print("||             TOP DESEMPENHO - ALUNOS           ||")
        print("="*51)
        
        alunos = self.carregar_dados_csv("cadastros/alunos.csv")
        notas = self.carregar_dados_csv("alunos/notas.csv")
        
        if not alunos or not notas:
            print("Dados insuficientes para analise.")
            return
        
        # Calcular média por aluno
        medias_alunos = {}
        
        for aluno in alunos:
            if len(aluno) >= 5:
                usuario = aluno[0]
                nome = aluno[3]
                
                # Filtrar notas do aluno
                notas_aluno = [n for n in notas if len(n) >= 4 and n[0] == usuario]
                
                if notas_aluno:
                    soma = 0
                    count = 0
                    for nota in notas_aluno:
                        if nota[3].replace('.', '').isdigit():
                            soma += float(nota[3])
                            count += 1
                    
                    if count > 0:
                        media = soma / count
                        medias_alunos[usuario] = {
                            'nome': nome,
                            'media': media,
                            'total_notas': count
                        }
        
        if not medias_alunos:
            print("Nenhuma nota registrada para os alunos.")
            return
        
        # Ordenar por média (melhores primeiro)
        top_alunos = sorted(medias_alunos.items(), key=lambda x: x[1]['media'], reverse=True)[:10]
        
        print(f"{'POS':<4} {'ALUNO':<30} {'MEDIA':<8} {'NOTAS':<8}")
        print("-" * 55)
        
        for pos, (usuario, dados) in enumerate(top_alunos, 1):
            print(f"{pos:<4} {dados['nome']:<30} {dados['media']:<8.1f} {dados['total_notas']:<8}")
    
    def menu_principal(self):
        """Menu principal do sistema administrativo"""
        while True:
            limpar_tela()
            print("\n" + "="*45)
            print("||          MENU ADMINISTRATIVO            ||")
            print("="*45)
            print("1. Estatisticas Gerais")
            print("2. Relacao de Alunos")
            print("3. Relacao de Professores")
            print("4. Relacao de Turmas")
            print("5. Desempenho por Turma")
            print("6. Top Desempenho - Alunos")
            print("7. Gerenciar Administradores")
            print("8. Gerenciar Usuarios")
            print("9. Sair")
            print("="*45)
            
            opcao = input("\nEscolha uma opcao: ").strip()
            
            if opcao == '1':
                self.estatisticas_gerais()
            elif opcao == '2':
                self.relacao_alunos()
            elif opcao == '3':
                self.relacao_professores()
            elif opcao == '4':
                self.relacao_turmas()
            elif opcao == '5':
                self.desempenho_alunos()
            elif opcao == '6':
                self.top_desempenho()
            elif opcao == '7':
                self.menu_administradores()
            elif opcao == '8':
                self.menu_usuarios()
            elif opcao == '9':
                print("\nSaindo do sistema administrativo...")
                break
            else:
                print("\nERRO: Opcao invalida! Tente novamente.")
            
            input("\nPressione Enter para continuar...")
    
    def menu_administradores(self):
        """Menu de gerenciamento de administradores"""
        while True:
            limpar_tela()
            print("\n" + "="*44)
            print("||       GERENCIAR ADMINISTRADORES        ||")
            print("="*44)
            print("1. Listar Administradores")
            print("2. Cadastrar Novo Administrador")
            print("3. Remover Administrador")
            print("4. Voltar ao Menu Principal")
            print("="*44)
            
            opcao = input("\nEscolha uma opcao: ").strip()
            
            if opcao == '1':
                self.listar_administradores()
            elif opcao == '2':
                self.cadastrar_admin()
            elif opcao == '3':
                self.remover_administrador()
            elif opcao == '4':
                break
            else:
                print("\nERRO: Opcao invalida! Tente novamente.")
            
            input("\nPressione Enter para continuar...")
    
    def menu_usuarios(self):
        """Menu de gerenciamento de usuários"""
        while True:
            limpar_tela()
            print("\n" + "="*43)
            print("||         GERENCIAR USUARIOS            ||")
            print("="*43)
            print("1. Excluir Aluno")
            print("2. Excluir Professor")
            print("3. Voltar ao Menu Principal")
            print("="*43)
            
            opcao = input("\nEscolha uma opcao: ").strip()
            
            if opcao == '1':
                self.excluir_aluno()
            elif opcao == '2':
                self.excluir_professor()
            elif opcao == '3':
                break
            else:
                print("\nERRO: Opcao invalida! Tente novamente.")
            
            input("\nPressione Enter para continuar...")

    def executar(self):
        """Método principal para executar o sistema"""
        limpar_tela()
        print("\n" + "="*50)
        print("||     SISTEMA ADMINISTRATIVO EDUGATE     ||")
        print("="*50)
        
        admins = self.carregar_admins()
        
        if not admins:
            print("Nenhum administrador cadastrado.")
            print("Vamos cadastrar o primeiro administrador do sistema.")
            self.cadastrar_admin()
        
        if self.fazer_login():
            self.menu_principal()

# Executar o sistema
if __name__ == "__main__":
    admin = AdminEduGate()
    admin.executar()