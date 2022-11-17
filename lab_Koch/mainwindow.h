#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QFileDialog>
#include <QList>
#include <QHash>
#include <thread>
#include <mutex>

#include "hv/WebSocketServer.h"
#include "hv/WebSocketChannel.h"
using namespace hv;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void addNewclient();
    void deleteNewclient(int index);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void addNewclients();
    void deleteNewclients(int index);
private:
    QHash<int,WebSocketChannelPtr> client;
    Ui::MainWindow *ui;
    WebSocketService ws;
    WebSocketServer server;
    std::thread *t;
    std::mutex m;
    std::mutex m_del;
    QString name_file_data;
    QString name_file_prog;
    int count_client;
};
#endif // MAINWINDOW_H
