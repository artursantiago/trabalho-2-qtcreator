#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include <semaphore.h>

#define VAZIO 0

sem_t trens[5]; //um para cada trem
sem_t mutex;//exclusao mutua para regioes cri­ticas

int ocupacaoStation[7];
int lastFreeStation[7]; //Último trem liberado na linha de conflito
int breakTrem[5]; //Linha em que o trem tá parado

int is_on = true;



void ocuparStation(int id, int linha);

bool avoidDeadlockTrem1(int linhaLocal);
bool avoidDeadlockTrem2(int linhaLocal);
bool avoidDeadlockTrem3(int linhaLocal);
bool avoidDeadlockTrem4(int linhaLocal);
bool avoidDeadlockTrem5(int linhaLocal);

bool (*avoidDeadlockTrem[5])(int);

void liberarStation(int id, int linha);
void liberarTremParado(int id, int linha);

void progressTrem1(int x, int y);
void progressTrem2(int x, int y);
void progressTrem3(int x, int y);
void progressTrem4(int x, int y);
void progressTrem5(int x, int y);


void verifySpeed(int speed, Trem *trem);



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    for(int i= 0; i < 5 ;i++ ){
       sem_init(&trens[i], 0, 1);
       sem_wait(&trens[i]);
    }

    avoidDeadlockTrem[0] = &avoidDeadlockTrem1;
    avoidDeadlockTrem[1] = &avoidDeadlockTrem2;
    avoidDeadlockTrem[2] = &avoidDeadlockTrem3;
    avoidDeadlockTrem[3] = &avoidDeadlockTrem4;
    avoidDeadlockTrem[4] = &avoidDeadlockTrem5;


    sem_init(&mutex, 0, 1);


    //Cria o trem com seu (ID, posição X, posição Y)
    trem1 = new Trem(1,130,30, ui->speed_trem1->value(), &progressTrem1);
    trem2 = new Trem(2,330,30, ui->speed_trem2->value(), &progressTrem2);
    trem3 = new Trem(3,530,30, ui->speed_trem3->value(), &progressTrem3);
    trem4 = new Trem(4,230,150, ui->speed_trem4->value(), &progressTrem4);
    trem5 = new Trem(5,430,150, ui->speed_trem5->value(), &progressTrem5);


    /*
     * Conecta o sinal UPDATEGUI à função UPDATEINTERFACE.
     * Ou seja, sempre que o sinal UPDATEGUI foi chamado, será executada a função UPDATEINTERFACE.
     * Os 3 parâmetros INT do sinal serão utilizados na função.
     * Trem1 e Trem2 são os objetos que podem chamar o sinal. Se um outro objeto chamar o
     * sinal UPDATEGUI, não haverá execução da função UPDATEINTERFACE
     */
    connect(trem1,SIGNAL(updateGUI(int,int,int)),SLOT(updateInterface(int,int,int)));
    connect(trem2,SIGNAL(updateGUI(int,int,int)),SLOT(updateInterface(int,int,int)));
    connect(trem3,SIGNAL(updateGUI(int,int,int)),SLOT(updateInterface(int,int,int)));
    connect(trem4,SIGNAL(updateGUI(int,int,int)),SLOT(updateInterface(int,int,int)));
    connect(trem5,SIGNAL(updateGUI(int,int,int)),SLOT(updateInterface(int,int,int)));

}

//Função que será executada quando o sinal UPDATEGUI for emitido
void MainWindow::updateInterface(int id, int x, int y){
    switch(id){
    case 1: //Atualiza a posição do objeto da tela (quadrado) que representa o trem1
        ui->label_trem1->setGeometry(x,y,21,17);
        break;
    case 2: //Atualiza a posição do objeto da tela (quadrado) que representa o trem2
        ui->label_trem2->setGeometry(x,y,21,17);
        break;
    case 3: //Atualiza a posição do objeto da tela (quadrado) que representa o trem3
        ui->label_trem3->setGeometry(x,y,21,17);
        break;
    case 4: //Atualiza a posição do objeto da tela (quadrado) que representa o trem4
        ui->label_trem4->setGeometry(x,y,21,17);
        break;
    case 5: //Atualiza a posição do objeto da tela (quadrado) que representa o trem5
        ui->label_trem5->setGeometry(x,y,21,17);
        break;
    default:
        break;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


/*
 * Ao clicar, trens começam execução
 */
void MainWindow::on_pushButton_clicked()
{
    is_on = true;
    trem1->start();
    trem2->start();
    trem3->start();
    trem4->start();
    trem5->start();
}

/*
 * Ao clicar, trens param execução
 */
void MainWindow::on_pushButton_2_clicked()
{
    is_on = false;
    trem1->terminate();
    trem2->terminate();
    trem3->terminate();
    trem4->terminate();
    trem5->terminate();
}


void MainWindow::on_speed_trem1_valueChanged(int value)
{
    verifySpeed(value, trem1);
}

void MainWindow::on_speed_trem2_valueChanged(int value)
{
    verifySpeed(value, trem2);
}

void MainWindow::on_speed_trem3_valueChanged(int value)
{
    verifySpeed(value, trem3);
}

void MainWindow::on_speed_trem4_valueChanged(int value)
{
    verifySpeed(value, trem4);
}

void MainWindow::on_speed_trem5_valueChanged(int value)
{
    verifySpeed(value, trem5);
}

void verifySpeed(int speed, Trem *trem) {
    if (is_on) {
        trem->mudarVelocidade(speed);
    }
}

/* Funções para liberação e ocupação dos trilhos */

void ocuparStation(int id, int linha){
    int localTrem= id-1;
    int linhaLocal = linha -1;

    sem_wait(&mutex);//down(&mutex); /* entra na regiao critica */

    if (ocupacaoStation[linhaLocal] == VAZIO && (*avoidDeadlockTrem[id-1])(linhaLocal)) {
        ocupacaoStation[linhaLocal] = id; /* registra que trem entrou na região*/
        sem_post(&trens[localTrem]); /*up(&s[i]); */
    }else{
        breakTrem[localTrem] = linha;
    }

    sem_post(&mutex);//up(&mutex);  /* sai da regiao cri­tica */
    sem_wait(&trens[localTrem]);//down(&s[i]); /* bloqueia se linha estiver ocupada */
}

void liberarStation(int id, int linha){
    int linhaLocal = linha -1;

    sem_wait(&mutex);//down(&mutex); /* entra na regiao critica */
    ocupacaoStation[linhaLocal] = VAZIO; /* registra que trem entrou na região*/
    liberarTremParado(id-1, linha);

    sem_post(&mutex);//up(&mutex);  /* sai da regiao cri­tica */
}

void liberarTremParado(int id, int linha){
    int linhaLocal = linha -1;
    int trem = lastFreeStation[linhaLocal];
    int busca = true;

    do{

        if(breakTrem[trem] == linha && trem != id && (*avoidDeadlockTrem[trem])(linhaLocal)){
            breakTrem[trem] = VAZIO; //Indicar que o trem não está preso
            busca = false; //Encerrar while
            lastFreeStation[linhaLocal] = trem; // Último trem liberado no local
            sem_post(&trens[trem]); /*up(&s[i]); */
        }


        //Modificar posicao do trem
        if(trem>3){
            trem = 0;
        }else{
            trem++;
        }

    }while(trem != lastFreeStation[linhaLocal]  && busca);
}


/* Funções para prevenção do Deadlock */
bool avoidDeadlockTrem1(int linhaLocal) {
    if (linhaLocal == 0 && ocupacaoStation[2] == 4 &&
            (ocupacaoStation[3] == 2 || (ocupacaoStation[4] == 2 && ocupacaoStation[6] == 5))) {
        return false;
    }

    if (linhaLocal == 0
        && ocupacaoStation[1] == 2
        && ocupacaoStation[5] == 3
        && ocupacaoStation[6] == 5
        && ocupacaoStation[2] == 4) {
        return false;
    }
    return true;
}

bool avoidDeadlockTrem2(int linhaLocal) {
    if (linhaLocal == 3 && ocupacaoStation[0] == 1 && ocupacaoStation[2] == 4) {
        return false;
    }

    if (linhaLocal == 1 && ocupacaoStation[5] == 3 &&
            (ocupacaoStation[4] == 5 || (ocupacaoStation[6] == 5 && ocupacaoStation[3] == 4))) {
        return false;
    }

    if (linhaLocal == 4 && ocupacaoStation[6] == 5
            && (ocupacaoStation[3] == 4 || (ocupacaoStation[0] == 1 && ocupacaoStation[2] == 4))) {
        return false;
    }

    if (linhaLocal == 1
        && ocupacaoStation[0] == 1
        && ocupacaoStation[5] == 3
        && ocupacaoStation[6] == 5
        && ocupacaoStation[2] == 4) {
        return false;
    }
    return true;
}

bool avoidDeadlockTrem3(int linhaLocal) {
    if (linhaLocal == 5 && ocupacaoStation[1] == 2 &&
            (ocupacaoStation[4] == 5 || (ocupacaoStation[6] == 5 && ocupacaoStation[3] == 4))) {
        return false;
    }

    if (linhaLocal == 5
        && ocupacaoStation[0] == 1
        && ocupacaoStation[1] == 2
        && ocupacaoStation[6] == 5
        && ocupacaoStation[2] == 4) {
        return false;
    }
    return true;
}

bool avoidDeadlockTrem4(int linhaLocal) {
    if (linhaLocal == 2 && ocupacaoStation[0] == 1 &&
            (ocupacaoStation[3] == 2 || (ocupacaoStation[4] == 2 && ocupacaoStation[6] == 5))) {
        return false;
    }

    if (linhaLocal == 3 && ocupacaoStation[6] == 5 &&
            (ocupacaoStation[4] == 2 || (ocupacaoStation[1] == 2 && ocupacaoStation[5] == 3))) {
        return false;
    }

    if (linhaLocal == 2
        && ocupacaoStation[0] == 1
        && ocupacaoStation[1] == 2
        && ocupacaoStation[6] == 5
        && ocupacaoStation[5] == 3) {
        return false;
    }
    return true;
}

bool avoidDeadlockTrem5(int linhaLocal) {
    if (linhaLocal == 4 && ocupacaoStation[1] == 2 && ocupacaoStation[5] == 3) {
        return false;
    }

    if (linhaLocal == 6 ) {
        if(ocupacaoStation[3] == 4 &&
                (ocupacaoStation[4] == 2 || (ocupacaoStation[1] == 2 && ocupacaoStation[5] == 3))){
            return false;
        }
        if(ocupacaoStation[0] == 1 && ocupacaoStation[2] == 4 && ocupacaoStation[4] == 2){
            return false;
        }

        if (ocupacaoStation[0] == 1
            && ocupacaoStation[1] == 2
            && ocupacaoStation[2] == 4
            && ocupacaoStation[5] == 3) {
            return false;
        }
    }

    return true;
}


/* Funções para verificar movimento do trem */
void progressTrem1(int x, int y){
    if(x==330 && y==30){
        ocuparStation(1, 1);//Solicita linha 1
    }
    if(x==330 && y==150){
        ocuparStation(1, 3);//Solicita linha 3
    }
    if(x==290 && y==150){
        liberarStation(1,1); // Libera linha 1
    }
    if(x==190 && y==150){
        liberarStation(1,3);//Libera linha 3
    }
}

void progressTrem2(int x, int y){
    if(x==530 && y==30){
        ocuparStation(2, 2);//Solicita linha 2
    }
    if(x==530 && y==150){
        ocuparStation(2, 5);//Solicita linha 5
    }
    if(x==430 && y==150){
        ocuparStation(2, 4);//Solicita linha 4
    }
    if(x==330 && y==150){
        ocuparStation(2, 1);//Solicita linha 1
    }

    if(x==490 && y==150){
        liberarStation(2,2);//Libera linha 2
    }
    if(x==390 && y==150){
        liberarStation(2,5);//Libera linha 5
    }
    if(x==330 && y==110){
        liberarStation(2,4);//Libera linha 4
    }
    if(x==370 && y==30){
       liberarStation(2,1); // Libera linha 1
    }
}

void progressTrem3(int x, int y){
    if(x==530 && y==150){
       ocuparStation(3,2); //Solicita linha 2
    }
    if(x==630 && y==150){
        ocuparStation(3,6);//Solicita linha 6
    }
    if(x==570 && y==30){
        liberarStation(3,2);//Libera linha 2
    }
    if(x==530 && y==110){
        liberarStation(3,6);//Libera linha 6
    }
}

void progressTrem4(int x, int y){
    if(x==230 && y==150){
        ocuparStation(4,3);//Solicita linha 3
    }
    if(x==330 && y==150){
        ocuparStation(4,4);//Solicita linha 4
    }
    if(x==430 && y==150){
        ocuparStation(4,7);//Solicita linha 7
    }


    if(x==370 && y==150){
        liberarStation(4,3);//Libera linha 3
    }
    if(x==430 && y==190){
        liberarStation(4,4);//Libera linha 4
    }
    if(x==390 && y==270){
        liberarStation(4,7);//Libera linha 7
    }
}

void progressTrem5(int x, int y){
    if(x==430 && y==150){
        ocuparStation(5,5);//Solicita linha 5
    }
    if(x==530 && y==150){
        ocuparStation(5,6);//Solicita linha 6
    }
    if(x==430 && y==270){
        ocuparStation(5,7);//Solicita linha 7
    }


    if(x==570 && y==150){
        liberarStation(5,5);//Libera linha 5
    }
    if(x==630 && y==190){
        liberarStation(5,6);//Libera linha 6
    }
    if(x==470 && y==150){
        liberarStation(5,7);//Libera linha 7
    }
}
