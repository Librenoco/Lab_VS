#include "mainwindow.h"
#include "ui_mainwindow.h"

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
        emit this->addNewclient();
        m.unlock();
    };

    ws.onmessage = [&](const WebSocketChannelPtr& channel, const std::string& msg) {

    };

    ws.onclose = [&](const WebSocketChannelPtr& channel) {
        m_del.lock();
        emit this->deleteNewclient(client.key(channel));
        m_del.unlock();
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
}

void MainWindow::addNewclients()
{
    this->ui->listWidget->addItem("Клиент " + QString::number(count_client));
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

