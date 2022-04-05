#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QByteArray>
#include <QBuffer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), server(this)
{
    qDebug() << "Constructor";
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
    qDebug() << "onNewConnection";
    QTcpSocket *clientSocket = server.nextPendingConnection();
    // Ready read
        connect(clientSocket, SIGNAL(readyRead()),this,SLOT(onReadyRead()));
    // State Changed
        connect(clientSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this,SLOT(onStateChanged(QAbstractSocket::SocketState)));

    sockets.push_back(clientSocket);
}


void MainWindow::onStateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << "onStateChanged";
    if (state == QAbstractSocket::UnconnectedState){
        QTcpSocket* s = static_cast<QTcpSocket*>(QObject::sender());
        sockets.removeOne(s);
    }
}


void  MainWindow::onReadyRead()
{
    qDebug() << "onReadyRead";

    QTcpSocket* s = static_cast<QTcpSocket*>(QObject::sender());
    tarjetaActual = s->readAll();
    qDebug() << QString::fromStdString(tarjetaActual.toStdString());
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QString posicion = QString(tarjetaActual);
    QString imgn = reparto[posicion];
    QString path = "/home/kendall/Descargas/Emparejados-master/";
    QString image = path.append(imgn).append(".png");
    qDebug() << image;
    QPixmap qp(image);
    qp.save(&buffer, "PNG");
    ui->imagen->setPixmap(qp);
    for (QTcpSocket* socket : sockets){
        socket ->write(buffer.data().toBase64());
    }
    if(!jugadaIniciada){
        tarjetaAnterior=tarjetaActual;
        jugadaIniciada=true;
    }
    else {
        definirResultadoParcial();

        jugadaIniciada=false;
    }

}
void MainWindow::inicializarJuego(){
    qDebug("inicializarJuego");
    //start turn
    jugadaIniciada=false;

    //Set score
    puntaje=0;
    ui->lblPuntaje->setText(QString::number(puntaje));;

    //Set matches counter
    parejasRestantes=30;

    //Set clock for countdown
    time.setHMS(0,1,0);

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
    qDebug("resarte");
    auto iterador=tarjetas.begin();
    for (int i=1; i<=15; i++){
        QString file_name=QString::number(i);
        reparto[(*iterador)]=file_name;
        iterador++;
        reparto[(*iterador)]=file_name;
        iterador++;
    }
}
void MainWindow::mezclar(QVector<QString> &tarjetas){
    qDebug("mezcla");
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle (tarjetas.begin(), tarjetas.end(), std::default_random_engine(seed));
}
void MainWindow::definirResultadoParcial(){
    qDebug("parseresult");
    //check if there is a match (the current tile matches the previous tile in the turn)
    if (reparto[tarjetaActual]==reparto[tarjetaAnterior]){
        puntaje+=15;
        ui->lblPuntaje->setText(QString::number(puntaje));
        parejasRestantes--;

        //if there is a match, find out if all tiles have been matched.
        definirResultadoFinal();
    }
    else{
        puntaje-=5;
        ui->lblPuntaje->setText(QString::number(puntaje));

        //disable the whole tile section so no tiles can be turned during the 1-second "memorizing period"

        //if there is no match, let user memorize tiles and after 1 second hide tiles from current turn so they can be used on another turn
        QTimer::singleShot(1000, this, SLOT(reiniciarTarjetas()));
    }
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
void MainWindow::actualizarCronometro(){
    qDebug("actCrono");
    time=time.addSecs(-1);
    ui->cronometro->setText(time.toString("m:ss"));
}
void MainWindow::actualizarEstado(){
    qDebug("ActEst");
    actualizarCronometro();
    definirResultadoFinal();
}


