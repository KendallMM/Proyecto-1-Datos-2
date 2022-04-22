#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QByteArray>
#include <QBuffer>
#include <vector>
#include <string>
#include <iostream>
#include <string.h>
#include <vector>
#include <fstream>
#include<stdlib.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), server(this)
{
    ui->setupUi(this);
    server.listen(QHostAddress::Any, 9999);
    connect(timer, SIGNAL(timeout()), this, SLOT(actualizarEstado()));
    // Connect to slot
    connect(&server, SIGNAL(newConnection()),this,SLOT(onNewConnection()));
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QPixmap qp("/home/kendall/Descargas/Emparejados-master/01.png");
    qp.save(&buffer, "PNG");
    QString encoded = buffer.data().toBase64();
    QPixmap image;
    image.loadFromData(QByteArray::fromBase64(encoded.toLocal8Bit()));
    ui->imagen->setPixmap(image);
    inicializarJuego();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::onNewConnection()
{
    QTcpSocket *clientSocket = server.nextPendingConnection();
    // Ready read
        connect(clientSocket, SIGNAL(readyRead()),this,SLOT(onReadyRead()));

    // State Changed
        connect(clientSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this,SLOT(onStateChanged(QAbstractSocket::SocketState)));

    sockets.push_back(clientSocket);
    QString texto = "tiempo";
    for (QTcpSocket* socket : sockets){
        socket->write(QByteArray::fromStdString(texto.toStdString()));;
    }
    juegoIniciado=true;
}
void MainWindow::onStateChanged(QAbstractSocket::SocketState state)
{
    if (state == QAbstractSocket::UnconnectedState){
        QTcpSocket* s = static_cast<QTcpSocket*>(QObject::sender());
        sockets.removeOne(s);
    }
}


void  MainWindow::onReadyRead()
{
    QTcpSocket* s = static_cast<QTcpSocket*>(QObject::sender());
    tarjetaActual = s->readAll();
    inicializarJugada();
}
void MainWindow::inicializarJugada(){
    if(!jugadaIniciada){
        tarjetaAnterior=tarjetaActual;
        jugadaIniciada=true;
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        QString posicion = QString(tarjetaActual);
        QString imgn = reparto[posicion];
        QString path = "/home/kendall/Descargas/Emparejados-master/";
        QString image = path.append(imgn).append(".png");
        QPixmap qp(image);
        qp.save(&buffer, "PNG");
        ui->imagen->setPixmap(qp);
        for (QTcpSocket* socket : sockets){
            socket ->write(buffer.data().toBase64());
        }
    }
    else {
        definirResultadoParcial();

        jugadaIniciada=false;
    }
}

void MainWindow::inicializarJuego(){
    //start turn
    jugadaIniciada=false;
    juegoIniciado=false;

    //Set score
    puntaje=0;
    ui->lblPuntaje->setText(QString::number(puntaje));;

    //Set matches counter
    parejasRestantes=15;

    //Set clock for countdown
    time.setHMS(0,0,0);

    //Initialize countdown
    ui->cronometro->setText(time.toString("m:ss"));

    // Start timer with a value of 1000 milliseconds, indicating that it will time out every second.
    timer->start(1000);

    //Randomly sort tiles in container
    mezclar(tarjetas);

    //Grab pairs of tiles and bind the name of an image file to each pair
    repartir(tarjetas, reparto);

    //enable every tile and reset its image
    QList<QPushButton *> botones =  ui->centralwidget->findChildren<QPushButton*>();
    foreach (QPushButton* b, botones) {
        b->setEnabled(true);
        b->setStyleSheet("#" + b->objectName() + "{ }");
    }
}

void MainWindow::repartir(QVector<QString> &tarjetas, QHash<QString, QString> &reparto){
    auto iterador=tarjetas.begin();
    for (int i=1; i<=15; i++){
        QString file_name=QString::number(i);
        reparto[(*iterador)]=file_name;
        iterador++;
        reparto[(*iterador)]=file_name;
        iterador++;
    }
    for (int i=1; i<31; i++){
        QString posicion = "tarjeta";
        if(i<10){
            posicion.append("0");
            posicion.append(QString(QString::number(i)));
        }else{
            posicion.append(QString(QString::number(i)));
        }
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        QString imgn = reparto[posicion];
        QString path = "/home/kendall/Descargas/Emparejados-master/";
        QString image = path.append(imgn).append(".png");
        QPixmap qp(image);
        qp.save(&buffer, "PNG");
        std::string text = buffer.data().toBase64().constData();
        Tarjetas.push_back(text);
    }

}
void MainWindow::mezclar(QVector<QString> &tarjetas){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle (tarjetas.begin(), tarjetas.end(), std::default_random_engine(seed));
}

void MainWindow::definirResultadoParcial(){
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QString posicion = QString(tarjetaActual);
    QString imgn = reparto[posicion];
    QString path = "/home/kendall/Descargas/Emparejados-master/";
    QString image = path.append(imgn).append(".png");
    QPixmap qp(image);
    qp.save(&buffer, "PNG");
    QBuffer buffer2;
    buffer2.open(QIODevice::WriteOnly);
    QString posicion2 = QString(tarjetaAnterior);
    QString imgn2 = reparto[posicion2];
    QString path2 = "/home/kendall/Descargas/Emparejados-master/";
    QString image2 = path2.append(imgn2).append(".png");
    QPixmap qp2(image2);
    qp2.save(&buffer2, "PNG");
    //check if there is a match (the current tile matches the previous tile in the turn)
    if (buffer.data().toBase64()==buffer2.data().toBase64()){
        enviarTarjeta();
        QTimer::singleShot(1000, this, SLOT(enviarPuntos()));
        puntaje+=15;
        ui->lblPuntaje->setText(QString::number(puntaje));
        parejasRestantes--;

        //if there is a match, find out if all tiles have been matched.
        definirResultadoFinal();
    }
    else{
        enviarTarjeta();
        QTimer::singleShot(1000, this, SLOT(reiniciarTarjetas()));
        puntaje-=5;
        ui->lblPuntaje->setText(QString::number(puntaje));

    }
}
void MainWindow::enviarPuntos(){
    QString texto = "conservar";
    for (QTcpSocket* socket : sockets){
        socket->write(QByteArray::fromStdString(texto.toStdString()));;
    }
}
void MainWindow::reiniciarTarjetas(){
    QString texto = "desechar";
    for (QTcpSocket* socket : sockets){
        socket->write(QByteArray::fromStdString(texto.toStdString()));;
    }
}
void MainWindow::enviarTarjeta(){
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QString posicion = QString(tarjetaActual);
    QString imgn = reparto[posicion];
    QString path = "/home/kendall/Descargas/Emparejados-master/";
    QString image = path.append(imgn).append(".png");
    QPixmap qp(image);
    qp.save(&buffer, "PNG");
    ui->imagen->setPixmap(qp);
    for (QTcpSocket* socket : sockets){
        socket ->write(buffer.data().toBase64());
    }
}

void MainWindow::definirResultadoFinal(){
    msgBox.setWindowTitle("Juego terminado");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setEscapeButton(QMessageBox::No);

    if (parejasRestantes==0){
        timer->stop();
        msgBox.setText("¡Ganaste! Puntaje final: " + QString::number(puntaje) + "\nVolver a jugar?");
        if (QMessageBox::Yes == msgBox.exec()){
            inicializarJuego();
        }
        else{
            QCoreApplication::quit();
        }
    }
}
void MainWindow::actualizarCronometro(){
    if (juegoIniciado){
    time=time.addSecs(+1);
    ui->cronometro->setText(time.toString("m:ss"));
    }
}
void MainWindow::actualizarEstado(){
    actualizarCronometro();
    definirResultadoFinal();
}


