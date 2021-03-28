#include "trem.h"
#include <QtCore>

#define MAX_SPEED 200

//Construtor
Trem::Trem(int ID, int x, int y, int velocidade, void (*progress)(int, int)){
    this->ID = ID;
    this->x = x;
    this->y = y;
    this->velocidade = MAX_SPEED - velocidade;
    this->progress = progress;
}

//Função a ser executada após executar trem->START
void Trem::run(){
    while(true){
        if (velocidade == 200) {
            continue;
        }
        switch(ID){
        case 1://Trem 1
            if (y == 30 && x <330)
                x+=20;
            else if (x == 330 && y < 150)
                y+=20;
            else if (x > 130 && y == 150)
                x-=20;
            else
                y-=20;
            progress(x, y);
            emit updateGUI(ID, x,y);    //Emite um sinal
            break;
        case 2: //Trem 2
            if (y == 30 && x <530)
                x+=20;
            else if (x == 530 && y < 150)
                y+=20;
            else if (x > 330 && y == 150)
                x-=20;
            else
                y-=20;
            progress(x, y);
            emit updateGUI(ID, x,y);    //Emite um sinal
            break;
        case 3: //Trem 3
            if (y == 30 && x <730)
                x+=20;
            else if (x == 730 && y < 150)
                y+=20;
            else if (x > 530 && y == 150)
                x-=20;
            else
                y-=20;
            progress(x, y);
            emit updateGUI(ID, x,y);    //Emite um sinal
            break;
        case 4: //Trem 4
            if (y == 150 && x <430)
                x+=20;
            else if (x == 430 && y < 270)
                y+=20;
            else if (x > 230 && y == 270)
                x-=20;
            else
                y-=20;
            progress(x, y);
            emit updateGUI(ID, x,y);    //Emite um sinal
            break;
        case 5: //Trem 5
            if (y == 150 && x <630)
                x+=20;
            else if (x == 630 && y < 270)
                y+=20;
            else if (x > 430 && y == 270)
                x-=20;
            else
                y-=20;
            progress(x, y);
            emit updateGUI(ID, x,y);    //Emite um sinal
            break;
        default:
            break;
        }
        msleep(velocidade);
    }
}

void Trem::mudarVelocidade(int velocidade) {
    this->velocidade = MAX_SPEED - velocidade;
}
