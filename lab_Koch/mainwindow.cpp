#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    count_client = 0;
    ws.onopen = [&](const WebSocketChannelPtr& channel, const std::string& req) {
        m.lock();
        count_client++;
        this->client.insert(count_client, channel);
        emit this->addNewclient(count_client);
        m.unlock();
    };

    ws.onmessage = [&](const WebSocketChannelPtr& channel, const std::string& msg) {
        m.lock();
        this->res += QString(msg.c_str()).toInt();
        count_client_dane++;
        if (client.size() == count_client_dane) {
            emit this->Dane();
        }
        m.unlock();
    };

    ws.onclose = [&](const WebSocketChannelPtr& channel) {
        m.lock();
        emit this->deleteNewclient(client.key(channel));
        this->client.remove(this->client.key(channel));
        m.unlock();
    };

    t = new std::thread([&](){
        server.registerWebSocketService(&ws);
        server.setPort(8999);
        server.setThreadNum(std::thread::hardware_concurrency() / 2);
        server.run();
    });

    t->detach();

    connect(this, &MainWindow::addNewclient, this, &MainWindow::addNewclients);
    connect(this, &MainWindow::deleteNewclient, this, &MainWindow::deleteNewclients);
    connect(this, &MainWindow::Dane, this, &MainWindow::daneClients);

    this->ui->textBrowser->setText("Ожидание выполнения...");
}

void MainWindow::daneClients()
{
    int time = this->time.elapsed();
    QString res = "\n\nРаспределенные вычисления:";
    res += "\nВремя выполнения : " + QString::number(time)+ " мс";
    res += "\nРезультат: " + QString::number(this->res);

    this->ui->textBrowser->append(res);
}

void MainWindow::addNewclients(int index)
{
    this->ui->listWidget->addItem("Клиент " + QString::number(index));
}

void MainWindow::deleteNewclients(int index)
{
    auto item = this->ui->listWidget->findItems("Клиент " + QString::number(index), Qt::MatchExactly);
    for (auto it : item)
        this->ui->listWidget->removeItemWidget(it);
}

MainWindow::~MainWindow()
{
    server.stop();
    delete t;
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this,"Open file",QString(),"ALL (*);;");
    if(!file_name.isNull()) {
        name_file_prog = file_name;
        this->ui->checkBox_2->setChecked(true);
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this,"Open file",QString(),"ALL (*);;");
    if(!file_name.isNull()) {
        name_file_data = file_name;
        this->ui->checkBox->setChecked(true);
    }
}

void MainWindow::sendClientProg()
{
    QFile file(this->name_file_prog);
    QByteArray data;

    if (!file.open(QIODevice::ReadOnly))
           return;

    data = file.readAll();
    for (auto it: this->client) {
        it->send("prog " + data.toBase64().toStdString());
    }
}

void MainWindow::sendClientArg(QString arg)
{
    for (auto it: this->client) {
        it->send("arg " + arg.toUtf8().toBase64().toStdString());
    }
}

void MainWindow::sendClientRun()
{
    for (auto it: this->client) {
        it->send("run ");
    }
}

void MainWindow::sendClientData() {
    QFile file(this->name_file_data);
    QByteArray data;

    if (!file.open(QIODevice::ReadOnly))
           return;

    data = file.readAll();
    QString str_data(data);
    QStringList list_data;
    QStringList str = str_data.split('\n');
    for (auto string : str) {
        list_data += string.split(' ');
    }

    int pos = 0;
    int j = 1;
    int step = list_data.size() / this->client.size();
    for (auto it: this->client) {
        QString send_data = "";

        for (int i = pos; i < ((j == client.size())?list_data.size():pos + step); ++i) {
            send_data += list_data[i] + " ";
        }

        it->send("data " + send_data.toUtf8().toBase64().toStdString());
        pos += step;
        j++;
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if (this->ui->checkBox->isChecked() && this->ui->checkBox_2->isChecked()) {
            QProcess proc;
            QStringList arg;
            if (!this->ui->lineEdit->text().isEmpty()){
                arg = this->ui->lineEdit->text().split(' ');
            }
            arg << name_file_data;

            this->time.start();
            proc.start(this->name_file_prog, arg);
            proc.waitForFinished(-1);
            int time = this->time.elapsed();
            QByteArray output = proc.readAll();
            proc.close();

            QString res = "Выполнение сервером:";
            res += "\nВремя выполнения : " +QString::number(time)+ " мс";
            res += "\nРезультат: " + QString(output);

            this->ui->textBrowser->setText(res);

            sendClientProg();
            sendClientData();
            if (!this->ui->lineEdit->text().isEmpty())
                sendClientArg(this->ui->lineEdit->text());
            this->time.start();
            sendClientRun();
            count_client_dane = 0;
            this->res = 0;
    } else {
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Выберите входные данные и задачу для выполнения!!!"));
    }
}

