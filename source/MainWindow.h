//#pragma once

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QTableWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDataReceived(QNetworkReply* reply);

private:
    QNetworkAccessManager *networkManager;
    QTableWidget *tableWidget;
    void fetchData();
};
