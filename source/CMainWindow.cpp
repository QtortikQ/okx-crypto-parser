#include "CMainWindow.hpp"
#include <QVBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHeaderView>
#include <QNetworkReply>
#include <QTimer>
#include <iostream>

CMainWindow::CMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mNetworkManager (new QNetworkAccessManager(this))
    , mIsUserSearch(false)
{
    qDebug() << "CMainWindow::CMainWindow()";

    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    mTableWidget = new QTableWidget(0, 6, this);
    mTableWidget->setHorizontalHeaderLabels(
        {"Instrument",
         "Best BID Qty",
         "Best BID",
         "Market price",
         "Best ASK",
         "Best ASK Qty"});
    mTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    mTextLine = new QLineEdit(this);
    mTextLine->setPlaceholderText("Enter crypto to search data for...");

    layout->addWidget(mTextLine);

    layout->addWidget(mTableWidget);
    setCentralWidget(central);
    resize(600, 400);
    setWindowTitle("OKX Market Data");

    // Connect signals
    connect(mTextLine, &QLineEdit::editingFinished,
            this, &CMainWindow::onLineEditTextFinished);
    connect(&mWebSocket, &QWebSocket::connected,
            this, &CMainWindow::onConnected);
    connect(&mWebSocket, &QWebSocket::textMessageReceived,
            this, &CMainWindow::onTextMessageReceived);
    connect(mTableWidget, &QTableWidget::cellClicked,
        this, &CMainWindow::onTableCellClicked);

    // Connect to OKX public WebSocket endpoint
    mWebSocket.open(QUrl(QStringLiteral("wss://ws.okx.com:8443/ws/v5/public")));

    connect(mNetworkManager, &QNetworkAccessManager::finished,
            this, &CMainWindow::onRESTDataReceived);

    fetchData();
}

CMainWindow::~CMainWindow()
{
    qDebug() << "CMainWindow::~CMainWindow()";

    mWebSocket.close();
}

void CMainWindow::fetchData() {
    qDebug() << "CMainWindow::fetchData()";

    QUrl url("https://www.okx.com/api/v5/public/instruments?instType=FUTURES");
    QNetworkRequest request(url);
    mNetworkManager->get(request);
}

void CMainWindow::subscribeToTicker(const QString &instId)
{
    QJsonObject subObj {
        {"op", "subscribe"},
        {"args", QJsonArray{
            QJsonObject{
                {"channel", "tickers"},
                {"instId", instId}
            }
        }}
    };

    QJsonDocument doc(subObj);
    QString jsonString = doc.toJson(QJsonDocument::Compact);

    mWebSocket.sendTextMessage(jsonString);
    mSubscribedTickers.insert(instId);
    qDebug() << "Subscribed to ticker for" << instId;
}

void CMainWindow::unsubscribeFromTicker(const QString &instId)
{
    qDebug() << "CMainWindow::unsubscribeFromTicker(): "<< instId;

    QJsonObject unsub{
        {"op", "unsubscribe"},
        {"args", QJsonArray{
            QJsonObject{
                {"channel", "tickers"},
                {"instId", instId}
            }
        }}
    };

    QJsonDocument doc(unsub);
    QString jsonString = doc.toJson(QJsonDocument::Compact);

    mWebSocket.sendTextMessage(jsonString);
    qDebug() << "Unsubscribed from ticker:" << instId;
}

void CMainWindow::subscribeToOderBook(const QString &instId)
{
    qDebug() << "CMainWindow::subscribeToOderBook(): " << instId;

    QJsonObject subObj {
        {"op", "subscribe"},
        {"args", QJsonArray{
            QJsonObject{
                {"channel", "books"},
                {"instId", instId}
            }
        }}
    };

    QJsonDocument doc(subObj);
    QString jsonString = doc.toJson(QJsonDocument::Compact);

    mWebSocket.sendTextMessage(jsonString);
    mSubscribedTickers.insert(instId);
    qDebug() << "Subscribed to ticker for" << instId;
}

void CMainWindow::unsubscribeFromOrderBook(const QString &instId)
{
    qDebug() << "CMainWindow::unsubscribeFromOrderBook(): "<< instId;


}

void CMainWindow::onLineEditTextFinished()
{
    qDebug() << "CMainWindow::onLineEditTextFinished()";
    
    if ("" != mTextLine->text()) {
        mIsUserSearch = true;
        mTableWidget->setRowCount(0);
        rbTreeSearch(mInstrumentArr);
    } else {
        qDebug() << "Field is empty. No search will be done";

        for (const QString val : mSubscribedTickers) {
            unsubscribeFromTicker(val);
            mSubscribedTickers.erase(val);
        }
        mIsUserSearch = false;
    }
}

void CMainWindow::onTableCellClicked(int row, int column)
{
    qDebug() << "CMainWindow::onTableCellClicked(): Row clicked:"
             << row << "Column:" << column;

    QTableWidgetItem* item = mTableWidget->item(row, column);
    
}

void CMainWindow::onConnected()
{
    qDebug() << "CMainWindow::onConnected(): WebSocket connected.";

    subscribeToTicker("BTC-USDT");

    startInternalTimer();
}

void CMainWindow::onTextMessageReceived(const QString &message)
{
    qDebug() << "CMainWindow::onTextMessageReceived()";
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return;
    }
    if (!doc.isObject()) {
        qWarning() << "CMainWindow::onTextMessageReceived(): \
                       Invalid JSON received.";
        return;
    }

    QJsonObject obj = doc.object();
    if (obj.contains("event") && "error" == obj["event"].toString()) {
        qDebug() << "CMainWindow::onTextMessageReceived(): Event:" \
                 << obj["event"].toString();
        qDebug() << "CMainWindow::onTextMessageReceived(): Code:" \
                 << obj["code"].toString();
        qDebug() << "CMainWindow::onTextMessageReceived(): Message:" \
                 << obj["msg"].toString();
        return;
    }
    if (!obj.contains("data") || !obj["data"].isArray()) {
        qWarning() << "No 'data' array found in message:" << message;
        qWarning() << "Contains data:" << obj.contains("data");
        qWarning() << "Is array:" << obj["data"].isArray();
        return;
    }

    QString lInstId = "N/A";

     if (obj.contains("data")) {
        QJsonArray dataArray = obj["data"].toArray();
        if (!dataArray.isEmpty()) {
            for (const QJsonValue &value : dataArray) {
                obj = value.toObject();
                lInstId = obj["instId"].toString();
                mInstrumentArr[lInstId] = obj;
            }
        }
    }
}

void CMainWindow::onInternalTimerTick()
{
    qDebug() << "CMainWindow::onInternalTimerTick()";

    updateDefaultTableView();
}

void CMainWindow::startInternalTimer()
{
    qDebug() << "CMainWindow::startInternalTimer()";

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CMainWindow::onInternalTimerTick);
    timer->start(1000);

    qDebug() << "Internal timer started";
}

void CMainWindow::onRESTDataReceived(QNetworkReply* reply)
{
    qDebug() << "CMainWindow::onRESTDataReceived()";

    if (reply->error()) {
        qDebug() << "Error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject rootObj = jsonDoc.object();
    QJsonArray data = rootObj["data"].toArray();
    
    for (const QJsonValue &value : data) {
        QJsonObject obj = value.toObject();
        QString instId = obj["instId"].toString();
        mInstrumentArr[instId] = obj;
    }

    reply->deleteLater();
}

void CMainWindow::updateDefaultTableView()
{
    qDebug() << "CMainWindow::updateDefaultTableView()";

    mTableWidget->setRowCount(0); // Clear previous data

    size_t dataSize = std::min(mSubscribedTickers.size(), mInstrumentArr.size());

    qDebug() << "CMainWindow::updateDefaultTableView(): Size of data: " << dataSize;

    size_t counter = 0;
    std::map<QString, QJsonObject>::iterator it;
    for (it = mInstrumentArr.begin(); counter < dataSize; ++it) {
        if (mSubscribedTickers.contains(it->first)) {
            fillTable(it->second, counter);
            counter++;
        }
    }
}

void CMainWindow::rbTreeSearch(const std::map<QString, QJsonObject>& data)
{
    qDebug() << "CMainWindow::rbTreeSearch()";

    QString prefix = mTextLine->text();
    QString nextPrefix = prefix;
    nextPrefix[nextPrefix.size() - 1] = QChar(nextPrefix.back().unicode() + 1); // Next lexicographic string

    auto itStart = data.lower_bound(prefix);
    auto itEnd   = data.upper_bound(nextPrefix); // not upper_bound, because QString is custom

    int8_t counter = 0;
    for (auto it = itStart; it != itEnd; ++it) {
        qDebug() << "Matching instrument with prefix: " << it->first;
        if (!mSubscribedTickers.contains(it->first)) {
            subscribeToTicker(it->first);
            fillTable(it->second, counter);
            ++counter;
        }
    }
}

void CMainWindow::fillTable(const QJsonObject& obj, const size_t& rowNum)
{
    qDebug() << "CMainWindow::fillTable() for row number: " << rowNum;

    QString cryptoName = obj.contains("instId") ? obj["instId"].toString() : "N/A";
    QString bestBIDQty = obj.contains("bidSz") ? obj["bidSz"].toString() : "N/A";
    QString bestBID = obj.contains("bidPx") ? obj["bidPx"].toString() : "N/A";
    QString marketPrice = obj.contains("last") ? obj["last"].toString()   : "N/A";
    QString bestASK = obj.contains("askPx") ? obj["askPx"].toString() : "N/A";
    QString bestASKQty = obj.contains("askSz") ? obj["askSz"].toString() : "N/A";

    mTableWidget->insertRow(rowNum);
    mTableWidget->setItem(rowNum, 0, new QTableWidgetItem(cryptoName));
    mTableWidget->setItem(rowNum, 1, new QTableWidgetItem(bestBIDQty));
    mTableWidget->setItem(rowNum, 2, new QTableWidgetItem(bestBID));
    mTableWidget->setItem(rowNum, 3, new QTableWidgetItem(marketPrice));
    mTableWidget->setItem(rowNum, 4, new QTableWidgetItem(bestASK));
    mTableWidget->setItem(rowNum, 5, new QTableWidgetItem(bestASKQty));
}
