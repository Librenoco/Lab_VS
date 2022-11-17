#include <QCoreApplication>
#include <QFile>
#include <QProcess>
#include <thread>
#include <QString>
#include <QDebug>
#include <QTextStream>
#include <chrono>
#include <string>
#include "hv/WebSocketClient.h"

using namespace hv;

QTextStream inp(stdin);
QTextStream outp(stdout);
QString arg;

void work(QString ip)
{
    std::string ip_server;
    WebSocketClient *ws = new WebSocketClient();
    ws->onmessage = [=](const std::string &msg) {
        QStringList list_data = QString(msg.c_str()).split(' ');
        if (list_data[0] == "prog") {
            QByteArray prog = QByteArray::fromBase64(list_data[1].toUtf8());
            QFile file("prog");
            if (file.open(QIODevice::WriteOnly)) {
                file.write(prog);
                file.close();
            }
            qDebug() << "get prog";
        } else if (list_data[0] == "data"){
            QByteArray data = QByteArray::fromBase64(list_data[1].toUtf8());
            QFile file("data");
            if (file.open(QIODevice::WriteOnly)) {
                file.write(data);
                file.close();
            }
            qDebug() << "get data";
        } else if (list_data[0] == "run") {
            QProcess proc;
            QStringList args;

            if (!arg.isEmpty()) {
               args = arg.split(' ');
            }

            args << "data";
            proc.start("./prog", args);
            proc.waitForFinished();
            QString res(proc.readAll());
            proc.close();
            qDebug() << "Result " + res;
            if (!res.isEmpty())
                ws->send(res.toStdString());
            else
                ws->send("NULL");
        } else if (list_data[0] == "arg") {
            arg = QString(QByteArray::fromBase64(list_data[1].toUtf8()));
        }
    };

    ws->onopen = [&]() {
        qDebug() << "connect server";
    };

    ws->onclose = [&](){
        qDebug() << "disconnect server";
    };

    reconn_setting_t reconn;
    reconn_setting_init(&reconn);
    reconn.min_delay = 1000;
    reconn.max_delay = 10000;
    reconn.delay_policy = 2;

    ws->setReconnect(&reconn);
    ws->open(("ws://" + ip + ":8999").toStdString().c_str());
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    std::thread t([](){
        work("192.168.0.102");
    });

    t.detach();
    return a.exec();
}
