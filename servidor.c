/****************************************************************************************
 ** ISCTE-IUL: Trabalho prático 2 de Sistemas Operativos 2024/2025, Enunciado Versão 4+
 **
 ** Aluno: Nº: 129837      Nome: Rúben Lopes Fonseca
 ** Nome do Módulo: servidor.c
 ** Descrição/Explicação do Módulo:
 **
 **
 ***************************************************************************************/

// #define SO_HIDE_DEBUG                // Uncomment this line to hide all @DEBUG statements
#include "common.h"

/*** Variáveis Globais ***/
Estacionamento clientRequest;           // Pedido enviado do Cliente para o Servidor
Estacionamento *lugaresEstacionamento;  // Array de Lugares de Estacionamento do parque
int dimensaoMaximaParque;               // Dimensão Máxima do parque (BD), recebida por argumento do programa
int indexClienteBD;                     // Índice do cliente que fez o pedido ao servidor/servidor dedicado na BD
long posicaoLogfile;                    // Posição no ficheiro Logfile para escrever o log da entrada corrente
LogItem logItem;                        // Informação da entrada corrente a escrever no logfile

/**
 * @brief  Processamento do processo Servidor e dos processos Servidor Dedicado
 *         OS ALUNOS NÃO DEVERÃO ALTERAR ESTA FUNÇÃO.
 * @param  argc (I) número de Strings do array argv
 * @param  argv (I) array de lugares de estacionamento que irá servir de BD
 * @return Success (0) or not (<> 0)
 */
int main(int argc, char *argv[]) {
    so_debug("<");

    s1_IniciaServidor(argc, argv);
    s2_MainServidor();

    so_error("Servidor", "O programa nunca deveria ter chegado a este ponto!");
    so_debug(">");
}

/**
 * @brief  s1_iniciaServidor Ler a descrição da tarefa S1 no enunciado.
 *         OS ALUNOS NÃO DEVERÃO ALTERAR ESTA FUNÇÃO.
 * @param  argc (I) número de Strings do array argv
 * @param  argv (I) array de lugares de estacionamento que irá servir de BD
 */
void s1_IniciaServidor(int argc, char *argv[]) {
    so_debug("<");

    s1_1_ObtemDimensaoParque(argc, argv, &dimensaoMaximaParque);
    s1_2_CriaBD(dimensaoMaximaParque, &lugaresEstacionamento);
    s1_3_ArmaSinaisServidor();
    s1_4_CriaFifoServidor(FILE_REQUESTS);

    so_debug(">");
}

/**
 * @brief  s1_1_ObtemDimensaoParque Ler a descrição da tarefa S1.1 no enunciado
 * @param  argc (I) número de Strings do array argv
 * @param  argv (I) array de lugares de estacionamento que irá servir de BD
 * @param  pdimensaoMaximaParque (O) número máximo de lugares do parque, especificado pelo utilizador
 */
void s1_1_ObtemDimensaoParque(int argc, char *argv[], int *pdimensaoMaximaParque) {
    so_debug("< [@param argc:%d, argv:%p]", argc, argv);

    if(argc != 2){
        so_error("S1.1", "Número de argumentos inválido");
        exit(1);
    }
    int i = 0;
    int interptr = 0;
    int count = 0;
    int a = 0;
    while (argv[1][i] != '\0'){
        if(argv[1][i] < '0' || argv[1][i] > '9'){
            interptr = 1;
            break;
        }
        if(argv[1][i] == '0') count++;
        i++;
    }
    a = strlen(argv[1]);
    if(count == a) interptr = 1;
    if(interptr){
        so_error("S1.1", "O Argumento %s não é válido. Tem de ser um número inteiro positivo", argv[1]);
        exit(1);
    }
    *pdimensaoMaximaParque = atoi(argv[1]);
    if(pdimensaoMaximaParque == NULL) so_error("S1.1", "É necessário introduzir a dimensão do parque");
    so_success("S1.1", "Dimensão do Parque: %d\n", *pdimensaoMaximaParque);
    

    so_debug("> [@param +pdimensaoMaximaParque:%d]", *pdimensaoMaximaParque);
}

/**
 * @brief  s1_2_CriaBD Ler a descrição da tarefa S1.2 no enunciado
 * @param  dimensaoMaximaParque (I) número máximo de lugares do parque, especificado pelo utilizador
 * @param  plugaresEstacionamento (O) array de lugares de estacionamento que irá servir de BD
 */
void s1_2_CriaBD(int dimensaoMaximaParque, Estacionamento **plugaresEstacionamento) {
    so_debug("< [@param dimensaoMaximaParque:%d]", dimensaoMaximaParque);

    *plugaresEstacionamento = malloc(dimensaoMaximaParque * sizeof(Estacionamento));
    if(*plugaresEstacionamento == NULL){
        so_error("S1.2", "Não foi possível criar a base de dados");
        exit(1);
    }
    for(int i = 0; i < dimensaoMaximaParque; i++){
        (*plugaresEstacionamento)[i].pidCliente = DISPONIVEL;
    }
    so_success("S1.2", "Base de Dados criada"); 
    

    so_debug("> [*plugaresEstacionamento:%p]", *plugaresEstacionamento);
}

/**
 * @brief  s1_3_ArmaSinaisServidor Ler a descrição da tarefa S1.3 no enunciado
 */
void s1_3_ArmaSinaisServidor() {
    so_debug("<");
    
    if(signal(SIGINT, s3_TrataCtrlC) == SIG_ERR){
        so_error("S1.3", "Erro no tratamento do sinal SIGINT");
        exit(1);
    }
    if(signal(SIGCHLD, s5_TrataTerminouServidorDedicado) == SIG_ERR){
        so_error("S1.3", "Erro no tratamento do sinal SIGCHLD");
        exit(1);
    }
    so_success("S1.3", "Sinais armados com sucesso");
    

    so_debug(">");
}

/**
 * @brief  s1_4_CriaFifoServidor Ler a descrição da tarefa S1.4 no enunciado
 * @param  filenameFifoServidor (I) O nome do FIFO do servidor (i.e., FILE_REQUESTS)
 */
void s1_4_CriaFifoServidor(char *filenameFifoServidor) {
    so_debug("< [@param filenameFifoServidor:%s]", filenameFifoServidor);

    if(remove(filenameFifoServidor) == 0){
        if(mkfifo(filenameFifoServidor, 0666) != 0){
            so_error("S1.4", "Não foi possível criar o FIFO %s", filenameFifoServidor);
            exit(1);
        }
        else{
            so_success("S1.4", "FIFO %s criado com sucesso!", filenameFifoServidor);
        }
    }
    else{
        if(mkfifo(filenameFifoServidor, 0666) != 0){
            so_error("S1.4", "Não foi possível criar o FIFO %s", filenameFifoServidor);
            exit(1);
        }
        else{
            so_success("S1.4", "FIFO %s criado com sucesso!", filenameFifoServidor);
        }
    }
    

    so_debug(">");
}

/**
 * @brief  s2_MainServidor Ler a descrição da tarefa S2 no enunciado.
 *         OS ALUNOS NÃO DEVERÃO ALTERAR ESTA FUNÇÃO, exceto depois de
 *         realizada a função s2_1_AbreFifoServidor(), altura em que podem
 *         comentar o statement sleep abaixo (que, neste momento está aqui
 *         para evitar que os alunos tenham uma espera ativa no seu código)
 */
void s2_MainServidor() {
    so_debug("<");

    FILE *fFifoServidor;
    while (TRUE) { 
        s2_1_AbreFifoServidor(FILE_REQUESTS, &fFifoServidor);
        s2_2_LePedidosFifoServidor(fFifoServidor);
        //sleep(10);  // TEMPORÁRIO, os alunos deverão comentar este statement apenas
                    // depois de terem a certeza que não terão uma espera ativa
    }

    so_debug(">");
}

/**
 * @brief  s2_1_AbreFifoServidor Ler a descrição da tarefa S2.1 no enunciado
 * @param  filenameFifoServidor (I) O nome do FIFO do servidor (i.e., FILE_REQUESTS)
 * @param  pfFifoServidor (O) descritor aberto do ficheiro do FIFO do servidor
 */
void s2_1_AbreFifoServidor(char *filenameFifoServidor, FILE **pfFifoServidor) {
    so_debug("< [@param filenameFifoServidor:%s]", filenameFifoServidor);

    int count = 0;
    while(1){
        if(count == 5){
            so_error("S2.1", "Não existe ficheiro FIFO ou não foi possível abrir");
            s4_EncerraServidor(filenameFifoServidor);
            break;
        }
        *pfFifoServidor = fopen(filenameFifoServidor, "rb");
        if(*pfFifoServidor == NULL){
            count++;
            continue;
        }
        else{
            so_success("S2.1", "Ficheiro FIFO aberto");
            break;
        }
    }

    

    so_debug("> [*pfFifoServidor:%p]", *pfFifoServidor);
}

/**
 * @brief  s2_2_LePedidosFifoServidor Ler a descrição da tarefa S2.2 no enunciado.
 *         OS ALUNOS NÃO DEVERÃO ALTERAR ESTA FUNÇÃO.
 * @param  fFifoServidor (I) descritor aberto do ficheiro do FIFO do servidor
 */
void s2_2_LePedidosFifoServidor(FILE *fFifoServidor) {
    so_debug("<");

    int terminaCiclo2 = FALSE;
    while (TRUE) {
        terminaCiclo2 = s2_2_1_LePedido(fFifoServidor, &clientRequest);
        if (terminaCiclo2)
            break;
        s2_2_2_ProcuraLugarDisponivelBD(clientRequest, lugaresEstacionamento, dimensaoMaximaParque, &indexClienteBD);
        s2_2_3_CriaServidorDedicado(lugaresEstacionamento, indexClienteBD);
    }

    so_debug(">");
}

/**
 * @brief  s2_2_1_LePedido Ler a descrição da tarefa S2.2.1 no enunciado
 * @param  fFifoServidor (I) descritor aberto do ficheiro do FIFO do servidor
 * @param  pclientRequest (O) pedido recebido, enviado por um Cliente
 * @return TRUE se não conseguiu ler um pedido porque o FIFO não tem mais pedidos.
 */
int s2_2_1_LePedido(FILE *fFifoServidor, Estacionamento *pclientRequest) {
    int naoHaMaisPedidos = TRUE;
    so_debug("< [@param fFifoServidor:%p]", fFifoServidor);

    size_t tamElem = sizeof(Estacionamento);
    size_t nrElem = fread(pclientRequest, tamElem, 1, fFifoServidor);
    if(nrElem != 1){
        if(feof(fFifoServidor)){
            so_success("S2.2.1", "Não há mais registos no FIFO");
            naoHaMaisPedidos = TRUE;
        }
        else {
            so_error("S2.2.1", "Erro ao ler pedido do FIFO");
            s4_EncerraServidor(FILE_REQUESTS);
        }
    }    
    else{
        so_success("S2.2.1", "Li Pedido do FIFO");
        naoHaMaisPedidos = FALSE;
    }
    

    so_debug("> [naoHaMaisPedidos:%d, *pclientRequest:[%s:%s:%c:%s:%d.%d]]", naoHaMaisPedidos, pclientRequest->viatura.matricula, pclientRequest->viatura.pais, pclientRequest->viatura.categoria, pclientRequest->viatura.nomeCondutor, pclientRequest->pidCliente, pclientRequest->pidServidorDedicado);
    return naoHaMaisPedidos;
}

/**
 * @brief  s2_2_2_ProcuraLugarDisponivelBD Ler a descrição da tarefa S2.2.2 no enunciado
 * @param  clientRequest (I) pedido recebido, enviado por um Cliente
 * @param  lugaresEstacionamento (I) array de lugares de estacionamento que irá servir de BD
 * @param  dimensaoMaximaParque (I) número máximo de lugares do parque, especificado pelo utilizador
 * @param  pindexClienteBD (O) índice do lugar correspondente a este pedido na BD (>= 0), ou -1 se não houve nenhum lugar disponível
 */
void s2_2_2_ProcuraLugarDisponivelBD(Estacionamento clientRequest, Estacionamento *lugaresEstacionamento, int dimensaoMaximaParque, int *pindexClienteBD) {
    so_debug("< [@param clientRequest:[%s:%s:%c:%s:%d:%d], lugaresEstacionamento:%p, dimensaoMaximaParque:%d]", clientRequest.viatura.matricula, clientRequest.viatura.pais, clientRequest.viatura.categoria, clientRequest.viatura.nomeCondutor, clientRequest.pidCliente, clientRequest.pidServidorDedicado, lugaresEstacionamento, dimensaoMaximaParque);

    for(int i = 0; i < dimensaoMaximaParque; i++){
        if(lugaresEstacionamento[i].pidCliente == DISPONIVEL){
            *pindexClienteBD = i;
            lugaresEstacionamento[i] = clientRequest;
            so_success("S2.2.2", "Reservei Lugar: %d", *pindexClienteBD);
            break;
        } 
        else{
            *pindexClienteBD = DISPONIVEL;
        }

         
    }
    

    so_debug("> [*pindexClienteBD:%d]", *pindexClienteBD);
}

/**
 * @brief  s2_2_3_CriaServidorDedicado    Ler a descrição da tarefa S2.2.3 no enunciado
 * @param  lugaresEstacionamento (I) array de lugares de estacionamento que irá servir de BD
 * @param  indexClienteBD (I) índice do lugar correspondente a este pedido na BD (>= 0), ou -1 se não houve nenhum lugar disponível
 */
void s2_2_3_CriaServidorDedicado(Estacionamento *lugaresEstacionamento, int indexClienteBD) {
    so_debug("< [@param lugaresEstacionamento:%p, indexClienteBD:%d]", lugaresEstacionamento, indexClienteBD);

    int pid = fork();
    if(pid == -1){
        so_error("S2.2.3", "Não foi possível criar SD");
        s4_EncerraServidor(FILE_REQUESTS);
    }
    if(pid == 0){
        int pidSD = getpid();
        so_success("S2.2.3", "SD: Nasci com PID %d", pidSD);
        sd7_MainServidorDedicado();
    }
    else if(pid > 0){
        if(indexClienteBD >= 0){
            lugaresEstacionamento[indexClienteBD].pidServidorDedicado = pid;
        }
        so_success("S2.2.3", "Servidor: Iniciei SD %d", pid);
    }
    

    so_debug(">");
}

/**
 * @brief  s3_TrataCtrlC    Ler a descrição da tarefa S3 no enunciado
 * @param  sinalRecebido (I) número do sinal que é recebido por esta função (enviado pelo SO)
 */
void s3_TrataCtrlC(int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    so_success("S3", "Servidor: Start Shutdown");
    int pidSD;
    for(int i = 0; i < dimensaoMaximaParque; i++){
        if(lugaresEstacionamento[i].pidCliente != DISPONIVEL){
            pidSD = lugaresEstacionamento[i].pidServidorDedicado;
            if(kill(pidSD, SIGUSR2) == -1){
                so_error("S3", "Erro ao terminar SD %d", pidSD);
                s4_EncerraServidor(FILE_REQUESTS);
            }
            //lugaresEstacionamento[i].pidCliente = DISPONIVEL;
        } 
    }
    char *fileFifoServidor = FILE_REQUESTS;
    s4_EncerraServidor(fileFifoServidor);
    

    so_debug(">");
}

/**
 * @brief  s4_EncerraServidor Ler a descrição da tarefa S4 no enunciado
 * @param  filenameFifoServidor (I) O nome do FIFO do servidor (i.e., FILE_REQUESTS)
 */
void s4_EncerraServidor(char *filenameFifoServidor) {
    so_debug("< [@param filenameFifoServidor:%s]", filenameFifoServidor);

    if(remove(filenameFifoServidor) != 0) so_error("S4", "Não foi possível eliminar O FIFO");
    else{
        so_success("S4", "Servidor: End Shutdown");
    }
    exit(0);
    

    so_debug(">");
}

/**
 * @brief  s5_TrataTerminouServidorDedicado    Ler a descrição da tarefa S5 no enunciado
 * @param  sinalRecebido (I) número do sinal que é recebido por esta função (enviado pelo SO)
 */
void s5_TrataTerminouServidorDedicado(int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    int status;
    pid_t resultado;
    resultado = wait(&status);
    if(resultado > 0){
        so_success("S5", "Servidor: Confirmo que terminou o SD %d", resultado);
    }        
        

    

    so_debug(">");
}

/**
 * @brief  sd7_ServidorDedicado Ler a descrição da tarefa SD7 no enunciado
 *         OS ALUNOS NÃO DEVERÃO ALTERAR ESTA FUNÇÃO.
 */
void sd7_MainServidorDedicado() {
    so_debug("<");

    // sd7_IniciaServidorDedicado:
    sd7_1_ArmaSinaisServidorDedicado();
    sd7_2_ValidaPidCliente(clientRequest);
    sd7_3_ValidaLugarDisponivelBD(indexClienteBD);

    // sd8_ValidaPedidoCliente:
    sd8_1_ValidaMatricula(clientRequest);
    sd8_2_ValidaPais(clientRequest);
    sd8_3_ValidaCategoria(clientRequest);
    sd8_4_ValidaNomeCondutor(clientRequest);

    // sd9_EntradaCliente:
    sd9_1_AdormeceTempoRandom();
    sd9_2_EnviaSigusr1AoCliente(clientRequest);
    sd9_3_EscreveLogEntradaViatura(FILE_LOGFILE, clientRequest, &posicaoLogfile, &logItem);

    // sd10_AcompanhaCliente:
    sd10_1_AguardaCheckout();
    sd10_2_EscreveLogSaidaViatura(FILE_LOGFILE, posicaoLogfile, logItem);

    sd11_EncerraServidorDedicado();

    so_error("Servidor Dedicado", "O programa nunca deveria ter chegado a este ponto!");

    so_debug(">");
}

/**
 * @brief  sd7_1_ArmaSinaisServidorDedicado    Ler a descrição da tarefa SD7.1 no enunciado
 */
void sd7_1_ArmaSinaisServidorDedicado() {
    so_debug("<");

    if(signal(SIGUSR2, sd12_TrataSigusr2) == SIG_ERR){ 
        so_error("SD7.1", "Erro no tratamento do sinal SIGUSR2");
        exit(1);
    } else{
        so_success("SD7.1", "Sinal SIGUSR2 tratado com sucesso");
    }
    if(signal(SIGINT, SIG_IGN) == SIG_ERR){ 
        so_error("SD7.1", "Erro no tratamento do sinal SIGINT");
        exit(1);
    } else{
        so_success("SD7.1", "Sinal SIGINT tratado com sucesso");
    }
    if(signal(SIGUSR1, sd13_TrataSigusr1) == SIG_ERR){
        so_error("SD7.1", "Erro no tratamento do sinal SIGUSR1");
        exit(1);
    } else{
        so_success("SD7.1", "Sinal SIGUSR1 tratado com sucesso");
    }
    

    so_debug(">");
}

/**
 * @brief  sd7_2_ValidaPidCliente    Ler a descrição da tarefa SD7.2 no enunciado
 * @param  clientRequest (I) pedido recebido, enviado por um Cliente
 */
void sd7_2_ValidaPidCliente(Estacionamento clientRequest) {
    so_debug("< [@param clientRequest:[%s:%s:%c:%s:%d:%d]]", clientRequest.viatura.matricula, clientRequest.viatura.pais, clientRequest.viatura.categoria, clientRequest.viatura.nomeCondutor, clientRequest.pidCliente, clientRequest.pidServidorDedicado);

    if(clientRequest.pidCliente > 0) so_success("SD7.2", "O Processo Cliente é válido");
    else{
        so_error("SD7.2", "O Processo Cliente é inválido");
        exit(1);
    }
    

    so_debug(">");
}

/**
 * @brief  sd7_3_ValidaLugarDisponivelBD    Ler a descrição da tarefa SD7.3 no enunciado
 * @param  indexClienteBD (I) índice do lugar correspondente a este pedido na BD (>= 0), ou -1 se não houve nenhum lugar disponível
 */
void sd7_3_ValidaLugarDisponivelBD(int indexClienteBD) {
    so_debug("< [@param indexClienteBD:%d]", indexClienteBD);

    if(indexClienteBD < 0){
        if(kill(clientRequest.pidCliente, SIGHUP) == INVALID){
            so_error("SD7.3", "Erro ao enviar o sinal SIGHUP ao Cliente");
            exit(1);
        }
    } else{
        so_success("SD7.3", "Lugar Validado");
    }
    

    so_debug(">");
}

/**
 * @brief  sd8_1_ValidaMatricula Ler a descrição da tarefa SD8.1 no enunciado
 * @param  clientRequest (I) pedido recebido, enviado por um Cliente
 */
void sd8_1_ValidaMatricula(Estacionamento clientRequest) {
    so_debug("< [@param clientRequest:[%s:%s:%c:%s:%d:%d]]", clientRequest.viatura.matricula, clientRequest.viatura.pais, clientRequest.viatura.categoria, clientRequest.viatura.nomeCondutor, clientRequest.pidCliente, clientRequest.pidServidorDedicado);

    char *matr = clientRequest.viatura.matricula;
    int matrLen = strlen(matr) - 1;
    for(int i = 0; i < matrLen; i++){
        if((matr[i] >= 'A' && matr[i] <= 'Z') || (matr[i] >= '0' && matr[i] <= '9')){
            so_success("SD8.1", "Matrícula Válida");
        } else {
            so_error("SD8.1", "Matrícula Inválida");
            sd11_EncerraServidorDedicado();
        }
    }
    

    so_debug(">");
}

/**
 * @brief  sd8_2_ValidaPais Ler a descrição da tarefa SD8.2 no enunciado
 * @param  clientRequest (I) pedido recebido, enviado por um Cliente
 */
void sd8_2_ValidaPais(Estacionamento clientRequest) {
    so_debug("< [@param clientRequest:[%s:%s:%c:%s:%d:%d]]", clientRequest.viatura.matricula, clientRequest.viatura.pais, clientRequest.viatura.categoria, clientRequest.viatura.nomeCondutor, clientRequest.pidCliente, clientRequest.pidServidorDedicado);

    char *pais = clientRequest.viatura.pais;
    int paisLen = strlen(pais);
    if(paisLen != 2){
        so_error("SD8.2", "Código País Inválido");
        sd11_EncerraServidorDedicado();
    } 
    for(int i = 0; i < paisLen; i++){
        if((pais[i] >= 'A') && (pais[i] <= 'Z')){
            so_success("SD8.2", "Código País Válido");
        } else {
            so_error("SD8.2", "Código País Inválido");
            sd11_EncerraServidorDedicado();
        }
    }
    

    so_debug(">");
}

/**
 * @brief  sd8_3_ValidaCategoria Ler a descrição da tarefa SD8.3 no enunciado
 * @param  clientRequest (I) pedido recebido, enviado por um Cliente
 */
void sd8_3_ValidaCategoria(Estacionamento clientRequest) {
    so_debug("< [@param clientRequest:[%s:%s:%c:%s:%d:%d]]", clientRequest.viatura.matricula, clientRequest.viatura.pais, clientRequest.viatura.categoria, clientRequest.viatura.nomeCondutor, clientRequest.pidCliente, clientRequest.pidServidorDedicado);

    char categ = clientRequest.viatura.categoria;
    if((categ == 'P') || (categ == 'L') || (categ == 'M')){
        so_success("SD8.3", "Categoria da Viatura Válida");
    } else {
        so_error("SD8.3", "Categoria da Viatura Inválida");
        sd11_EncerraServidorDedicado();
    }
    

    so_debug(">");
}

/**
 * @brief  sd8_4_ValidaNomeCondutor Ler a descrição da tarefa SD8.4 no enunciado
 * @param  clientRequest (I) pedido recebido, enviado por um Cliente
 */
void sd8_4_ValidaNomeCondutor(Estacionamento clientRequest) {
    so_debug("< [@param clientRequest:[%s:%s:%c:%s:%d:%d]]", clientRequest.viatura.matricula, clientRequest.viatura.pais, clientRequest.viatura.categoria, clientRequest.viatura.nomeCondutor, clientRequest.pidCliente, clientRequest.pidServidorDedicado);

    char *nomeCon = clientRequest.viatura.nomeCondutor;
    char *buffer = malloc(sizeof(char) * 151);
    FILE *passwd = fopen(FILE_USERS, "r");
    if(passwd == NULL){
        so_error("SD8.4", "Erro ao abrir o ficheiro _etc_passwd");
        sd11_EncerraServidorDedicado();
    }
    int a = 0;
    while(fgets(buffer, 150, passwd) != NULL){
        if(strstr(buffer, nomeCon)){
            so_success("SD8.4", "Utilizador Válido");
            a = 1;
            break;
        }
    }
    if(!a){
        so_error("SD8.4", "Utilizador Inválido");
        sd11_EncerraServidorDedicado();
    }
    free(buffer);
    fclose(passwd);
    

    so_debug(">");
}

/**
 * @brief  sd9_1_AdormeceTempoRandom Ler a descrição da tarefa SD9.1 no enunciado
 */
void sd9_1_AdormeceTempoRandom() {
    so_debug("<");

    int a = so_random_between_values(1, MAX_ESPERA);
    so_success("SD9.1", "%d", a);
    sleep(a);
    

    so_debug(">");
}

/**
 * @brief  sd9_2_EnviaSigusr1AoCliente Ler a descrição da tarefa SD9.2 no enunciado
 * @param  clientRequest (I) pedido recebido, enviado por um Cliente
 */
void sd9_2_EnviaSigusr1AoCliente(Estacionamento clientRequest) {
    so_debug("< [@param clientRequest:[%s:%s:%c:%s:%d:%d]]", clientRequest.viatura.matricula, clientRequest.viatura.pais, clientRequest.viatura.categoria, clientRequest.viatura.nomeCondutor, clientRequest.pidCliente, clientRequest.pidServidorDedicado);

    if(kill(clientRequest.pidCliente, SIGUSR1) == INVALID){
        so_error("SD9.2", "Erro ao enviar o sinal SIGUSR1 ao Cliente");
        sd11_EncerraServidorDedicado();
    } else {
        so_success("SD9.2", "SD: Confirmei Cliente Lugar %d", indexClienteBD);
    }

    

    so_debug(">");
}

/**
 * @brief  sd9_3_EscreveLogEntradaViatura Ler a descrição da tarefa SD9.3 no enunciado
 * @param  logFilename (I) O nome do ficheiro de Logfile (i.e., FILE_LOGFILE)
 * @param  clientRequest (I) pedido recebido, enviado por um Cliente
 * @param  pposicaoLogfile (O) posição do ficheiro Logfile mesmo antes de inserir o log desta viatura
 * @param  plogItem (O) registo de Log para esta viatura
 */
void sd9_3_EscreveLogEntradaViatura(char *logFilename, Estacionamento clientRequest, long *pposicaoLogfile, LogItem *plogItem) {
    so_debug("< [@param logFilename:%s, clientRequest:[%s:%s:%c:%s:%d:%d]]", logFilename, clientRequest.viatura.matricula, clientRequest.viatura.pais, clientRequest.viatura.categoria, clientRequest.viatura.nomeCondutor, clientRequest.pidCliente, clientRequest.pidServidorDedicado);

    FILE *estacionamentos = fopen(logFilename, "ab");
    if(estacionamentos == NULL){
        so_error("SD9.3", "Não foi possível abrir o ficheiro");
        sd11_EncerraServidorDedicado();
    }
    *pposicaoLogfile = ftell(estacionamentos);
    if(*pposicaoLogfile == -1L){
        so_error("SD9.3", "Erro ao obter a posição do arquivo");
        fclose(estacionamentos);
        sd11_EncerraServidorDedicado();
    }
    plogItem->viatura = clientRequest.viatura;
    time_t time1 = time(NULL);
    struct tm *time2 = localtime(&time1);
    char *timeFormated;
    timeFormated = malloc(sizeof(char) * 17);
    strftime(timeFormated, 17, "%Y-%m-%dT%Hh%M", time2);
    strcpy(plogItem->dataEntrada, timeFormated);
    size_t num = fwrite(plogItem, sizeof(LogItem), 1, estacionamentos);
    if(num != 1){
        free(timeFormated);
        so_error("SD9.3", "Erro ao escrever no ficheiro");
        fclose(estacionamentos);
        sd11_EncerraServidorDedicado();
    }
    else{
        fclose(estacionamentos);
        so_success("SD9.3", "SD: Guardei log na posição %ld: Entrada Cliente %s em %s", *pposicaoLogfile, clientRequest.viatura.matricula, timeFormated);
        free(timeFormated);
    }
    

    so_debug("> [*pposicaoLogfile:%ld, *plogItem:[%s:%s:%c:%s:%s:%s]]", *pposicaoLogfile, plogItem->viatura.matricula, plogItem->viatura.pais, plogItem->viatura.categoria, plogItem->viatura.nomeCondutor, plogItem->dataEntrada, plogItem->dataSaida);
}

/**
 * @brief  sd10_1_AguardaCheckout Ler a descrição da tarefa SD10.1 no enunciado
 */
void sd10_1_AguardaCheckout() {
    so_debug("<");

    pause();
    so_success("SD10.1", "SD: A viatura %s deseja sair do parque", clientRequest.viatura.matricula);
    

    so_debug(">");
}

/**
 * @brief  sd10_2_EscreveLogSaidaViatura Ler a descrição da tarefa SD10.2 no enunciado
 * @param  logFilename (I) O nome do ficheiro de Logfile (i.e., FILE_LOGFILE)
 * @param  posicaoLogfile (I) posição do ficheiro Logfile mesmo antes de inserir o log desta viatura
 * @param  logItem (I) registo de Log para esta viatura
 */
void sd10_2_EscreveLogSaidaViatura(char *logFilename, long posicaoLogfile, LogItem logItem) {
    so_debug("< [@param logFilename:%s, posicaoLogfile:%ld, logItem:[%s:%s:%c:%s:%s:%s]]", logFilename, posicaoLogfile, logItem.viatura.matricula, logItem.viatura.pais, logItem.viatura.categoria, logItem.viatura.nomeCondutor, logItem.dataEntrada, logItem.dataSaida);

    FILE *estacionamentos = fopen(logFilename, "rb+");
    if(estacionamentos == NULL){
        so_error("SD10.2", "Não foi possível abrir o ficheiro");
        sd11_EncerraServidorDedicado();
    }
    if(fseek(estacionamentos, posicaoLogfile, SEEK_SET) != 0) {
        so_error("SD10.2", "Erro ao posicionar cursor");
        sd11_EncerraServidorDedicado();
    }
    LogItem *plogItem = &logItem;
    time_t time1 = time(NULL);
    struct tm *time2 = localtime(&time1);
    char *timeFormated;
    timeFormated = malloc(sizeof(char) * 17);
    strftime(timeFormated, 17, "%Y-%m-%dT%Hh%M", time2);
    strcpy(plogItem->dataSaida, timeFormated);
    size_t num = fwrite(plogItem, sizeof(LogItem), 1, estacionamentos);
    if(num != 1){
        free(timeFormated);
        so_error("SD10.2", "Erro ao escrever no ficheiro");
        fclose(estacionamentos);
        sd11_EncerraServidorDedicado();
    }
    else{
        fclose(estacionamentos);
        so_success("SD10.2", "SD: Atualizei log na posição %ld: Saída Cliente %s em %s", posicaoLogfile, logItem.viatura.matricula, timeFormated);
        free(timeFormated);
        sd11_EncerraServidorDedicado();
    }
    

    so_debug(">");
}

/**
 * @brief  sd11_EncerraServidorDedicado Ler a descrição da tarefa SD11 no enunciado
 *         OS ALUNOS NÃO DEVERÃO ALTERAR ESTA FUNÇÃO.
 */
void sd11_EncerraServidorDedicado() {
    so_debug("<");

    sd11_1_LibertaLugarViatura(lugaresEstacionamento, indexClienteBD);
    sd11_2_EnviaSighupAoClienteETermina(clientRequest);

    so_debug(">");
}

/**
 * @brief  sd11_1_LibertaLugarViatura Ler a descrição da tarefa SD11.1 no enunciado
 * @param  lugaresEstacionamento (I) array de lugares de estacionamento que irá servir de BD
 * @param  indexClienteBD (I) índice do lugar correspondente a este pedido na BD (>= 0), ou -1 se não houve nenhum lugar disponível
 */
void sd11_1_LibertaLugarViatura(Estacionamento *lugaresEstacionamento, int indexClienteBD) {
    so_debug("< [@param lugaresEstacionamento:%p, indexClienteBD:%d]", lugaresEstacionamento, indexClienteBD);
    
    if (indexClienteBD < 0 || indexClienteBD >= dimensaoMaximaParque) {
        so_error("SD11.1", "Índice do lugar do pedido inválido");
    }
    lugaresEstacionamento[indexClienteBD].pidCliente = DISPONIVEL;
    so_success("SD11.1", "SD: Libertei Lugar: %d", indexClienteBD);
    

    so_debug(">");
}

/**
 * @brief  sd11_2_EnviaSighupAoClienteETerminaSD Ler a descrição da tarefa SD11.2 no enunciado
 * @param  clientRequest (I) pedido recebido, enviado por um Cliente
 */
void sd11_2_EnviaSighupAoClienteETermina(Estacionamento clientRequest) {
    so_debug("< [@param clientRequest:[%s:%s:%c:%s:%d:%d]]", clientRequest.viatura.matricula, clientRequest.viatura.pais, clientRequest.viatura.categoria, clientRequest.viatura.nomeCondutor, clientRequest.pidCliente, clientRequest.pidServidorDedicado);

    if(kill(clientRequest.pidCliente, SIGHUP) == INVALID){
        so_error("SD11.2", "Erro ao enviar o sinal SIGHUP ao Cliente");
        exit(0);
    } else {
        so_success("SD11.2", "SD: Shutdown");
        exit(0);
    }
    

    so_debug(">");
}

/**
 * @brief  sd12_TrataSigusr2    Ler a descrição da tarefa SD12 no enunciado
 * @param  sinalRecebido (I) número do sinal que é recebido por esta função (enviado pelo SO)
 */
void sd12_TrataSigusr2(int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);
    
    so_success("SD12", "SD: Recebi pedido do Servidor para terminar");
    sd11_EncerraServidorDedicado();
    

    so_debug(">");
}

/**
 * @brief  sd13_TrataSigusr1    Ler a descrição da tarefa SD13 no enunciado
 * @param  sinalRecebido (I) número do sinal que é recebido por esta função (enviado pelo SO)
 */
void sd13_TrataSigusr1(int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    so_success("SD13", "SD: Recebi pedido do Cliente para terminar o estacionamento");
    

    so_debug(">");
}