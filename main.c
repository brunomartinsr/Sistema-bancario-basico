#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <time.h>

//estruturas
typedef struct{
    int conta_numero;
    char titular[50];
    char senha[20];
    float saldo;
} ContaBancaria;

typedef struct {
    int conta_origem;
    int conta_destino;
    float valor;
    char data[10];
} Trasacao;

//variaveis globais para os números de contas e saldos
float saldo_inicial = 0.0;
int numero_atual = 0;

//DECLARAÇÃO DE FUNÇÕES A BAIXO
//Função do menu inicial
int menu_de_entrada();

//Funções para criação de conta
void criar_conta(ContaBancaria *conta);
int confirmar_senha(char senha[], char senha_confirmada[]);
void salvar_conta(ContaBancaria *conta);
void atualizar_numero_conta();

//Fução para login
void login(int numero_conta, char senha_conta[]);

//Função para o menu de operações disponíveis para o usuario
void menu_operacoes_basicas(int numero_conta);
void realizar_operacao(int opcao, int numero_conta);

//Funções para depositar em uma conta
void depositar(int numero_conta);
int validar_numero(int numero);//Função para checar se o número da conta existe no arquivo
int depositar_saldo(int numero_conta, float valor);

//Função para verificar a senha
int validar_senha(int numero_conta, char senha_conta[]);

//funções para sacar
void sacar(int numero_conta);
int checar_saldo(int numero_conta, float valor); // checa se a conta tem saldo suficiente
int retirar_saldo(int numero_conta, float valor);

//funções para transferir 
void transferir(int numero_conta);
void registrar_transacao(int numero_conta, int conta_destino, float valor);

//funções para consultar saldo de uma conta
void consultar_saldo(int numero_conta);

int main(){   
    setlocale(LC_ALL, "Portuguese");
    //Criando variáveis que referenciam a estrutura
    ContaBancaria conta_banco;
    int numero_conta = 0;
    char senha_conta[20];
    int opcao_escolhida = menu_de_entrada();

    switch (opcao_escolhida) {
        case 1:
            criar_conta(&conta_banco);
            //mostrando dados importantes da conta para o usuario
            printf("---DADOS DA CONTA---\n");
            printf("Numero da conta: %d\n", conta_banco.conta_numero);
            printf("Nome registrado: %s\n", conta_banco.titular);
            printf("Saldo disponivel: %.2f\n", conta_banco.saldo);
            break;
        case 2: {
            login(numero_conta, senha_conta);//chama a função login passando as variaveis do numero e senha
            break;
        }
        case 3:
            printf("Saindo...\n");
            break;
        default:
            printf("Opção inválida! Tente novamente.\n");
    }

    return 0;
}

int menu_de_entrada(){
    int opcao;
    printf("Bem vindo!\n");
    do{
        //Mostra as opções para o usuário e permite ele escolher
        printf("\n======== SISTEMA BANCÁRIO ========\n");
        printf("1- Cadastrar\n");
        printf("2- Login\n");
        printf("3- Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        //valida a opção escolhida
        if(opcao < 1 || opcao > 6){
            printf("OPCAO INVALIDA! TENTE NOVAMENTE.\n");
        }
    } while(opcao < 1 || opcao > 3);//mostra o menu enquanto a opção não for válida
    
    return opcao;
}

void criar_conta(ContaBancaria *conta) {
    char senha_confirmar[20]; //variável para confirmação de senha
    int validado; //variavel para validação

    // Chama a função para atualizar o número da conta com base no arquivo
    atualizar_numero_conta();

    // Incrementa o número da conta para a nova conta
    numero_atual++;
    conta->conta_numero = numero_atual;
    //menu de cadastro, colhe e armazena os dados digitados 
    printf("----CADASTRO----\n");
    printf("Digite o nome: ");
    scanf("%49s", conta->titular);
    printf("Digite a senha: ");
    scanf("%19s", conta->senha);

    // confirmação da senha
    while (1) {
        printf("Confirmar senha: ");
        scanf("%19s", senha_confirmar);
        validado = confirmar_senha(conta->senha, senha_confirmar);
        
        if (validado == 1) { // se validado, armazena o saldo inicial na conta
            conta->saldo = saldo_inicial;
            printf("Conta criada com sucesso!\n");
            // Salva a conta no arquivo
            salvar_conta(conta);
            break;
        } else {
            printf("ERRO AO CONFIRMAR SENHA: senhas diferentes\n");
        }
    }
}

void atualizar_numero_conta() {
    FILE *arquivo = fopen("contasRegistradas.txt", "r");//abre o arquivo e le ele, armazenando em uma variavel
    int numero_lido;
    char linha[500];

    if (arquivo != NULL) {
        //Leitura do arquivo linha por linha
        while(fgets(linha, sizeof(linha), arquivo) != NULL){
            // Percorre o arquivo para encontrar o maior número de conta registrado
            if (sscanf(linha, "Número da conta:%d\n", &numero_lido) == 1) {
                //Operador ternario irá comparar os valores, o maior será atribuido ao numero_atual
                numero_atual = (numero_lido > numero_atual) ? numero_lido : numero_atual;
            }
        }
        fclose(arquivo);
    } else {
        printf("Erro ao abrir o arquivo");
    }

}

int confirmar_senha(char senha[], char senha_confirmar[]){
    //strcamp verifica se senha e senha_confirmada são iguais (== 0) ou diferentes (!= 0)
    return (strcmp(senha, senha_confirmar) != 0) ? 0 : 1; //após verificar, o operador ternário retorna 0 ou 1
}

void salvar_conta(ContaBancaria *conta) {
    FILE *arquivo = fopen("contasRegistradas.txt", "a");//abre o arquivo e insere as informações no final do arquivo

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo\n");
        return;
    } else {
        //escreve as informações da conta no arquivo na ordem:
        fprintf(arquivo, "Número da conta:%d\n", conta->conta_numero);
        fprintf(arquivo, "Nome:%s\n", conta->titular);
        fprintf(arquivo, "Senha:%s\n", conta->senha);
        fprintf(arquivo, "Saldo:%.2f\n", conta->saldo);
        fprintf(arquivo, "-----------------------------\n");

        fclose(arquivo);
    }
}

void login(int numero_conta, char senha_conta[]){
    printf("---REALIZAR LOGIN---\n");
    printf("Número da conta: ");
    scanf("%d", &numero_conta);
    if(validar_numero(numero_conta)){//sí valida a senha caso o número seja validado
        printf("Digite a senha: ");
        scanf("%s", senha_conta);
        //operador ternário para validar a senha, se validar vai chamar o menu de operações basicas, senão da erro
        (validar_senha(numero_conta, senha_conta)) ? menu_operacoes_basicas(numero_conta) : printf("Senha incorreta!");
    }
}

void menu_operacoes_basicas(int numero_conta){
    int opcao;
    printf("---BEM VINDO!---\n");
    do{
        //Mostra as opções para o usuário e permite ele escolher
        printf("Escolha uma das opcoes a baixo: \n");
        printf("1- Deposito\n");
        printf("2- Saque\n");
        printf("3- Transferencia entre Contas\n");
        printf("4- Consultar Saldo\n");
        printf("5- Sair\n");
        scanf("%d", &opcao);
        //valida a opção escolhida
        if(opcao < 1 || opcao > 6){
            printf("OPCAO INVALIDA! TENTE NOVAMENTE.\n");
        } else {
            realizar_operacao(opcao, numero_conta);

        }
    } while(opcao < 1 || opcao > 6);//mostra o menu enquanto a opção não for válida
}

void realizar_operacao(int opcao, int numero_conta){
    switch (opcao){
        case 1 ://cada função recebe o parametro numero_conta que foi registrado no login
            depositar(numero_conta);
            break;
        case 2:
            sacar(numero_conta);
            break;
        case 3:
            transferir(numero_conta);
            break;
        case 4:
            consultar_saldo(numero_conta);
            break;
        case 5:
            printf("Saindo...");
            break;

    default:
        printf("Opcao invalida!");//Caso o número digitado não esteja entre as opções
        break;
    }    
}

void depositar(int numero_conta){
    float valor_deposito;
    //loop para lidar com valores negativos digitados
    do{
        printf("Digite o valor que deseja depositar: ");
        scanf("%f", &valor_deposito);
        //operador ternário para checar se o valor é positivo
        (valor_deposito > 0) ? depositar_saldo(numero_conta, valor_deposito) : printf("ERRO: Valor inválido\n");
    }while(valor_deposito < 0);

    printf("Depósito de R$%.2f realizado com sucesso\n", valor_deposito);
}

int validar_numero(int numero){
    FILE *arquivo = fopen("contasRegistradas.txt", "r");
    char linha[100];  // Buffer para armazenar uma linha do arquivo
    int numero_lido;

    if(arquivo != NULL){
        int numero_encontrado = 0;

        while(fgets(linha, sizeof(linha), arquivo) != NULL){
            // Usa sscanf para extrair o número da linha lida
            if(sscanf(linha, "Número da conta:%d", &numero_lido) == 1){
                if(numero_lido == numero){
                    numero_encontrado = 1;
                    break;
                }
            }
        }
        fclose(arquivo);
        //Operador ternário que verifica se o numero foi encontrado
        return (numero_encontrado) ? 1 : (printf("ERRO: Nenhuma de número %d cadastrada!", numero), 0);

    } else{
        printf("Erro ao abrir o arquivo!\n");
        return -1;
    }
}

int validar_senha(int numero_conta, char senha_conta[]) {
    FILE *arquivo = fopen("contasRegistradas.txt", "r");//abre e le o arquivo
    int numero_lido;
    char senha_lida[20], linha[500], titular[50];

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return 0; // Retorna 0 indicando erro na abertura do arquivo
    }
    // Percorre o arquivo linha por linha
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        // Usa sscanf para extrair o número da conta da linha
        if (sscanf(linha, "Número da conta:%d", &numero_lido) == 1) {
            // Verifica se o número da conta corresponde ao número fornecido
            if (numero_lido == numero_conta) {
                fgets(titular, sizeof(titular), arquivo);  // Lê o nome do titular
                fgets(senha_lida, sizeof(senha_lida), arquivo);  // Lê a senha registrada

                // Remove "Senha:" do início e '\n' do final da senha
                sscanf(senha_lida, "Senha:%s", senha_lida);
                fclose(arquivo);
                // Verifica se a senha digitada corresponde à senha registrada
                return (strcmp(senha_lida, senha_conta) == 0) ? 1 : 0;
            }
        }
    }

    fclose(arquivo);
    return 0; // Conta não encontrada ou senha inválida
}

int depositar_saldo(int numero_conta, float valor) {
    FILE *arquivo = fopen("contasRegistradas.txt", "r+");//abre e escreve no arquivo
    int numero_lido;
    float saldo_lido;
    char linha[500], titular[50], senha[20];
    long posicao_saldo;

    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo!\n");
        return -1;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (sscanf(linha, "Número da conta:%d", &numero_lido) == 1) {
            fgets(titular, sizeof(titular), arquivo);
            fgets(senha, sizeof(senha), arquivo);
            posicao_saldo = ftell(arquivo); // Guarda a posição do saldo
            
            if (posicao_saldo == -1L) { // Verifica se a posição é válida
                printf("Erro ao obter posição do saldo!\n");
                fclose(arquivo);
                return -1;
            }

            fscanf(arquivo, "Saldo:%f\n", &saldo_lido); // Lê o saldo

            if (numero_lido == numero_conta) {
                // Posiciona o cursor na linha do saldo
                if (fseek(arquivo, posicao_saldo, SEEK_SET) != 0) {
                    printf("Erro ao posicionar o cursor no saldo!\n");
                    fclose(arquivo);
                    return -1;
                }

                // Limpa a linha do saldo e escreve o novo valor sem adicionar múltiplas quebras de linha
                fprintf(arquivo, "Saldo:      \n"); // Espaços para limpar a linha
                fseek(arquivo, posicao_saldo, SEEK_SET); // Volta para a posição do saldo
                fprintf(arquivo, "Saldo:%.2f", saldo_lido + valor); // Sem \n para evitar quebra de linha extra
                
                fclose(arquivo);
                return 1; // Sucesso
            }
        }
    }
    printf("ERRO: Conta não encontrada!\n");
    fclose(arquivo);
    return 0; // Falha por conta não encontrada
}

void menu_de_saque(int numero_conta, char senha_conta[]){
    //Coletando os dados da conta em que o usuario deseja realizar o saque
    printf("---REALIZAR SAQUE---\n");
    printf("Numero da conta: ");
    scanf("%d", &numero_conta);
    //validando a senha
    if(validar_numero(numero_conta)){
        printf("Digite a senha: ");
        scanf("%s", senha_conta);
        //Validando se a senha está correta através de um valor ternário compactado
        (validar_senha(numero_conta, senha_conta)) ? sacar(numero_conta) : printf("Senha incorreta!");
    }
}

void sacar(int numero_conta) {
    float valor;
    do{
        printf("Digite o valor do saque: ");
        scanf("%f", &valor);
        if(valor < 0){
            printf("ERRO: valor inválido!\n");
        }
    }while(valor < 0);
    //checa se o usuario possui o saldo na conta suficiente para sacar (checar_saldo) e então realiza o saque (retirar_saldo)
    if(checar_saldo(numero_conta, valor)){
        retirar_saldo(numero_conta, valor);
    }else {
        printf("ERRO: SALDO INSUFICIENTE!");//erro em caso de saldo insuficiente
    }

}

int retirar_saldo(int numero_conta, float valor) {
    FILE *arquivo = fopen("contasRegistradas.txt", "r");  // Abre o arquivo para leitura
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo!\n");
        return -1;
    }

    char linha[500];
    char novo_conteudo[5000] = "";  // Para armazenar o conteúdo modificado do arquivo
    int numero_lido;
    float saldo_lido;
    char titular[50], senha[20];
    int conta_encontrada = 0;  // Flag para marcar se a conta foi encontrada

    // Percorre o arquivo linha por linha
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        strcat(novo_conteudo, linha);  // Copia a linha atual para o novo conteúdo
        if (sscanf(linha, "Número da conta:%d", &numero_lido) == 1) {
            // Lê as próximas linhas (titular, senha, saldo)
            fgets(titular, sizeof(titular), arquivo);
            strcat(novo_conteudo, titular);  // Copia o nome do titular para o novo conteúdo
            fgets(senha, sizeof(senha), arquivo);
            strcat(novo_conteudo, senha);    // Copia a senha para o novo conteúdo
            fgets(linha, sizeof(linha), arquivo);  // Lê a linha do saldo

            if (sscanf(linha, "Saldo:%f", &saldo_lido) == 1 && numero_lido == numero_conta) {
                if (saldo_lido >= valor) {
                    // Atualiza o saldo
                    saldo_lido -= valor;
                    sprintf(linha, "Saldo:%.2f\n", saldo_lido);
                    conta_encontrada = 1;  // Marca que a conta foi encontrada e atualizada
                } else {
                    printf("Saldo insuficiente!\n");
                    fclose(arquivo);
                    return 0;  // Falha por saldo insuficiente
                }
            }
            strcat(novo_conteudo, linha);  // Copia o saldo atualizado ou o original
        }
    }

    fclose(arquivo);  // Fecha o arquivo de leitura

    if (!conta_encontrada) {
        printf("ERRO: Conta não encontrada!\n");
        return 0;  // Falha por conta não encontrada
    }

    // Agora reabre o arquivo em modo de escrita para sobrescrever o conteúdo
    arquivo = fopen("contasRegistradas.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo para escrita!\n");
        return -1;
    }

    // Reescreve o conteúdo modificado de volta no arquivo
    fputs(novo_conteudo, arquivo);
    fclose(arquivo);  // Fecha o arquivo de escrita

    printf("Saque de R$%.2f realizado com sucesso\n", valor);
    return 1;  // Sucesso
}

int checar_saldo(int numero_conta, float valor){
    FILE *arquivo = fopen("contasRegistradas.txt", "r"); //abre o arquivo e le
    int numero_lido;//armazena numero lido pela função
    char senha_lida[20], linha[500], titular[50];//dados a serem lidos
    float saldo_lido;//variavel que armazena o saldo lido

    //le linha por linha
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        // Usa sscanf para extrair o número da conta da linha
        if (sscanf(linha, "Número da conta:%d", &numero_lido) == 1) {
            // Verifica se o número da conta corresponde ao número fornecido
            if (numero_lido == numero_conta) {
                fgets(titular, sizeof(titular), arquivo);  // Le o nome do titular
                fgets(senha_lida, sizeof(senha_lida), arquivo);// Le a senha registrada
                fgets(linha, sizeof(linha), arquivo);//le a linha que conterá o saldo
                // Le o valor que está após o prefíxo "Saldo:"
                if(sscanf(linha, "Saldo:%f", &saldo_lido) == 1){
                    // Retorna 1 se o saldo lido for o suficiente ou 0 senao
                    return saldo_lido >= valor ? 1 : 0;
                }    
            }
        }
    }
    fclose(arquivo);
    return -1;
}

void transferir(int numero_conta) {
    int conta_destino;
    float valor_transferencia;

    printf("Número da conta de destino: ");
    scanf("%d", &conta_destino);

    if(!validar_numero(conta_destino)){
        return;
    }

    do{
        printf("Digite o valor da transferência: ");
        scanf("%f", &valor_transferencia);
        if(valor_transferencia < 0){
            printf("ERRO: valor inválido\n");
        }
    }while(valor_transferencia < 0);

    if (checar_saldo(numero_conta, valor_transferencia)) {
        if (retirar_saldo(numero_conta, valor_transferencia)) {
            depositar_saldo(conta_destino, valor_transferencia);
            registrar_transacao(numero_conta, conta_destino, valor_transferencia);
            printf("Transferência realizada com sucesso!\n");
        }   
        
    }else{
        printf("ERRO: Saldo indiponível!");
    }    
}

void registrar_transacao(int conta_origem, int conta_destino, float valor) {
    FILE *arquivo = fopen("transacoes.txt", "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de transações!\n");
        return;
    }
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(arquivo, "----------\n");
    fprintf(arquivo, "Conta origem: %d\n", conta_origem);
    fprintf(arquivo, "Conta destino: %d\n", conta_destino);
    fprintf(arquivo, "Valor transferido: %.2f\n", valor);
    fprintf(arquivo, "Data: %02d-%02d-%04d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    fprintf(arquivo, "----------\n\n");

    fclose(arquivo);
}

void consultar_saldo(int numero_conta) {
    FILE *arquivo = fopen("contasRegistradas.txt", "r");
    int numero_lido;
    float saldo_lido;
    char linha[100];

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (sscanf(linha, "Número da conta:%d", &numero_lido) == 1) {
            fgets(linha, sizeof(linha), arquivo);
            fgets(linha, sizeof(linha), arquivo); 
            fscanf(arquivo, "Saldo:%f\n", &saldo_lido);

            if (numero_lido == numero_conta) {
                printf("Saldo disponível: %.2f\n", saldo_lido);
                break;
            }
        }
    }
    fclose(arquivo);
}