#ifndef LOGFILTERPROXYMODEL_H
#define LOGFILTERPROXYMODEL_H

#include <QtCore/qsortfilterproxymodel.h>
#include <QtQml/qqmlparserstatus.h>

class LogFilterProxyModel : public QSortFilterProxyModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)


public:
    LogFilterProxyModel(QObject *parent);

    void classBegin() override;
    void componentComplete() override;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};

#endif // LOGFILTERPROXYMODEL_H
