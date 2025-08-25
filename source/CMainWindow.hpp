#ifndef CMAINWINDOW_HPP
#define CMAINWINDOW_HPP

#include "COrderBookWindow.hpp"

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QTableWidget>
#include <QLineEdit>
#include <QWebSocket>

#include <set>

class CMainWindow : public QMainWindow {
    Q_OBJECT

public:
    CMainWindow(QWidget *parent = nullptr);
    ~CMainWindow();

private slots:
    void onLineEditTextFinished();
    void onTableCellClicked(int row, int column);
    void onRESTDataReceived(QNetworkReply* reply);

    void onConnected();
    void onTextMessageReceived(const QString &message);
    void onInternalTimerTick();

private:
    QNetworkAccessManager *mNetworkManager;
    QTableWidget *mTableWidget;
    QLineEdit *mTextLine;
    QWebSocket mWebSocket;
    QString mOrderBookInstId;

    COrderBookWindow* mOrderBookWindow;

    bool mIsUserSearch;

    std::set<QString> mSubscribedTickers; 
    std::map<QString, QJsonObject> mInstrumentArr;

    void fetchData();
    void sendWebSocketMessage(const QJsonObject &subAction);
    void startInternalTimer();
    void updateDefaultTableView();
    void subscribeToTicker(const QString &instId);
    void unsubscribeFromTicker(const QString &instId);
    void subscribeToOderBook(const QString &instId);
    void unsubscribeFromOrderBook(const QString &instId);
    void rbTreeSearch(const std::map<QString, QJsonObject>& data);
    void fillTable(const QJsonObject& obj, const size_t& rowNum);
};

#endif // CMAINWINDOW_HPP
