#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QTime>
#include <QString>
#include <QVector>
#include <QHash>
#include <QPushButton>
#include <QMessageBox>
#include <random>
#include <QInputDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTimer *timer=new QTimer();//Crea un temporizador.
    QTime time;//Crea un contador
    QVector<QString> tarjetas{"tarjeta01", "tarjeta02", "tarjeta03", "tarjeta04",
                              "tarjeta05", "tarjeta06", "tarjeta07", "tarjeta08",
                              "tarjeta09", "tarjeta10", "tarjeta11", "tarjeta12",
                              "tarjeta13", "tarjeta14", "tarjeta15", "tarjeta16",
                              "tarjeta17", "tarjeta18", "tarjeta19", "tarjeta20",
                              "tarjeta21", "tarjeta22", "tarjeta23", "tarjeta24",
                              "tarjeta25", "tarjeta26", "tarjeta27", "tarjeta28",
                              "tarjeta29", "tarjeta30"};
    QHash<QString, QString> reparto;
    int puntaje=0;
    int puntaje2=0;
    bool jugador1;
    bool jugadaIniciada;
    bool juegoIniciado;
    QPushButton* tarjetaAnterior;
    QPushButton* tarjetaActual;
    int parejasRestantes;
    QMessageBox msgBox;
    QString p1name = QInputDialog::getText(this,"Jugador #1","Introduce tu nombre");
    QString p2name = QInputDialog::getText(this,"Jugador #2","Introduce tu nombre");

public slots:
    void actualizarCronometro();
    void onReadyRead(QPushButton* tarjeta, QByteArray data);
    void onReading();
    void onSendButtonPressed();
    void tarjetaDescubierta();
    void definirResultadoFinal();
    void actualizarEstado();
    void inicializarJuego();
    void waitServer();
    void enviarPosicion();



private:
    Ui::MainWindow *ui;
    QTcpSocket socket;
};
#endif // MAINWINDOW_H
