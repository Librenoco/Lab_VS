#include <QCoreApplication>
#include "hv/WebSocketClient.h"
#include <thread>
#include <chrono>
using namespace hv;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    WebSocketClient *ws = new WebSocketClient();
    ws->onmessage = [&](const std::string &msg) {

    };
    ws->onopen = [&]() {

    };
    ws->onclose = [&](){

    };

    reconn_setting_t reconn;
    reconn_setting_init(&reconn);
    reconn.min_delay = 1000;
    reconn.max_delay = 10000;
    reconn.delay_policy = 2;

    ws->setReconnect(&reconn);
    ws->open("ws://127.0.0.1:8999");

    ws->close();
    delete ws;
    return a.exec();
}
