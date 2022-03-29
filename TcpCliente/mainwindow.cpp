#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      socket(this)
{
    ui->setupUi(this);
    socket.connectToHost(QHostAddress("127.0.0.1"), 9999);
    connect(&socket,SIGNAL(readyRead()),this,SLOT(onReadyRead()));

    connect(ui->sendButton,SIGNAL(pressed()),this,SLOT(onSendButtonPressed()));
    connect(ui->loginButton,SIGNAL(pressed()),this,SLOT(onLoginButtonPressed()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onReadyRead()
{
    QByteArray data = socket.readAll();
    qDebug() << data;
    ui->textEdit->append(QString::fromStdString(data.toStdString()));

}
void MainWindow::onSendButtonPressed()
{
    qDebug() << "onSendButtonPressed";
    QString text = ui->username->text()+": "+ui->lineEdit->text();
    ui->lineEdit->setText("");
    socket.write(QByteArray::fromStdString(text.toStdString()));
}
void MainWindow::onLoginButtonPressed()
{
    qDebug() << "onLoginButtonPressed";
    ui->sendButton->setEnabled(true);
    ui->loginButton->setEnabled(false);
    ui->username->setEnabled(false);
}
