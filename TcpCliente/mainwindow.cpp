#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include <QInputDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      socket(this)
{
    ui->setupUi(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(actualizarEstado()));
    socket.connectToHost(QHostAddress("127.0.0.1"), 9999);
    connect(&socket,SIGNAL(readyRead()),this,SLOT(onReading()));
    connect(ui->tarjeta01, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta02, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta03, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta04, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta05, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta06, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta07, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta08, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta09, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta10, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta11, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta12, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta13, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta14, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta15, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta16, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta17, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta18, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta19, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta20, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta21, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta22, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta23, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta24, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta25, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta26, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta27, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta28, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta29, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    connect(ui->tarjeta30, SIGNAL(clicked()), this, SLOT(tarjetaDescubierta()));
    inicializarJuego();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::onReading()
{
    QByteArray data = socket.readAll();
    if(data=="desechar"){
        //return tiles from current turn to the default state (remove backgrounds)
        QPixmap image("/home/kendall/Descargas/Emparejados-master/16.png");
        tarjetaAnterior->setIcon(image);
        tarjetaActual->setText("");
        tarjetaActual->setIcon(image);
        tarjetaActual->setIconSize(tarjetaActual->rect().size());

        //re-enable both tiles so they can be used on another turn
        tarjetaActual->blockSignals(false);
        tarjetaAnterior->blockSignals(false);
        if(jugador1){
            puntaje-=5;
            ui->lblPuntaje->setText(QString::number(puntaje));
            jugador1=false;
            ui->checkBoxp1->setChecked(false);
            ui->checkBoxp2->setChecked(true);
            }
        else{
            puntaje2-=5;
            ui->lblPuntaje2->setText(QString::number(puntaje2));
            jugador1=true;
            ui->checkBoxp1->setChecked(true);
            ui->checkBoxp2->setChecked(false);
            }
        }
    else if(data=="conservar"){
        if(jugador1){
            puntaje+=15;
            ui->lblPuntaje->setText(QString::number(puntaje));
            }
        else{
            puntaje2+=15;
            ui->lblPuntaje2->setText(QString::number(puntaje2));
            }
    }
    else if(data=="tiempo"){
        juegoIniciado=true;
    }
    else{
    onReadyRead(tarjetaActual, data);
    }

}
void MainWindow::onReadyRead(QPushButton* tarjeta, QByteArray data)
{
    QString encoded = data;
    QPixmap image;
    image.loadFromData(QByteArray::fromBase64(encoded.toLocal8Bit()));
    tarjeta->setText("");
    tarjeta->setIcon(image);
    tarjeta->setIconSize(tarjeta->rect().size());
}
void MainWindow::onSendButtonPressed()
{
    QString tiempo = "tiempo";
    socket.write(QByteArray::fromStdString(tiempo.toStdString()));;
}
void MainWindow::tarjetaDescubierta()
{
    enviarPosicion();
    if(!jugadaIniciada){
        tarjetaAnterior=tarjetaActual;
        jugadaIniciada=true;
    }
    else {
        jugadaIniciada=false;
        QList<QPushButton *> botones =  ui->centralwidget->findChildren<QPushButton*>();
        foreach (QPushButton* b, botones) {
            b->blockSignals(true);;
        }
        QTimer::singleShot(1000, this, SLOT(waitServer()));
    }
}
void MainWindow::enviarPosicion(){
    tarjetaActual=qobject_cast<QPushButton*>(sender());
    tarjetaActual->blockSignals(true);
    QString texto = tarjetaActual->objectName();
    socket.write(QByteArray::fromStdString(texto.toStdString()));
}
void MainWindow::waitServer(){
    QList<QPushButton *> botones =  ui->centralwidget->findChildren<QPushButton*>();
    foreach (QPushButton* b, botones) {
        b->blockSignals(false);;
    }
}
void MainWindow::inicializarJuego(){
    ui->p1->setText(p1name);
    ui->p2->setText(p2name);
    //start turn
    jugadaIniciada=false;
    juegoIniciado=false;
    jugador1=true;
    ui->checkBoxp1->setChecked(true);
    ui->checkBoxp2->setChecked(false);

    //Set matches counter
    parejasRestantes=15;

    //Set clock for countdown
    time.setHMS(0,0,0);

    //Initialize countdown
    ui->cronometro->setText(time.toString("m:ss"));

    // Start timer with a value of 1000 milliseconds, indicating that it will time out every second.
    timer->start(1000);


    //enable tiles frame
    ui->frame->setEnabled(true);

    //enable every tile and reset its image
    QPixmap image("/home/kendall/Descargas/Emparejados-master/16.png");
    QList<QPushButton *> botones =  ui->centralwidget->findChildren<QPushButton*>();
    foreach (QPushButton* b, botones) {
        b->setEnabled(true);
        b->setText("");
        b->setIcon(image);
        b->setIconSize(b->rect().size());

    }
}
void MainWindow::actualizarCronometro(){
    if (juegoIniciado){
    time=time.addSecs(+1);
    ui->cronometro->setText(time.toString("m:ss"));
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
void MainWindow::actualizarEstado(){
    actualizarCronometro();
    definirResultadoFinal();
}
