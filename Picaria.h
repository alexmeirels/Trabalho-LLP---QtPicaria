#ifndef PICARIA_H
#define PICARIA_H

#include <QMainWindow>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui {
    class Picaria;
}
QT_END_NAMESPACE

class Hole;

class Picaria : public QMainWindow {
    Q_OBJECT
    Q_PROPERTY(Picaria::Mode mode READ mode WRITE setMode NOTIFY modeChanged)

public:
    enum Mode {
        NineHoles,
        ThirteenHoles
    };
    Q_ENUM(Mode)

    enum Player {
        RedPlayer,
        BluePlayer
    };
    Q_ENUM(Player)

    enum Phase {
        DropPhase,
        MovePhase
    };
    Q_ENUM(Phase)

    Picaria(QWidget *parent = nullptr);
    virtual ~Picaria();

    Picaria::Mode mode() const { return m_mode; }
    void setMode(Picaria::Mode mode);

signals:
    void modeChanged(Picaria::Mode mode);
    void gameOver(Player player);

private:
    Ui::Picaria *ui;
    Hole* m_holes[13];
    Mode m_mode;
    Player m_player;
    Phase m_phase;
    int m_dropCount;
    Hole* m_selected;

    QList<Hole*> case0;
    QList<Hole*> case1;
    QList<Hole*> case2;
    QList<Hole*> case3;
    QList<Hole*> case4;
    QList<Hole*> case5;
    QList<Hole*> case6;
    QList<Hole*> case7;
    QList<Hole*> case8;
    QList<Hole*> case9;
    QList<Hole*> case10;
    QList<Hole*> case11;
    QList<Hole*> case12;

    void setNeighbor(); // Conferir depois os parametros.
    bool isGameOver();

    void drop(Hole* hole);
    void move(Hole* hole,int id);

    void clearSelectable();
    QList<Hole*> findSelectables(Hole* hole, int id);

    void switchPlayer();

private slots:
    void play(int id);
    void reset();

    void showAbout();
    void showGameOver(Player player);
    void updateMode(QAction* action);
    void updateStatusBar();

};

#endif // PICARIA_H
