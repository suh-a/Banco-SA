#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// Estrutura para armazenar informações do cliente
typedef struct Cliente {
    char nome[100];
    char cpf[15];
    char data_nascimento[12];
    char telefone[15];
    char endereco[100];
    char senha[10];
    char numero_conta[15];
    double saldo;
    struct Cliente *proximo;
} Cliente;

// Estrutura para senhas de atendimento
typedef struct SenhaAtendimento {
    char codigo_servico[3];
    int numero_senha;
    int preferencial; // 1 para preferencial, 0 para normal
    struct SenhaAtendimento *proximo;
} SenhaAtendimento;

// Ponteiros globais para lista de clientes e fila de senhas
Cliente *clientes_ativos = NULL;
Cliente *ex_clientes = NULL;
SenhaAtendimento *fila_senhas = NULL;

// Protótipos das funções
void menu_principal();
void gerar_senha_atendimento();
void iniciar_atendimento();
void gestao();
void salvar_clientes_em_arquivo();
void carregar_clientes_do_arquivo();
void salvar_senhas_em_arquivo();
void carregar_senhas_do_arquivo();
void enfileirar_senha(SenhaAtendimento *nova_senha);
SenhaAtendimento* desenfileirar_senha(char *codigo_servico);
void abertura_conta();
void deposito();
void transferencia();
void saldo_conta();
void encerramento_conta();
int validar_cpf(char *cpf);
int calcular_idade(char *data_nascimento);
Cliente* buscar_cliente_por_cpf(char *cpf);
void salvar_comprovante(char *conteudo, char *nome_arquivo);
void listar_clientes_ativos();
void listar_ex_clientes();

int main() {
    setlocale(LC_ALL, "Portuguese");

    carregar_clientes_do_arquivo();
    carregar_senhas_do_arquivo();
    menu_principal();
    salvar_clientes_em_arquivo();
    salvar_senhas_em_arquivo();
    return 0;
}

// Função Menu Principal
void menu_principal() {
    int opcao;

    printf("Seja bem-vindo ao S.A BANK\n\n");
    do {
        printf("== Menu de opções == \n");
        printf("1. Gerar senha de atendimento\n");
        printf("2. Iniciar atendimento\n");
        printf("3. Gestão\n");
        printf("4. Sair\n");
        printf("Opção: ");
        scanf("%d", &opcao);
        getchar();
        system("cls");

        switch(opcao) {
            case 1:
                gerar_senha_atendimento();
                break;
            case 2:
                iniciar_atendimento();
                break;
            case 3:
                gestao();
                break;
            case 4:
                printf("Saindo do sistema...\n");
                break;
            default:
                printf("\n");
                printf("Opção inválida! Tente novamente.\n");
        }
    } while(opcao != 4);
}

// Função Gerar Senha de Atendimento
void gerar_senha_atendimento() {
    int opcao_servico;
    char codigo_servico[3];
    SenhaAtendimento *nova_senha = (SenhaAtendimento *)malloc(sizeof(SenhaAtendimento));

    printf("Selecione o serviço:\n");
    printf("1. Abertura de conta (AB)\n");
    printf("2. Depósito (DP)\n");
    printf("3. Transferência (TF)\n");
    printf("4. Saldo em conta (SD)\n");
    printf("5. Encerramento de conta (EC)\n");
    printf("Opção: ");
    scanf("%d", &opcao_servico);
    getchar();
    system("cls");

    switch(opcao_servico) {
        case 1:
            strcpy(codigo_servico, "AB");
            break;
        case 2:
            strcpy(codigo_servico, "DP");
            break;
        case 3:
            strcpy(codigo_servico, "TF");
            break;
        case 4:
            strcpy(codigo_servico, "SD");
            break;
        case 5:
            strcpy(codigo_servico, "EC");
            break;
        default:
            printf("\n");
            printf("Opção inválida!\n");
            free(nova_senha);
            return;
    }

    strcpy(nova_senha->codigo_servico, codigo_servico);
    nova_senha->numero_senha = rand() % 1000;

    printf("A senha é preferencial? (1 - Sim, 0 - Não): ");
    scanf("%d", &nova_senha->preferencial);
    getchar();

    if(nova_senha->preferencial < 0 || nova_senha->preferencial > 1){
        printf("\n");
        printf("Opção inválida! \n");
        printf("\n");
        free(nova_senha);
        return;
    }

    system("cls");

    enfileirar_senha(nova_senha);
    printf("Sua senha é: %s%03d\n", nova_senha->codigo_servico, nova_senha->numero_senha);
    printf("\n\n");

    int continuar_fila;
    printf("Deseja continuar na fila? (1 - Sim, 0 - Não): ");
    scanf("%d", &continuar_fila);
    getchar();

    if(continuar_fila < 0 || continuar_fila > 1){
        printf("\n");
        printf("Opção inválida! \n");
        printf("\n");
        return;
    }
    printf("\n");

    if(!continuar_fila) {
        // Remover a senha da fila
        free(nova_senha);
        printf("\n");
        printf("Senha excluída.\n");
        printf("\n");
    } else {
        printf("\n");
        printf("Você permanecerá na fila.\n");
        printf("\n");
    }

    // Salvar as senhas em arquivo
    salvar_senhas_em_arquivo();
}

// Função Enfileirar Senha
void enfileirar_senha(SenhaAtendimento *nova_senha) {
    if(fila_senhas == NULL) {
        fila_senhas = nova_senha;
        nova_senha->proximo = NULL;
    } else {
        SenhaAtendimento *atual = fila_senhas;
        SenhaAtendimento *anterior = NULL;

        // Senhas preferenciais são colocadas à frente das não preferenciais
        while(atual != NULL && (!nova_senha->preferencial || atual->preferencial)) {
            anterior = atual;
            atual = atual->proximo;
        }

        if(anterior == NULL) {
            nova_senha->proximo = fila_senhas;
            fila_senhas = nova_senha;
        } else {
            nova_senha->proximo = atual;
            anterior->proximo = nova_senha;
        }
    }
}

// Função Iniciar Atendimento
void iniciar_atendimento() {
    int opcao_guiche;

    printf("Selecione o guichê de atendimento:\n");
    printf("1. Guichê 1 - AB\n");
    printf("2. Guichê 2 - DP\n");
    printf("3. Guichê 3 - TF\n");
    printf("4. Guichê 4 - SD\n");
    printf("5. Guichê 5 - EC\n");
    printf("Opção: ");
    scanf("%d", &opcao_guiche);
    getchar(); 
    system("cls");

    char *codigo_servico;
    switch(opcao_guiche) {
        case 1:
            codigo_servico = "AB";
            break;
        case 2:
            codigo_servico = "DP";
            break;
        case 3:
            codigo_servico = "TF";
            break;
        case 4:
            codigo_servico = "SD";
            break;
        case 5:
            codigo_servico = "EC";
            break;
        default:
            printf("\n");
            printf("Opção inválida!\n");
            return;
    }

    SenhaAtendimento *senha_atual = desenfileirar_senha(codigo_servico);
    if(senha_atual == NULL) {
        printf("\n");
        printf("Não há senhas para este serviço no momento.\n");
        printf("\n");
        return;
    }

    // Chama a função de serviço correspondente
    if(strcmp(codigo_servico, "AB") == 0) {
        abertura_conta();
    } else if(strcmp(codigo_servico, "DP") == 0) {
        deposito();
    } else if(strcmp(codigo_servico, "TF") == 0) {
        transferencia();
    } else if(strcmp(codigo_servico, "SD") == 0) {
        saldo_conta();
    } else if(strcmp(codigo_servico, "EC") == 0) {
        encerramento_conta();
    }

    // Libera a memória da senha após o atendimento
    free(senha_atual);
}

// Função Desenfileirar Senha
SenhaAtendimento* desenfileirar_senha(char *codigo_servico) {
    SenhaAtendimento *atual = fila_senhas;
    SenhaAtendimento *anterior = NULL;

    while(atual != NULL) {
        if(strcmp(atual->codigo_servico, codigo_servico) == 0) {
            if(anterior == NULL) {
                fila_senhas = atual->proximo;
            } else {
                anterior->proximo = atual->proximo;
            }
            atual->proximo = NULL;
            return atual;
        }
        anterior = atual;
        atual = atual->proximo;
    }
    return NULL;
}

void abertura_conta() {
    Cliente *novo_cliente = (Cliente *)malloc(sizeof(Cliente));
    char tipo_conta[20];
    int idade;

    printf("Conta corrente ou poupança? ");
    fgets(tipo_conta, sizeof(tipo_conta), stdin);
    tipo_conta[strcspn(tipo_conta, "\n")] = 0; 

    printf("Nome completo: ");
    fgets(novo_cliente->nome, sizeof(novo_cliente->nome), stdin);
    novo_cliente->nome[strcspn(novo_cliente->nome, "\n")] = 0;

    printf("CPF: ");
    fgets(novo_cliente->cpf, sizeof(novo_cliente->cpf), stdin);
    novo_cliente->cpf[strcspn(novo_cliente->cpf, "\n")] = 0;

    if (!validar_cpf(novo_cliente->cpf)) {
        printf("CPF inválido!\n");
        free(novo_cliente);
        return;
    }

    printf("Data de nascimento (DD/MM/AAAA): ");
    fgets(novo_cliente->data_nascimento, sizeof(novo_cliente->data_nascimento), stdin);
    novo_cliente->data_nascimento[strcspn(novo_cliente->data_nascimento, "\n")] = 0;

    // Validar o formato da data e verificar o mês
    int dia, mes, ano;
    if (sscanf(novo_cliente->data_nascimento, "%d/%d/%d", &dia, &mes, &ano) != 3 || mes < 1 || mes > 12) {
        printf("\n");
        printf("Data de nascimento inválida! Certifique-se de usar o formato DD/MM/AAAA e um mês entre 1 e 12.\n");
        free(novo_cliente);
        return;
    }

    idade = calcular_idade(novo_cliente->data_nascimento);
    if (idade < 18) {
        printf("\n");
        printf("Você precisa ter 18 anos ou mais para abrir uma conta.\n");
        free(novo_cliente);
        return;
    }

    printf("Número de telefone: ");
    fgets(novo_cliente->telefone, sizeof(novo_cliente->telefone), stdin);
    novo_cliente->telefone[strcspn(novo_cliente->telefone, "\n")] = 0;

    printf("Endereço: ");
    fgets(novo_cliente->endereco, sizeof(novo_cliente->endereco), stdin);
    novo_cliente->endereco[strcspn(novo_cliente->endereco, "\n")] = 0;

    char senha_confirmacao[10];
    do {
        printf("Crie uma senha de acesso (4 dígitos): ");
        fgets(novo_cliente->senha, sizeof(novo_cliente->senha), stdin);
        novo_cliente->senha[strcspn(novo_cliente->senha, "\n")] = 0;

        if (strlen(novo_cliente->senha) != 4) {
            printf("\n");
            printf("A senha deve ter exatamente 4 dígitos.\n");
            printf("\n");
            continue;
        }

        printf("Confirme sua senha: ");
        fgets(senha_confirmacao, sizeof(senha_confirmacao), stdin);
        senha_confirmacao[strcspn(senha_confirmacao, "\n")] = 0;

        if (strcmp(novo_cliente->senha, senha_confirmacao) != 0) {
            printf("\n");
            printf("As senhas não coincidem. Tente novamente.\n");
            printf("\n");
        } else {
            break;
        }
    } while (1);

    // Gera o número da conta
    sprintf(novo_cliente->numero_conta, "%07d-%d", rand() % 10000000, rand() % 10);
    novo_cliente->saldo = 0.0;
    novo_cliente->proximo = clientes_ativos;
    clientes_ativos = novo_cliente;

    // Salvar o cliente em arquivo
    salvar_clientes_em_arquivo();

    // Gerar e exibir comprovante
    char comprovante[500];
    time_t agora = time(NULL);
    struct tm *t = localtime(&agora);

    snprintf(comprovante, sizeof(comprovante),
             "== Comprovante de Abertura ==\nTitular: %s\nN° da conta: %s\nSaldo: %.2f R$\n\nData: %02d/%02d/%02d     Hora: %02d:%02d\n",
             novo_cliente->nome, novo_cliente->numero_conta, novo_cliente->saldo,
             t->tm_mday, t->tm_mon + 1, t->tm_year % 100, t->tm_hour, t->tm_min);

    system("cls");
    printf("\n%s\n", comprovante);

    // Salvar comprovante em arquivo
    salvar_comprovante(comprovante, "comprovante_abertura.txt");

    printf("Atendimento finalizado.\n\n");
}

// Função Validar CPF
int validar_cpf(char *cpf) {
    if(strlen(cpf) != 11) {
        return 0;
    }
   
    return 1;
}

// Função Calcular Idade
int calcular_idade(char *data_nascimento) {
    int dia, mes, ano;
    sscanf(data_nascimento, "%d/%d/%d", &dia, &mes, &ano);

    time_t agora = time(NULL);
    struct tm *t = localtime(&agora);

    int idade = t->tm_year + 1900 - ano;
    if(t->tm_mon + 1 < mes || (t->tm_mon + 1 == mes && t->tm_mday < dia)) {
        idade--;
    }
    return idade;
}

// Função Buscar Cliente por CPF
Cliente* buscar_cliente_por_cpf(char *cpf) {
    Cliente *atual = clientes_ativos;
    while(atual != NULL) {
        if(strcmp(atual->cpf, cpf) == 0) {
            return atual;
        }
        atual = atual->proximo;
    }
    return NULL;
}

// Função Salvar Clientes em Arquivo 
void salvar_clientes_em_arquivo() {
    FILE *arquivo = fopen("clientes.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }

    Cliente *atual = clientes_ativos;
    while (atual != NULL) {
        fprintf(arquivo, "nome: %s\n", atual->nome);
        fprintf(arquivo, "cpf: %s\n", atual->cpf);
        fprintf(arquivo, "data_nascimento: %s\n", atual->data_nascimento);
        fprintf(arquivo, "telefone: %s\n", atual->telefone);
        fprintf(arquivo, "endereco: %s\n", atual->endereco);
        fprintf(arquivo, "senha: %s\n", atual->senha);
        fprintf(arquivo, "numero_conta: %s\n", atual->numero_conta);
        fprintf(arquivo, "saldo: %.2f\n", atual->saldo);
        fprintf(arquivo, "---\n");
        atual = atual->proximo;
    }
    fclose(arquivo);
}

// Função Carregar Clientes do Arquivo 
void carregar_clientes_do_arquivo() {
    FILE *arquivo = fopen("clientes.txt", "r");
    if (arquivo == NULL) {
        return;
    }

    char linha[256];
    Cliente *novo_cliente = NULL;
    while (fgets(linha, sizeof(linha), arquivo)) {
        linha[strcspn(linha, "\n")] = 0; 

        if (strncmp(linha, "nome: ", 6) == 0) {
            novo_cliente = (Cliente *)malloc(sizeof(Cliente));
            strcpy(novo_cliente->nome, linha + 6);
        } else if (strncmp(linha, "cpf: ", 5) == 0) {
            strcpy(novo_cliente->cpf, linha + 5);
        } else if (strncmp(linha, "data_nascimento: ", 17) == 0) {
            strcpy(novo_cliente->data_nascimento, linha + 17);
        } else if (strncmp(linha, "telefone: ", 10) == 0) {
            strcpy(novo_cliente->telefone, linha + 10);
        } else if (strncmp(linha, "endereco: ", 10) == 0) {
            strcpy(novo_cliente->endereco, linha + 10);
        } else if (strncmp(linha, "senha: ", 7) == 0) {
            strcpy(novo_cliente->senha, linha + 7);
        } else if (strncmp(linha, "numero_conta: ", 14) == 0) {
            strcpy(novo_cliente->numero_conta, linha + 14);
        } else if (strncmp(linha, "saldo: ", 7) == 0) {
            novo_cliente->saldo = atof(linha + 7);
        } else if (strcmp(linha, "---") == 0) {
            novo_cliente->proximo = clientes_ativos;
            clientes_ativos = novo_cliente;
            novo_cliente = NULL;
        }
    }
    fclose(arquivo);
}

// Função Salvar Senhas em Arquivo 
void salvar_senhas_em_arquivo() {
    FILE *arquivo = fopen("senhas.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }

    SenhaAtendimento *atual = fila_senhas;
    while (atual != NULL) {
        fprintf(arquivo, "codigo_servico: %s\n", atual->codigo_servico);
        fprintf(arquivo, "numero_senha: %d\n", atual->numero_senha);
        fprintf(arquivo, "preferencial: %d\n", atual->preferencial);
        fprintf(arquivo, "---\n");
        atual = atual->proximo;
    }
    fclose(arquivo);
}

// Função Carregar Senhas do Arquivo 
void carregar_senhas_do_arquivo() {
    FILE *arquivo = fopen("senhas.txt", "r");
    if (arquivo == NULL) {
        return;
    }

    char linha[128];
    SenhaAtendimento *nova_senha = NULL;
    while (fgets(linha, sizeof(linha), arquivo)) {
        linha[strcspn(linha, "\n")] = 0; 

        if (strncmp(linha, "codigo_servico: ", 16) == 0) {
            nova_senha = (SenhaAtendimento *)malloc(sizeof(SenhaAtendimento));
            strcpy(nova_senha->codigo_servico, linha + 16);
        } else if (strncmp(linha, "numero_senha: ", 14) == 0) {
            nova_senha->numero_senha = atoi(linha + 14);
        } else if (strncmp(linha, "preferencial: ", 14) == 0) {
            nova_senha->preferencial = atoi(linha + 14);
        } else if (strcmp(linha, "---") == 0) {
            nova_senha->proximo = NULL;
            enfileirar_senha(nova_senha);
            nova_senha = NULL;
        }
    }
    fclose(arquivo);
}

// Função Salvar Comprovante
void salvar_comprovante(char *conteudo, char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "a");
    fprintf(arquivo, "%s\n", conteudo);
    fclose(arquivo);
}

// Função Depósito
void deposito() {
    char cpf[15];
    char senha[10];
    double valor;

    printf("CPF: ");
    fgets(cpf, sizeof(cpf), stdin);
    cpf[strcspn(cpf, "\n")] = 0;

    printf("Senha: ");
    fgets(senha, sizeof(senha), stdin);
    senha[strcspn(senha, "\n")] = 0;

    Cliente *cliente = buscar_cliente_por_cpf(cpf);
    if(cliente == NULL || strcmp(cliente->senha, senha) != 0) {
        printf("CPF ou senha incorretos!\n");
        return;
    }

    printf("Valor do depósito: ");
    scanf("%lf", &valor);
    getchar(); 

    cliente->saldo += valor;

    salvar_clientes_em_arquivo();

    // Gerar e exibir comprovante
    char comprovante[500];
    time_t agora = time(NULL);
    struct tm *t = localtime(&agora);

    snprintf(comprovante, sizeof(comprovante),
        "== Comprovante de Depósito ==\nTitular: %s\nN° da conta: %s\nDepósito: %.2f R$\n\nData: %02d/%02d/%02d     Hora: %02d:%02d\n",
        cliente->nome, cliente->numero_conta, valor,
        t->tm_mday, t->tm_mon + 1, t->tm_year % 100, t->tm_hour, t->tm_min);

    printf("\n");
    printf("%s\n", comprovante);

    // Salvar comprovante em arquivo
    salvar_comprovante(comprovante, "comprovante_deposito.txt");

    printf("\n\n");
    printf("Saldo atual: %.2f R$\n", cliente->saldo);
    printf("\n");
    printf("Atendimento finalizado.\n");
    printf("\n");
}

void transferencia() {
    char cpf[15];
    char senha[10];
    char conta_origem[15];
    char conta_destino[15];
    double valor;

    // Solicitar o CPF
    printf("CPF: ");
    fgets(cpf, sizeof(cpf), stdin);
    cpf[strcspn(cpf, "\n")] = 0; 

    // Solicitar a senha
    printf("Senha: ");
    fgets(senha, sizeof(senha), stdin);
    senha[strcspn(senha, "\n")] = 0;

    // Buscar cliente pelo CPF
    Cliente *cliente_origem = buscar_cliente_por_cpf(cpf);
    if(cliente_origem == NULL) {
        printf("\n");
        printf("CPF não encontrado!\n");
        return;
    }

    // Comparar a senha
    if(strcmp(cliente_origem->senha, senha) != 0) {
        printf("\n");
        printf("Senha incorreta!\n");
        return;
    }

    // Solicitar a conta de origem
    printf("Conta de origem: ");
    fgets(conta_origem, sizeof(conta_origem), stdin);
    conta_origem[strcspn(conta_origem, "\n")] = 0; 

    // Verificar se a conta de origem corresponde ao CPF
    if(strcmp(cliente_origem->numero_conta, conta_origem) != 0) {
        printf("\n");
        printf("Conta de origem não corresponde ao CPF.\n");
        return;
    }

    // Solicitar a conta de destino
    printf("Conta de destino: ");
    fgets(conta_destino, sizeof(conta_destino), stdin);
    conta_destino[strcspn(conta_destino, "\n")] = 0; 

    // Buscar cliente de destino
    Cliente *cliente_destino = clientes_ativos;
    while(cliente_destino != NULL && strcmp(cliente_destino->numero_conta, conta_destino) != 0) {
        cliente_destino = cliente_destino->proximo;
    }

    if(cliente_destino == NULL) {
        printf("\n");
        printf("Conta de destino não existe.\n");
        return;
    }

    // Solicitar o valor da transferência
    printf("Valor da transferência: ");
    scanf("%lf", &valor);
    getchar(); 

    // Verificar se o saldo é suficiente
    if(cliente_origem->saldo < valor) {
        printf("\n");
        printf("Saldo insuficiente!\n");
        return;
    }

    // Realizar a transferência
    cliente_origem->saldo -= valor;
    cliente_destino->saldo += valor;

    salvar_clientes_em_arquivo();

    // Gerar e exibir o comprovante
    char comprovante[500];
    time_t agora = time(NULL);
    struct tm *t = localtime(&agora);

    snprintf(comprovante, sizeof(comprovante),
        "== Comprovante de Transferência ==\nConta de origem: %s\nConta de destino: %s\nValor transferido: %.2f R$\nData: %02d/%02d/%02d\nHora: %02d:%02d\n",
        conta_origem, conta_destino, valor,
        t->tm_mday, t->tm_mon + 1, t->tm_year % 100, t->tm_hour, t->tm_min);

    printf("\n%s\n", comprovante);
    printf("\n\n");

    // Salvar o comprovante em arquivo
    salvar_comprovante(comprovante, "comprovante_transferencia.txt");

    // Exibir saldo atual
    printf("Saldo atual: %.2f R$\n", cliente_origem->saldo);
    printf("\n\n");
    printf("Atendimento finalizado.\n");
    printf("\n");
}

// Função Saldo em Conta
void saldo_conta() {
    char cpf[15];
    char senha[10];

    printf("CPF: ");
    fgets(cpf, sizeof(cpf), stdin);
    cpf[strcspn(cpf, "\n")] = 0;

    printf("Senha: ");
    fgets(senha, sizeof(senha), stdin);
    senha[strcspn(senha, "\n")] = 0;

    Cliente *cliente = buscar_cliente_por_cpf(cpf);
    if(cliente == NULL || strcmp(cliente->senha, senha) != 0) {
        printf("\n");
        printf("CPF ou senha incorretos!\n");
        return;
    }

    printf("\n");
    printf("Saldo em conta: %.2f R$\n", cliente->saldo);
    printf("\n\n");
    printf("Atendimento finalizado.\n");
    printf("\n");
}

// Função Encerramento de Conta
void encerramento_conta() {
    char cpf[15];
    char senha[10];
    int confirmacao;

    printf("CPF: ");
    fgets(cpf, sizeof(cpf), stdin);
    cpf[strcspn(cpf, "\n")] = 0;

    printf("Senha: ");
    fgets(senha, sizeof(senha), stdin);
    senha[strcspn(senha, "\n")] = 0;

    Cliente *cliente = buscar_cliente_por_cpf(cpf);
    if(cliente == NULL || strcmp(cliente->senha, senha) != 0) {
        printf("\n");
        printf("CPF ou senha incorretos!\n");
        return;
    }

	printf("\n");
    printf("Tem certeza que deseja encerrar a conta? (1 - Sim, 0 - Não): ");
    scanf("%d", &confirmacao);
    getchar(); 

    if(confirmacao < 0 || confirmacao > 1){
        printf("\n");
        printf("Opção inválida! \n");
        printf("\n");
        return;
    }

    if(confirmacao) {
        // Remover cliente da lista de clientes ativos
        Cliente *atual = clientes_ativos;
        Cliente *anterior = NULL;

        while(atual != NULL && atual != cliente) {
            anterior = atual;
            atual = atual->proximo;
        }

        if(anterior == NULL) {
            clientes_ativos = atual->proximo;
        } else {
            anterior->proximo = atual->proximo;
        }

        // Adicionar cliente à lista de ex-clientes
        if (ex_clientes == NULL) {
            printf("\n");
            printf("Nenhum ex-cliente foi registrado até o momento. Criando lista de ex-clientes.\n");
            printf("\n");
        }

        cliente->proximo = ex_clientes;
        ex_clientes = cliente;

        // Salvar clientes em arquivos
        salvar_clientes_em_arquivo();

        printf("\n");
        printf("Conta encerrada com sucesso.\n");
        printf("\n");
    } else {
        printf("\n");
        printf("Operação cancelada.\n");
        printf("\n");
    }

    printf("\n");
    printf("Atendimento finalizado.\n");
    printf("\n");
}

// Função Gestão
void gestao() {
    char login[15];
    char senha[15];

    printf("Login: ");
    fgets(login, sizeof(login), stdin);
    login[strcspn(login, "\n")] = 0;

    printf("Senha: ");
    fgets(senha, sizeof(senha), stdin);
    senha[strcspn(senha, "\n")] = 0;

    // Credenciais de login predefinidas
    if(strcmp(login, "admin") != 0 || strcmp(senha, "admin123") != 0) {
        printf("\n");
        printf("Login ou senha incorretos!\n");
        printf("\n");
        return;
    }

    int opcao;
    printf("\n");
    printf("Seja bem-vindo gestor! \n\n");
    do {
        printf("== Opções de gestão == \n");
        printf("1. Listar clientes ativos\n");
        printf("2. Listar ex-clientes\n");
        printf("3. Encerrar o programa\n");
        printf("Opção: ");
        scanf("%d", &opcao);
        getchar();
        system("cls");

        switch(opcao) {
            case 1:
                listar_clientes_ativos();
                printf("\n\n");
                break;
            case 2:
                listar_ex_clientes();
                printf("\n\n");
                break;
            case 3:
                printf("Encerrando o programa...\n");
                exit(0);
            default:
                printf("Opção inválida!\n");
        }
    } while(opcao != 3);
}

// Função Listar Clientes Ativos
void listar_clientes_ativos() {
    Cliente *atual = clientes_ativos;
    if (atual == NULL) {
        printf("Nenhum cliente ativo encontrado.\n");
        return;
    }
    printf("Clientes Ativos:\n");
    while (atual != NULL) {
        printf("Nome: %s, CPF: %s, Conta: %s\n",
               atual->nome, atual->cpf, atual->numero_conta);
        atual = atual->proximo;
    }
}

// Função Listar Ex-Clientes
void listar_ex_clientes() {
    Cliente *atual = ex_clientes;
    if (atual == NULL) {
        printf("Nenhum ex-cliente encontrado.\n");
        return;
    }
    printf("Ex-Clientes:\n");
    while (atual != NULL) {
        printf("Nome: %s, CPF: %s, Conta: %s\n",
               atual->nome, atual->cpf, atual->numero_conta);
        atual = atual->proximo;
    }
}



