#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QTime>
#include <QString>
#include <QVector>
#include <QHash>
#include <QPushButton>
#include <QMessageBox>
#include <random>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTimer *timer=new QTimer();
    QTime time;
    QVector<QString> tarjetas{"tarjeta01", "tarjeta02", "tarjeta03", "tarjeta04",
                              "tarjeta05", "tarjeta06", "tarjeta07", "tarjeta08",
                              "tarjeta09", "tarjeta10", "tarjeta11", "tarjeta12",
                              "tarjeta13", "tarjeta14", "tarjeta15", "tarjeta16",
                              "tarjeta17", "tarjeta18", "tarjeta19", "tarjeta20",
                              "tarjeta21", "tarjeta22", "tarjeta23", "tarjeta24",
                              "tarjeta25", "tarjeta26", "tarjeta27", "tarjeta28",
                              "tarjeta29", "tarjeta30"};
    QHash<QString, QString> reparto;
    bool jugadaIniciada;
    QByteArray tarjetaAnterior;
    QByteArray tarjetaActual;
    QString vector;
    int parejasRestantes;
    QMessageBox msgBox;
    int puntaje=0;

public slots:
    void onNewConnection();
    void onReadyRead();
    void onStateChanged(QAbstractSocket::SocketState state);
    void repartir(QVector<QString> &tarjetas, QHash<QString, QString> &reparto);
    void mezclar(QVector<QString> &tarjetas);
    void definirResultadoParcial();
    void definirResultadoFinal();
    void actualizarCronometro();
    void actualizarEstado();

    void inicializarJuego();


private:
    Ui::MainWindow *ui;
    QTcpServer server;
    QList<QTcpSocket*> sockets;
};
#endif // MAINWINDOW_H
