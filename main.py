import os
import time
import csv

alunos = []

def aluninhos():
    with open("output/alunos.csv", mode="r", encoding="utf-8") as arquivo_csv:
        leitor_dict = csv.DictReader(arquivo_csv)
        for linha in leitor_dict:
            alunos.append(linha)
        
def limpar():
    os.system("cls")

def menu():
    while True:
        
        print("\n === Portal EduGate - Relatórios e Análises ===\n")
        print("1. Ver total de alunos cadastrados")
        print("2. Ver senhas fracas")
        print("3. Ver nomes mais comuns")
        print("4. Gerar relatório completo")
        print("5. Sair\n")
        
        try:
            escolha = int(input("Selecione uma opção com base no numero dela: "))
            if escolha == 1:
                total_de_alunos()
        except ValueError:
            print("\nErro: Entrada invalida. Por favor, digite apenas numeros inteiros")
            input("Aperte qualquer tecla para voltar ao menu principal: ")
    
    
def total_de_alunos():
    limpar()
    print(f"Total de alunos cadastrados: {len(alunos)}")
    
    
if __name__ == "__main__":
    aluninhos()
    menu()