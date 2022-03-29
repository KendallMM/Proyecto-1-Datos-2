#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), server(this)
{
    qDebug() << "Constructor";
    ui->setupUi(this);
    server.listen(QHostAddress::Any, 9999);
    // Connect to slot
    connect(&server, SIGNAL(newConnection()),this,SLOT(onNewConnection()));
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
    QByteArray data = s->readAll();
    ui->textEdit->append(QString::fromStdString(data.toStdString()));
    qDebug() << QString::fromStdString(data.toStdString());
    for (QTcpSocket* socket : sockets){
        socket ->write(data);
    }
}
