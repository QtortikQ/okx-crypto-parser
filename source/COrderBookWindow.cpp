
#include "COrderBookWindow.hpp"
#include <QVBoxLayout>
#include <QJsonObject>
#include <QJsonArray>
#include <QHeaderView>

COrderBookWindow::COrderBookWindow(QWidget *parent)
    : QWidget(parent)
    , mInstId("N/A")
{
    qDebug() << "COrderBookWindow::COrderBookWindow()";
    setWindowTitle("Order Book");
    resize(400, 300);

    auto *layout = new QVBoxLayout(this);
    mLabel = new QLabel("N/A");
    layout->addWidget(mLabel);

    mTableWidget = new QTableWidget(0, 4, this);
    mTableWidget->setHorizontalHeaderLabels(
        {"BID Quantity",
         "BID Price",
         "ASK Price",
         "ASK Quantity"});
    mTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout->addWidget(mTableWidget);
}

COrderBookWindow::~COrderBookWindow()
{
    qDebug() << "COrderBookWindow::~COrderBookWindow()";
}

void COrderBookWindow::parseJsonObj(const QJsonArray& dataArray)
{
    QJsonObject obj = dataArray.first().toObject();
    mInstId = obj["instId"].toString();

    QJsonArray bids = obj["bids"].toArray();
    QJsonArray asks = obj["asks"].toArray();

    fillTable(bids, asks);
}

void COrderBookWindow::fillTable(const QJsonArray& bids, const QJsonArray& asks)
{
    int rowCount = std::max(bids.size(), asks.size());
    
    mTableWidget->setRowCount(rowCount);

    for (int i = 0; i < rowCount; ++i) {
        if (i < bids.size()) {
            QJsonArray bidEntry = bids[i].toArray();
            QString bidPrice = bidEntry[0].toString();
            QString bidQty = bidEntry[1].toString();
            mTableWidget->setItem(i, 0, new QTableWidgetItem(bidQty));
            mTableWidget->setItem(i, 1, new QTableWidgetItem(bidPrice));
        }
        if (i < asks.size()) {
            QJsonArray askEntry = asks[i].toArray();
            QString askPrice = askEntry[0].toString();
            QString askQty = askEntry[1].toString();
            mTableWidget->setItem(i, 2, new QTableWidgetItem(askPrice));
            mTableWidget->setItem(i, 3, new QTableWidgetItem(askQty));
        }
    }
}

void COrderBookWindow::setLabel(const QString &label)
{
    mLabel->setText(label);
}

void COrderBookWindow::closeEvent(QCloseEvent *event) {
    emit windowClosed(mInstId);
    QWidget::closeEvent(event);
}
