#include "MainWindow.h"
#include <QVBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHeaderView>
#include <QNetworkReply>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), networkManager(new QNetworkAccessManager(this)) {

    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    tableWidget = new QTableWidget(0, 3, this);
    tableWidget->setHorizontalHeaderLabels({"Symbol", "Last Price", "Volume"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout->addWidget(tableWidget);
    setCentralWidget(central);
    resize(600, 400);
    setWindowTitle("OKX Market Data");

    connect(networkManager, &QNetworkAccessManager::finished,
            this, &MainWindow::onDataReceived);

    fetchData();
}

MainWindow::~MainWindow() {}

void MainWindow::fetchData() {
    QUrl url("https://www.okx.com/api/v5/market/tickers?instType=SPOT");
    QNetworkRequest request(url);
    networkManager->get(request);
}

void MainWindow::onDataReceived(QNetworkReply* reply) {
    if (reply->error()) {
        qDebug() << "Error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject rootObj = jsonDoc.object();
    QJsonArray data = rootObj["data"].toArray();

    tableWidget->setRowCount(0); // Clear previous data

    int dataSize = std::min(10, static_cast<int>(data.size()));

    QString symbol = "N/A";
    QString price = "N/A";
    QString volume = "N/A";

    for (int i = 0; i < dataSize; ++i) {

        if (data[i].isObject()) {
            QJsonObject obj = data[i].toObject();

            QString symbol = obj.contains("instId") ? obj["instId"].toString() : "N/A";
            QString price  = obj.contains("last")   ? obj["last"].toString()   : "N/A";
            QString volume = obj.contains("vol24h") ? obj["vol24h"].toString() : "N/A";
        }

        tableWidget->insertRow(i);
        tableWidget->setItem(i, 0, new QTableWidgetItem(symbol));
        tableWidget->setItem(i, 1, new QTableWidgetItem(price));
        tableWidget->setItem(i, 2, new QTableWidgetItem(volume));
    }

    reply->deleteLater();
}
