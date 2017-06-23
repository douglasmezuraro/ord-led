#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// bibliotecas personalizadas
#include "uteis.h"
#include "registro.h"

// Constantes uteis
#define C_NOME_FILE_DADOS     "dados-inline.txt"
#define C_NOME_FILE_REGISTROS "registros.txt"
#define C_PIPE                '|'
// Constantes do menu de op��es
#define C_PARAR_EXECUCAO 0
#define C_IMPORTAR       1
#define C_BUSCAR         2
#define C_INSERIR        3
#define C_REMOVER        4

// REQUISITOS
void importar();
void buscar();
void inserir();
void inserir();
// MENU
void mostrarMenu();
int selecionarOpcao();
// METODOS UTEIS DE UM REGISTRO

Registro getRegAsRegistro(FILE * arquivo);
void getRegAsString(char str[], FILE * arquivo);
void lerCampo(char campo[], char delimitador, FILE * arquivo);
void getLED(FILE * arquivo, char led[]);
void posicinarNoPrimeiroRegistro(FILE * arquivo);
//
int main();
// REQUISITOS

void importar() {
    FILE * fDados = fopen(C_NOME_FILE_DADOS, "r");
    FILE * fRegistros = fopen(C_NOME_FILE_REGISTROS, "a+");

    char buffer[C_QTD_CAMPOS * C_TAMANHO_CAMPO],
         led[10];

    limparString(buffer);
    limparString(led);

    rewind(fDados);
    rewind(fRegistros);

    getLED(fRegistros, led);
    fputs(led, fRegistros);

    Registro reg = getRegAsRegistro(fDados);

    while(assigned(reg)) {
        registroToString(reg, buffer);
        fputs(buffer, fRegistros);
        limparString(buffer);
        reg = getRegAsRegistro(fDados);
    }
}

void buscar() {
    FILE * fd = fopen(C_NOME_FILE_REGISTROS, "r");
    bool match = false;
    char inscricao[C_TAMANHO_CAMPO],
         tam[C_TAMANHO_CAMPO],
         key[C_TAMANHO_CAMPO],
         buffer[C_QTD_CAMPOS * C_TAMANHO_CAMPO];

    limparString(tam); limparString(key); limparString(buffer); limparBuffer();

    puts("Qual inscricao deseja buscar?");
    gets(inscricao);

    posicinarNoPrimeiroRegistro(fd);

    long byteOffset = -1;
    int iTam = 0;

    while(!match) {
      lerCampo(tam, C_PIPE, fd);
      iTam = atoi(tam);

      byteOffset = ftell(fd); // pega o byteoffset do inicio de um registro

      lerCampo(key, C_PIPE, fd);
      removerCaractere(key, C_PIPE);

      fseek(fd, byteOffset, SEEK_SET);    // posiciona no inicio do registro

      if(stringsIguais(key, inscricao)) {
        getRegAsString(buffer, fd);
        match = true;
      }
      else fseek(fd, iTam, SEEK_CUR); // senao pula p/ o proximo
    }

    puts("OCORRENCIA: ");
    puts(buffer);
}

void inserir() {
    Registro reg = newRegistro();

    limparBuffer();

    printf("\nDigite a inscricao:\n  > ");
    gets(reg.inscricao);

    printf("\nDigite o nome:\n  > ");
    gets(reg.nome);

    printf("\nDigite o curso:\n  > ");
    gets(reg.curso);

    printf("\nDigite o score:\n  > ");
    gets(reg.score);

    char str[C_QTD_CAMPOS * C_TAMANHO_CAMPO];
    limparString(str);

    setTamanhoRegistro(&reg);
    registroToString(reg, str);
    puts("STRING:");
    puts(str);

    printRegistro(reg);
}

void remover() {
    // TODO: Implementar o m�todo de remo��o
}

// MENU

void mostrarMenu() {
    switch(selecionarOpcao()) {
        case C_PARAR_EXECUCAO:
            exit(EXIT_SUCCESS);
            break;
        case C_IMPORTAR:
            importar();
            puts("Dados importados com sucesso!");
            break;
        case C_BUSCAR:
            buscar();
            //puts("Dados buscados com sucesso!");
            break;
        case C_INSERIR:
            inserir();
            //puts("Dados inseridos com sucesso!");
            break;
        case C_REMOVER:
            remover();
            //puts("Dados removidos com sucesso!");
            break;
        default:
            puts("Opcao invalida!");
    }

    getchar();

    puts("Pressione qualquer tecla para continuar...");
    getchar();

    system("cls");
    mostrarMenu();
}

int selecionarOpcao() {
    puts("MENU:");
    puts("  1 - Importar.");
    puts("  2 - Buscar.");
    puts("  3 - Inserir.");
    puts("  4 - Remover.");
    puts("");
    puts("  0 - Sair.");
    puts("");
    puts("> Selecione uma opcao: ");

    int aux = -1;

    scanf("%i", &aux);

    return aux;
}

Registro getRegAsRegistro(FILE * arquivo) {
    Registro reg = newRegistro();

    lerCampo(reg.inscricao, C_PIPE, arquivo);
    lerCampo(reg.nome, C_PIPE, arquivo);
    lerCampo(reg.curso, C_PIPE, arquivo);
    lerCampo(reg.score, C_PIPE, arquivo);

    setTamanhoRegistro(&reg);

    removerPipeRegistro(&reg);

    return reg;
}

void getRegAsString(char str[], FILE * arquivo) {
  int i;
  char campo[C_TAMANHO_CAMPO];

  limparString(str); limparString(campo);

  for(i = 1; i <= C_QTD_CAMPOS; i++) {
    lerCampo(campo, C_PIPE, arquivo);
    strcat(str, campo);
  }
}

void lerCampo(char campo[], char delimitador, FILE * arquivo) {
    int i = 0;
    char flag;

    limparString(campo);

    if(flag = fgetc(arquivo) != EOF)
        fseek(arquivo, -1l, SEEK_CUR);

    if(feof(arquivo) == 0) {
      do {
          campo[i] = fgetc(arquivo);
          i++;
      } while(campo[i - 1] != delimitador);
    }

    campo[i] = '\0'; // finaliza a string para evitar lixo
}

void getLED(FILE * arquivo, char led[])
// Esse m�todo atribui ao par�metro led uma string que contenha a led do arquivo onde
// caso o arquivo n�o exista � retornado a led apontando para -1
{
    lerCampo(led, ']', arquivo);

    if(stringsIguais(led, C_EMPTY_STRING))
      strcpy(led, "[LED=*-1]");
}

void posicinarNoPrimeiroRegistro(FILE * arquivo)
// OBJETIVO : posicionar o ponteiro do arquivo depois da LED
{
  char aux;
  rewind(arquivo);
  do {
    aux = fgetc(arquivo);
  } while(aux != ']');
}

// METODO PRINCIPAL

int main() {
    mostrarMenu();
    exit(EXIT_SUCCESS);
}
