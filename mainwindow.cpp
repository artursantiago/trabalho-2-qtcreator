#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include <semaphore.h>

#define VAZIO 0

sem_t station[7]; //um semaforo por regiao critica
sem_t mutex;//exclusao mutua para regioes cri­ticas

int ocupacaoStation[7];

void liberarStation(int linha);
void ocuparStation(int id, int linha);

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

    sem_init(&mutex, 0, 1);
//    for(int i= 0; i < 7 ;i++ ){
//       sem_init(&station[i], 0, 1);
//    }


    //Cria o trem com seu (ID, posição X, posição Y)
    trem1 = new Trem(1,130,30, ui->speed_trem1->value(), &progressTrem1);
    trem2 = new Trem(2,430,150, ui->speed_trem2->value(), &progressTrem2);
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
    if (speed <= 0) {
        trem->terminate();
    } else {
        trem->start();
        trem->mudarVelocidade(speed);
    }
}


void ocuparStation(int id, int linha){
    sem_wait(&mutex);//down(&mutex); /* entra na regiao critica */

    printf("Trem%d\n", id);
    printf("linha = %d\n", linha + 1);
    printf("ocupacaoStation[%d] = %d\n", linha, ocupacaoStation[linha]);

    if (ocupacaoStation[linha] == VAZIO) {
        printf("VAZIA\n");
        ocupacaoStation[linha] = id; /* registra que trem entrou na região*/
        sem_post(&station[linha]); /*up(&s[i]); */
    } else {
        printf("OCUPADA\n");
    }


    sem_post(&mutex);//up(&mutex);  /* sai da regiao cri­tica */
    sem_wait(&station[linha]);//down(&s[i]); /* bloqueia se linha estiver ocupada */
}

void liberarStation(int linha){
    sem_wait(&mutex);//down(&mutex); /* entra na regiao critica */

    ocupacaoStation[linha] = VAZIO; /* registra que trem entrou na região*/

    sem_post(&station[linha]);///* up(&s[i]); */
    sem_post(&mutex);//up(&mutex);  /* sai da regiao cri­tica */
}


void progressTrem1(int x, int y){
//    sem_wait(&station[1]);
    if(x==330 && y==30){
        ocuparStation(1, 0);//Solicita linha 1
    }
    if(x==330 && y==150){
        ocuparStation(1, 2);//Solicita linha 3
    }
    if(x==310 && y==150){
        liberarStation(0); // Libera linha 1
    }
    if(x==210 && y==150){
        liberarStation(2);//Libera linha 3
    }
}

void progressTrem2(int x, int y){
    if(x==530 && y==30){
        ocuparStation(2, 1);//Solicita linha 2
    }
    if(x==530 && y==150){
        ocuparStation(2, 4);//Solicita linha 5
    }
    if(x==430 && y==150){
        ocuparStation(2, 3);//Solicita linha 4
    }
    if(x==330 && y==150){
        ocuparStation(2, 0);//Solicita linha 1
    }

    if(x==510 && y==150){
        liberarStation(1);//Libera linha 2
    }
    if(x==410 && y==150){
        liberarStation(4);//Libera linha 5
    }
    if(x==330 && y==130){
        liberarStation(3);//Libera linha 4
    }
    if(x==350 && y==30){
        liberarStation(0); // Libera linha 1
    }
}

void progressTrem3(int x, int y){
    if(x==630 && y==150){
        //Solicita linha 2
    }
    if(x==530 && y==150){
        //Solicita linha 6
    }
    if(x==530 && y==130){
        //Libera linha 2
    }
    if(x==550 && y==30){
        //Libera linha 6
    }
}



void progressTrem4(int x, int y){
    if(x==230 && y==170){
        //Solicita linha 3
    }
    if(x==330 && y==150){
        //Solicita linha 4
    }
    if(x==430 && y==150){
        //Solicita linha 7
    }


    if(x==350 && y==150){
        //Libera linha 3
    }
    if(x==430 && y==170){
        //Libera linha 4
    }
    if(x==410 && y==270){
        //Libera linha 7
    }
}

void progressTrem5(int x, int y){

    if(x==430 && y==150){
        //Solicita linha 5
    }
    if(x==530 && y==150){
        //Solicita linha 6
    }
    if(x==430 && y==270){
        //Solicita linha 7
    }


    if(x==550 && y==150){
        //Libera linha 5
    }
    if(x==630 && y==170){
        //Libera linha 6
    }
    if(x==450 && y==150){
        //Libera linha 7
    }
}
