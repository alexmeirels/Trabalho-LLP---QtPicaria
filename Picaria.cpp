#include "Picaria.h"
#include "ui_Picaria.h"

#include <QDebug>
#include <QMessageBox>
#include <QActionGroup>
#include <QSignalMapper>


Picaria::Player state2player(Hole::State state) {
    switch (state) {
        case Hole::RedState:
            return Picaria::RedPlayer;
        case Hole::BlueState:
            return Picaria::BluePlayer;
        default:
            Q_UNREACHABLE();
    }
}

Hole::State player2state(Picaria::Player player) {

    return player == Picaria::RedPlayer ? Hole::RedState : Hole::BlueState;
}

Picaria::Picaria(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::Picaria),
      m_mode(Picaria::NineHoles),
      m_player(Picaria::RedPlayer),
      m_phase(Picaria::DropPhase),
      m_dropCount(0),
      m_selected(nullptr){

    ui->setupUi(this);

    QActionGroup* modeGroup = new QActionGroup(this);
    modeGroup->setExclusive(true);
    modeGroup->addAction(ui->action9holes);
    modeGroup->addAction(ui->action13holes);

    QObject::connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(reset()));
    QObject::connect(ui->actionQuit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    QObject::connect(modeGroup, SIGNAL(triggered(QAction*)), this, SLOT(updateMode(QAction*)));
    QObject::connect(this, SIGNAL(modeChanged(Picaria::Mode)), this, SLOT(reset()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(showAbout()));
    QObject::connect(this, SIGNAL(gameOver(Player)), this, SLOT(showGameOver(Player)));
    QObject::connect(this, SIGNAL(gameOver(Player)), this, SLOT(reset()));

    QSignalMapper* map = new QSignalMapper(this);
    for (int id = 0; id < 13; ++id) {

        QString holeName = QString("hole%1").arg(id+1, 2, 10, QChar('0'));
        Hole* hole = this->findChild<Hole*>(holeName);
        Q_ASSERT(hole != nullptr);

        m_holes[id] = hole;
        map->setMapping(hole, id);
        QObject::connect(hole, SIGNAL(clicked(bool)), map, SLOT(map()));
    }
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    QObject::connect(map, SIGNAL(mapped(int)), this, SLOT(play(int)));
#else
    QObject::connect(map, SIGNAL(mappedInt(int)), this, SLOT(play(int)));
#endif

    this->reset();

    this->adjustSize();
    this->setFixedSize(this->size());
}

Picaria::~Picaria() {
    delete ui;
}

void Picaria::setMode(Picaria::Mode mode) {
    if (m_mode != mode) {
        m_mode = mode;
        emit modeChanged(mode);
    }
}

void Picaria::switchPlayer() {
    m_player = m_player == Picaria::RedPlayer ?
                    Picaria::BluePlayer : Picaria::RedPlayer;
    this->updateStatusBar();
}

void Picaria::play(int id) {
    Hole* hole = m_holes[id];
    Q_ASSERT(hole != nullptr);
    qDebug() << "clicked on: " << hole->objectName();

    switch (m_phase) {
        case Picaria::DropPhase:
            drop(hole);
            break;
        case Picaria::MovePhase:
            move(hole, id);
            break;
        default:
            Q_UNREACHABLE();
    }


}
void Picaria::drop(Hole* hole) {
    if (hole->state() == Hole::EmptyState) {
        hole->setState(player2state(m_player));

        if (isGameOver()==true)
            emit gameOver(m_player);
        else{
            ++m_dropCount;
            if (m_dropCount == 6){
                m_phase = Picaria::MovePhase;
            }

            this->switchPlayer();
        }
    }
}

void Picaria::move(Hole* hole, int id) {
    QPair<Hole*,Hole*>* movement = nullptr;
    if (hole->state() == Hole::SelectableState) {
        Q_ASSERT(m_selected != 0);
        movement = new QPair<Hole*,Hole*>(m_selected, hole);
    } else {
        if (hole->state() == player2state(m_player)) {
            QList<Hole*> selectables = this->findSelectables(hole, id);
            if (selectables.count() == 1) {
                movement = new QPair<Hole*,Hole*>(hole, selectables.at(0));
            } else if (selectables.count() > 1) {
                this->clearSelectable();
                foreach (Hole* tmp, selectables)
                    tmp->setState(Hole::SelectableState);

                m_selected = hole;
            }
        }
    }

    if (movement != nullptr) {
        this->clearSelectable();
        m_selected = 0;

        Q_ASSERT(movement->first->state() == player2state(m_player));
        Q_ASSERT(movement->second->state() == Hole::EmptyState);

        movement->first->setState(Hole::EmptyState);
        movement->second->setState(player2state(m_player));

        if (isGameOver()==true)
            emit gameOver(m_player);
        else
            this->switchPlayer();

        delete movement;
    }
}

void Picaria::clearSelectable() {
    for (int id = 0; id < 13; id++) {
        Hole* hole = m_holes[id];
        if (hole->state() == Hole::SelectableState)
            hole->setState(Hole::EmptyState);
    }
}

QList<Hole*> Picaria::findSelectables(Hole* hole, int id) {
    QList<Hole*> list;
    switch(id){
        case 0:
            for(int i = 0; i < case0.size(); i++){
                hole = case0.value(i);
                if(hole->state() == Hole::EmptyState ||
                                     hole->state() == Hole::SelectableState){
                    list << case0.value(i);

                }
            }

        break;
        case 1:

            for(int i = 0; i < case1.size(); i++){
                hole = case1.value(i);
                if(hole->state() == Hole::EmptyState ||
                        hole->state() == Hole::SelectableState){
                    list << case1.value(i);

                }
            }
        break;
        case 2:
            for(int i = 0; i < case2.size(); i++){
                hole = case2.value(i);
                if(hole->state() == Hole::EmptyState ||
                        hole->state() == Hole::SelectableState){
                    list << case2.value(i);

                }
            }
        break;
        case 3:
            for(int i = 0; i < case3.size(); i++){
                hole = case3.value(i);
                if(hole->state() == Hole::EmptyState ||
                        hole->state() == Hole::SelectableState){
                    list << case3.value(i);

                }
            }
        break;
            case 4:
            for(int i = 0; i < case4.size(); i++){
                hole = case4.value(i);
                if(hole->state() == Hole::EmptyState ||
                        hole->state() == Hole::SelectableState){
                    list << case4.value(i);

                }
            }
        break;
        case 5:
            for(int i = 0; i < case5.size(); i++){
                hole = case5.value(i);
                if(hole->state() == Hole::EmptyState ||
                        hole->state() == Hole::SelectableState){
                    list << case5.value(i);
                }
            }
        break;
        case 6:
            for(int i = 0; i < case6.size(); i++){
                hole = case6.value(i);
                if(hole->state() == Hole::EmptyState ||
                        hole->state() == Hole::SelectableState){
                    list << case6.value(i);
                }
            }
        break;
        case 7:
            for(int i = 0; i < case7.size(); i++){
                hole = case7.value(i);
                if(hole->state() == Hole::EmptyState ||
                        hole->state() == Hole::SelectableState){
                    list << case7.value(i);
                }
            }
        break;
        case 8:
            for(int i = 0; i < case8.size(); i++){
                hole = case8.value(i);
                if(hole->state() == Hole::EmptyState ||
                        hole->state() == Hole::SelectableState){
                    list << case8.value(i);
                }
            }
        break;
        case 9:
            for(int i = 0; i < case9.size(); i++){
                hole = case9.value(i);
                if(hole->state() == Hole::EmptyState ||
                        hole->state() == Hole::SelectableState){
                    list << case9.value(i);                  
                }
            }
        break;
        case 10:
            for(int i = 0; i < case10.size(); i++){
                hole = case10.value(i);
                if(hole->state() == Hole::EmptyState ||
                        hole->state() == Hole::SelectableState){
                    list << case10.value(i);                    
                }
            }
        break;
        case 11:
            for(int i = 0; i < case11.size(); i++){
                hole = case11.value(i);
                if(hole->state() == Hole::EmptyState ||
                        hole->state() == Hole::SelectableState){
                    list << case11.value(i);                    
                }
            }
        break;
        case 12:
            for(int i = 0; i < case12.size(); i++){
                hole = case12.value(i);
                if(hole->state() == Hole::EmptyState ||
                        hole->state() == Hole::SelectableState){
                    list << case12.value(i);
                }
            }
        break;

    default:
    break;
    }
    return list;
}
void Picaria::setNeighbor(){

    for (int id = 0; id < 13  ; ++id) {
        Hole* hole = m_holes[id];
        hole->reset();

        switch (id) {
            case 0:
                if  (m_mode == Picaria::NineHoles){
                    case0.append(m_holes[1]);
                    case0.append(m_holes[5]);
                    case0.append(m_holes[6]);
                }else{
                    case0.append(m_holes[1]);
                    case0.append(m_holes[5]);
                    case0.append(m_holes[3]);
                }

                break;
            case 1:
                if  (m_mode == Picaria::NineHoles){
                    case1.append(m_holes[0]);
                    case1.append(m_holes[2]);
                    case1.append(m_holes[5]);
                    case1.append(m_holes[6]);
                    case1.append(m_holes[7]);
                }else{
                    case1.append(m_holes[0]);
                    case1.append(m_holes[2]);
                    case1.append(m_holes[3]);
                    case1.append(m_holes[6]);
                    case1.append(m_holes[4]);
                }
                break;
            case 2:
                if  (m_mode == Picaria::NineHoles){
                    case2.append(m_holes[1]);
                    case2.append(m_holes[6]);
                    case2.append(m_holes[7]);
                }else{
                    case2.append(m_holes[1]);
                    case2.append(m_holes[4]);
                    case2.append(m_holes[7]);
                }
                break;
            case 3:
                if  (m_mode == Picaria::ThirteenHoles){
                    case3.append(m_holes[0]);
                    case3.append(m_holes[1]);
                    case3.append(m_holes[5]);
                    case3.append(m_holes[6]);
                }
                break;
            case 4:
                if  (m_mode == Picaria::ThirteenHoles){
                    case4.append(m_holes[1]);
                    case4.append(m_holes[2]);
                    case4.append(m_holes[6]);
                    case4.append(m_holes[7]);
                }
                break;
            case 5:
                if  (m_mode == Picaria::NineHoles){
                    case5.append(m_holes[0]);
                    case5.append(m_holes[6]);
                    case5.append(m_holes[10]);
                    case5.append(m_holes[1]);
                    case5.append(m_holes[11]);
                }else{
                    case5.append(m_holes[0]);
                    case5.append(m_holes[3]);
                    case5.append(m_holes[6]);
                    case5.append(m_holes[8]);
                    case5.append(m_holes[10]);
                }
                break;
            case 6:
                if  (m_mode == Picaria::NineHoles){
                    case6.append(m_holes[1]);
                    case6.append(m_holes[5]);
                    case6.append(m_holes[7]);
                    case6.append(m_holes[11]);
                    case6.append(m_holes[0]);
                    case6.append(m_holes[12]);
                    case6.append(m_holes[2]);
                    case6.append(m_holes[10]);
                }else{
                    case6.append(m_holes[1]);
                    case6.append(m_holes[3]);
                    case6.append(m_holes[5]);
                    case6.append(m_holes[8]);
                    case6.append(m_holes[11]);
                    case6.append(m_holes[9]);
                    case6.append(m_holes[7]);
                    case6.append(m_holes[4]);
                }
                break;
            case 7:
                if  (m_mode == Picaria::NineHoles){
                    case7.append(m_holes[2]);
                    case7.append(m_holes[6]);
                    case7.append(m_holes[12]);
                    case7.append(m_holes[1]);
                    case7.append(m_holes[11]);
                }else{
                    case7.append(m_holes[2]);
                    case7.append(m_holes[4]);
                    case7.append(m_holes[6]);
                    case7.append(m_holes[9]);
                    case7.append(m_holes[12]);
                }
                break;
            case 8:
                if  (m_mode == Picaria::ThirteenHoles){
                    case8.append(m_holes[5]);
                    case8.append(m_holes[6]);
                    case8.append(m_holes[10]);
                    case8.append(m_holes[11]);
                }
                break;
            case 9:
                if  (m_mode == Picaria::ThirteenHoles){
                    case9.append(m_holes[6]);
                    case9.append(m_holes[7]);
                    case9.append(m_holes[11]);
                    case9.append(m_holes[12]);
                }
                break;
            case 10:
                if  (m_mode == Picaria::NineHoles){
                    case10.append(m_holes[5]);
                    case10.append(m_holes[6]);
                    case10.append(m_holes[11]);
                }else{
                    case10.append(m_holes[5]);
                    case10.append(m_holes[8]);
                    case10.append(m_holes[11]);
                }
                break;
            case 11:
                if  (m_mode == Picaria::NineHoles){
                    case11.append(m_holes[5]);
                    case11.append(m_holes[6]);
                    case11.append(m_holes[7]);
                    case11.append(m_holes[10]);
                    case11.append(m_holes[12]);
                }else{
                    case11.append(m_holes[10]);
                    case11.append(m_holes[8]);
                    case11.append(m_holes[6]);
                    case11.append(m_holes[9]);
                    case11.append(m_holes[12]);
                }
                break;
            case 12:
                if  (m_mode == Picaria::NineHoles){
                    case12.append(m_holes[7]);
                    case12.append(m_holes[6]);
                    case12.append(m_holes[11]);
                }else{
                    case12.append(m_holes[9]);
                    case12.append(m_holes[7]);
                    case12.append(m_holes[11]);
                }

                break;
            default:
                break;

        }
    }
}
void Picaria::reset() {
    case0.clear();
    case1.clear();
    case2.clear();
    case3.clear();
    case4.clear();
    case5.clear();
    case6.clear();
    case7.clear();
    case8.clear();
    case9.clear();
    case10.clear();
    case11.clear();
    case12.clear();

    for (int id = 0; id < 13  ; ++id) {
        Hole* hole = m_holes[id];
        hole->reset();

        switch (id) {
            case 3:
            case 4:
            case 8:
            case 9:
                hole->setVisible(m_mode == Picaria::ThirteenHoles);
                break;
            default:
                break;
        }
    }

    this->setNeighbor();
    m_player = Picaria::RedPlayer;
    m_phase = Picaria::DropPhase;
    m_dropCount = 0;
    m_selected = nullptr;
    this->updateStatusBar();

}

void Picaria::showAbout() {
    QMessageBox::information(this, tr("About"), tr("Picaria\n\nAlex Meireles Santos Almeida - alexmeirelesalmeida@hotmail.com\n\nVitor Theodoro Rocha Domingues - vitor-theodoro@hotmail.com\n"));
}

void Picaria::updateMode(QAction* action) {
    if (action == ui->action9holes)
        this->setMode(Picaria::NineHoles);
    else if (action == ui->action13holes)
        this->setMode(Picaria::ThirteenHoles);
    else
        Q_UNREACHABLE();
}

void Picaria::updateStatusBar() {
    QString player(m_player == Picaria::RedPlayer ? "vermelho" : "azul");
    QString phase(m_phase == Picaria::DropPhase ? "colocar" : "mover");

    ui->statusbar->showMessage(tr("Fase de %1: vez do jogador %2").arg(phase).arg(player));
}
void Picaria::showGameOver(Player player) {

    switch (player) {
        case Picaria::RedPlayer:
            QMessageBox::information(this, tr("Vencedor"), tr("Parabéns, o jogador vermelho venceu."));
            break;
        case Picaria::BluePlayer:
            QMessageBox::information(this, tr("Vencedor"), tr("Parabéns, o jogador azul venceu."));
            break;
        default:
            Q_UNREACHABLE();
    }
}
bool Picaria::isGameOver(){
    Hole* hole0 = m_holes[0];
    Hole* hole1 = m_holes[1];
    Hole* hole2 = m_holes[2];
    Hole* hole3 = m_holes[3];
    Hole* hole4 = m_holes[4];
    Hole* hole5 = m_holes[5];
    Hole* hole6 = m_holes[6];
    Hole* hole7 = m_holes[7];
    Hole* hole8 = m_holes[8];
    Hole* hole9 = m_holes[9];
    Hole* hole10 = m_holes[10];
    Hole* hole11 = m_holes[11];
    Hole* hole12 = m_holes[12];

    if  (m_mode == Picaria::NineHoles){
        if((hole0->state() == Hole::RedState && hole1->state() == Hole::RedState && hole2->state() == Hole::RedState) || (hole0->state() == Hole::BlueState && hole1->state() == Hole::BlueState && hole2->state() == Hole::BlueState)){
            return true;
        } else if((hole5->state() == Hole::RedState && hole6->state() == Hole::RedState && hole7->state() == Hole::RedState) || (hole5->state() == Hole::BlueState && hole6->state() == Hole::BlueState && hole7->state() == Hole::BlueState)){
            return true;
        }
        else if((hole10->state() == Hole::RedState && hole11->state() == Hole::RedState && hole12->state() == Hole::RedState) || (hole10->state() == Hole::BlueState && hole11->state() == Hole::BlueState && hole12->state() == Hole::BlueState)){
            return true;
        }
        else if((hole0->state() == Hole::RedState && hole5->state() == Hole::RedState && hole10->state() == Hole::RedState) || (hole0->state() == Hole::BlueState && hole5->state() == Hole::BlueState && hole10->state() == Hole::BlueState)){
            return true;
        }
        else if((hole1->state() == Hole::RedState && hole6->state() == Hole::RedState && hole11->state() == Hole::RedState) || (hole1->state() == Hole::BlueState && hole6->state() == Hole::BlueState && hole11->state() == Hole::BlueState)){
            return true;
        }
        else if((hole2->state() == Hole::RedState && hole7->state() == Hole::RedState && hole12->state() == Hole::RedState) || (hole2->state() == Hole::BlueState && hole7->state() == Hole::BlueState && hole12->state() == Hole::BlueState)){
            return true;
        }
        else if((hole0->state() == Hole::RedState && hole6->state() == Hole::RedState && hole12->state() == Hole::RedState) || (hole0->state() == Hole::BlueState && hole6->state() == Hole::BlueState && hole12->state() == Hole::BlueState)){
            return true;
        }
        else if((hole2->state() == Hole::RedState && hole6->state() == Hole::RedState && hole10->state() == Hole::RedState) || (hole2->state() == Hole::BlueState && hole6->state() == Hole::BlueState && hole10->state() == Hole::BlueState)){
            return true;
        }


    } else if(m_mode == Picaria::ThirteenHoles){
        if((hole0->state() == Hole::RedState && hole1->state() == Hole::RedState && hole2->state() == Hole::RedState) || (hole0->state() == Hole::BlueState && hole1->state() == Hole::BlueState && hole2->state() == Hole::BlueState)){
            return true;
        } else if((hole5->state() == Hole::RedState && hole6->state() == Hole::RedState && hole7->state() == Hole::RedState) || (hole5->state() == Hole::BlueState && hole6->state() == Hole::BlueState && hole7->state() == Hole::BlueState)){
            return true;
        }
        else if((hole10->state() == Hole::RedState && hole11->state() == Hole::RedState && hole12->state() == Hole::RedState) || (hole10->state() == Hole::BlueState && hole11->state() == Hole::BlueState && hole12->state() == Hole::BlueState)){
            return true;
        }
        else if((hole0->state() == Hole::RedState && hole5->state() == Hole::RedState && hole10->state() == Hole::RedState) || (hole0->state() == Hole::BlueState && hole5->state() == Hole::BlueState && hole10->state() == Hole::BlueState)){
            return true;
        }
        else if((hole1->state() == Hole::RedState && hole6->state() == Hole::RedState && hole11->state() == Hole::RedState) || (hole1->state() == Hole::BlueState && hole6->state() == Hole::BlueState && hole11->state() == Hole::BlueState)){
            return true;
        }
        else if((hole2->state() == Hole::RedState && hole7->state() == Hole::RedState && hole12->state() == Hole::RedState) || (hole2->state() == Hole::BlueState && hole7->state() == Hole::BlueState && hole12->state() == Hole::BlueState)){
            return true;
        }
        else if((hole0->state() == Hole::RedState && hole3->state() == Hole::RedState && hole6->state() == Hole::RedState) || (hole0->state() == Hole::BlueState && hole3->state() == Hole::BlueState && hole6->state() == Hole::BlueState)){
            return true;
        }
        else if((hole1->state() == Hole::RedState && hole3->state() == Hole::RedState && hole5->state() == Hole::RedState) || (hole1->state() == Hole::BlueState && hole3->state() == Hole::BlueState && hole5->state() == Hole::BlueState)){
            return true;
        }
        else if((hole5->state() == Hole::RedState && hole8->state() == Hole::RedState && hole11->state() == Hole::RedState) || (hole5->state() == Hole::BlueState && hole8->state() == Hole::BlueState && hole11->state() == Hole::BlueState)){
            return true;
        }
        else if((hole10->state() == Hole::RedState && hole8->state() == Hole::RedState && hole6->state() == Hole::RedState) || (hole10->state() == Hole::BlueState && hole8->state() == Hole::BlueState && hole6->state() == Hole::BlueState)){
            return true;
        }
        else if((hole1->state() == Hole::RedState && hole4->state() == Hole::RedState && hole7->state() == Hole::RedState) || (hole1->state() == Hole::BlueState && hole4->state() == Hole::BlueState && hole7->state() == Hole::BlueState)){
            return true;
        }
        else if((hole2->state() == Hole::RedState && hole4->state() == Hole::RedState && hole6->state() == Hole::RedState) || (hole2->state() == Hole::BlueState && hole4->state() == Hole::BlueState && hole6->state() == Hole::BlueState)){
            return true;
        }
        else if((hole6->state() == Hole::RedState && hole9->state() == Hole::RedState && hole12->state() == Hole::RedState) || (hole6->state() == Hole::BlueState && hole9->state() == Hole::BlueState && hole12->state() == Hole::BlueState)){
            return true;
        }
        else if((hole7->state() == Hole::RedState && hole9->state() == Hole::RedState && hole11->state() == Hole::RedState) || (hole7->state() == Hole::BlueState && hole9->state() == Hole::BlueState && hole11->state() == Hole::BlueState)){
            return true;
        }
        else if((hole3->state() == Hole::RedState && hole6->state() == Hole::RedState && hole9->state() == Hole::RedState) || (hole3->state() == Hole::BlueState && hole6->state() == Hole::BlueState && hole9->state() == Hole::BlueState)){
            return true;
        }
        else if((hole8->state() == Hole::RedState && hole6->state() == Hole::RedState && hole4->state() == Hole::RedState) || (hole8->state() == Hole::BlueState && hole6->state() == Hole::BlueState && hole4->state() == Hole::BlueState)){
            return true;
        }

    }

return 0;
}
