import os
import time
import csv
import bycript

alunos = []

def login():
    while True:   
        print("=== Login -- Portal EduGate ===\n")
        print("""Bem vindo! Acesse sua conta para continuar e visualizar os relatórios.""")
        usuario = input("Usuario: ")
        senha = input("Senha: ")
        break

def aluninhos():
    with open("output/alunos.csv", mode="r", encoding="utf-8") as arquivo_csv:
        leitor_dict = csv.DictReader(arquivo_csv)
        for linha in leitor_dict:
            alunos.append(linha)
        
def limpar():
    os.system("cls")

def menu():
    while True:
        limpar()
        print("=== Portal EduGate - Relatórios e Análises ===\n")
        print("1. Ver total de alunos cadastrados")
        print("2. Ver senhas fracas")
        print("3. Ver nomes mais comuns")
        print("4. Gerar relatório completo")
        print("5. Sair\n")
        
        try:
            escolha = int(input("Selecione uma opção com base no numero dela: "))
            
            if escolha < 1 or escolha > 5:
                limpar()
                print("\nErro: Opção inválida. Digite um número entre 1 e 5.")
                input("Aperte qualquer tecla para voltar ao menu principal: ")
                continue
            elif escolha == 1:
                total_de_alunos()
                break
            elif escolha == 2:
                senhas_fracas()
                break
        except ValueError:
            limpar()
            print("\nErro: Entrada invalida. Por favor, digite apenas numeros inteiros")
            input("Aperte qualquer tecla para voltar ao menu principal: ")
    

def total_de_alunos():
    limpar()
    print(f"Total de alunos cadastrados: {len(alunos)}")

def senhas_fracas():
    usuarios = alunos
    for nome in usuarios:
        print(f"{nome["Senha"]}")
    
if __name__ == "__main__":
    aluninhos()
    login()