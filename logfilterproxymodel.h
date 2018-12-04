#ifndef LOGFILTERPROXYMODEL_H
#define LOGFILTERPROXYMODEL_H

#include <QtCore/qsortfilterproxymodel.h>
#include <QtQml/qqmlparserstatus.h>

class LogFilterProxyModel : public QSortFilterProxyModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(QObject *source READ source WRITE setSource)
    Q_PROPERTY(QString filterString READ filterString WRITE setFilterString)

public:
    explicit LogFilterProxyModel(QObject *parent = nullptr);

    QObject *source() const;
    void setSource(QObject *source);

    QString filterString() const;
    void setFilterString(const QString &filter);

    void classBegin() override;
    void componentComplete() override;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString m_logTextFilterString;
};

#endif // LOGFILTERPROXYMODEL_H
