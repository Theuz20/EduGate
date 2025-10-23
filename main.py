import os
import time
import json

def limpar():
    os.system("cls")

def menu():
    while True:
        
        print("\n=== Portal EduGate - Relatórios e Análises ===\n")
        print("1. Ver total de alunos cadastrados")
        print("2. Ver senhas fracas")
        print("3. Ver nomes mais comuns")
        print("4. Gerar relatório completo")
        print("5. Sair\n")
        
        escolha = int(input("Selecione uma opção com base no numero dela: "))
    
if __name__ == "__main__":
    menu()