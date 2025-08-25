#ifndef ORDERBOOKWINDOW_HPP
#define ORDERBOOKWINDOW_HPP

#include <QLabel>
#include <QTableWidget>
#include <QWidget>

class COrderBookWindow : public QWidget {
    Q_OBJECT

public:
    explicit COrderBookWindow(QWidget *parent = nullptr);
    ~COrderBookWindow();

    void parseJsonObj(const QJsonArray& obj);
    void fillTable(const QJsonArray& bids, const QJsonArray& asks);
    void setLabel(const QString &label);
signals:
    void windowClosed(const QString &instId);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QTableWidget *mTableWidget;
    QLabel *mLabel;
    QString mInstId;

    std::map<QString, QJsonObject> mInstrumentArr;
};

#endif // ORDERBOOKWINDOW_HPP
