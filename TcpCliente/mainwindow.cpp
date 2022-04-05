#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      socket(this)
{

    ui->setupUi(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(actualizarEstado()));
    socket.connectToHost(QHostAddress("127.0.0.1"), 9999);
    connect(&socket,SIGNAL(readyRead()),this,SLOT(onReading()));

    connect(ui->sendButton,SIGNAL(pressed()),this,SLOT(onSendButtonPressed()));
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
    onReadyRead(tarjetaActual, data);
}
void MainWindow::onReadyRead(QPushButton* tarjeta, QByteArray data)
{
    QString encoded = data;
    QPixmap image;
    image.loadFromData(QByteArray::fromBase64(encoded.toLocal8Bit()));
    tarjeta->setText("");
    tarjeta->setIcon(image);
    tarjeta->setIconSize(tarjeta->rect().size());
    tarjeta->blockSignals(true);
    ui->textEdit->append(QString::fromStdString(data.toStdString()));
}
void MainWindow::onSendButtonPressed()
{
    qDebug() << "onSendButtonPressed";
    QString text = ui->lineEdit->text();
    ui->lineEdit->setText("");
    socket.write(QByteArray::fromStdString(text.toStdString()));
}
void MainWindow::tarjetaDescubierta()
{
    qDebug("tarjetadesc");
    tarjetaActual=qobject_cast<QPushButton*>(sender());
    QString texto = tarjetaActual->objectName();
    socket.write(QByteArray::fromStdString(texto.toStdString()));
}
void MainWindow::mezclar(QVector<QString> &tarjetas){
    qDebug("mezcla");
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle (tarjetas.begin(), tarjetas.end(), std::default_random_engine(seed));
}
void MainWindow::inicializarJuego(){
    qDebug() << "inicializarJuego";
    //start turn
    jugadaIniciada=false;

    //Set score
    puntaje=0;
    ui->lblPuntaje->setText(QString::number(puntaje));;

    //Set matches counter
    parejasRestantes=15;

    //Set clock for countdown
    time.setHMS(0,1,0);

    //Initialize countdown
    ui->cronometro->setText(time.toString("m:ss"));

    // Start timer with a value of 1000 milliseconds, indicating that it will time out every second.
    timer->start(1000);

    //Randomly sort tiles in container
    mezclar(tarjetas);

    //enable tiles frame
    ui->frame->setEnabled(true);

    //enable every tile and reset its image
    QList<QPushButton *> botones =  ui->centralwidget->findChildren<QPushButton*>();
    foreach (QPushButton* b, botones) {
        b->setEnabled(true);
        b->setStyleSheet("#" + b->objectName() + "{ }");
    }
}
void MainWindow::actualizarCronometro(){
    qDebug("actCrono");
    time=time.addSecs(-1);
    ui->cronometro->setText(time.toString("m:ss"));
}
void MainWindow::definirResultadoFinal(){
    qDebug("result");
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
    else{
        if (time.toString()=="00:00:00"){
            timer->stop();
            ui->frame->setEnabled(false);
            msgBox.setText("Perdiste ;( \n¿Volver a jugar?");
            if (QMessageBox::Yes == msgBox.exec()){
                inicializarJuego();
            }
            else{
                QCoreApplication::quit();
            }
        }
    }
}
void MainWindow::actualizarEstado(){
    qDebug("ActEst");
    actualizarCronometro();
    definirResultadoFinal();
}
