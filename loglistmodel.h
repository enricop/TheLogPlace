#ifndef LOGLISTMODEL_H
#define LOGLISTMODEL_H

#include <QAbstractListModel>

class LogItemList;

class LogListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(LogItemList *list READ list WRITE setList)

public:
    explicit LogListModel(QObject *parent = nullptr);

    enum LogRoles {
        TimestampRole = Qt::UserRole + 1,
        ProcessNameRole,
        MessageRole
    };

    int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QHash<int, QByteArray> roleNames() const override;

    // read/write list property
    LogItemList *list() const;
    void setList(LogItemList *list);

protected:

private:
    LogItemList *mlogs;
};

#endif // LOGLISTMODEL_H
