
#include "COrderBookWindow.hpp"
#include "common/consts.hpp"

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
    int rowCount = std::min(NotAMagicJustNums::ROW_UPPER_LIMIT, 
        static_cast<int>(std::max(bids.size(), asks.size())));
    
    mTableWidget->setRowCount(rowCount);

    for (int rowNum = 0; rowNum < rowCount; ++rowNum) {
        QTableWidgetItem *bidQtyItem = nullptr;
        QTableWidgetItem *bidPriceItem = nullptr;
        QTableWidgetItem *askPriceItem = nullptr;
        QTableWidgetItem *askQtyItem = nullptr;

        if (rowNum < bids.size()) {
            QJsonArray bidEntry = bids[rowNum].toArray();
            QString bidPrice = bidEntry[0].toString();
            QString bidQty = bidEntry[1].toString();
            bidQtyItem = new QTableWidgetItem(bidQty);
            bidPriceItem = new QTableWidgetItem(bidPrice);
            mTableWidget->setItem(rowNum, 0, bidQtyItem);
            mTableWidget->setItem(rowNum, 1, bidPriceItem);
        }

        if (rowNum < asks.size()) {
            QJsonArray askEntry = asks[rowNum].toArray();
            QString askPrice = askEntry[0].toString();
            QString askQty = askEntry[1].toString();
            askPriceItem = new QTableWidgetItem(askPrice);
            askQtyItem = new QTableWidgetItem(askQty);
            mTableWidget->setItem(rowNum, 2, askPriceItem);
            mTableWidget->setItem(rowNum, 3, askQtyItem);
        }

        // Highlight the top result
        if (NotAMagicJustNums::FIRST_ROW == rowNum) {
            QBrush highlightBrush(Qt::yellow);

            if (bidQtyItem) bidQtyItem->setBackground(highlightBrush);
            if (bidPriceItem) bidPriceItem->setBackground(highlightBrush);
            if (askPriceItem) askPriceItem->setBackground(highlightBrush);
            if (askQtyItem) askQtyItem->setBackground(highlightBrush);
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
