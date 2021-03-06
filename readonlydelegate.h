#ifndef READONLYDELEGATE_H
#define READONLYDELEGATE_H

#include <QItemDelegate>
#include <QWidget>
class ReadOnlyDelegate: public QItemDelegate
{

public:
    ReadOnlyDelegate(QWidget *parent = nullptr):QItemDelegate(parent)
    {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
    const QModelIndex &index) const override //final
    {
        Q_UNUSED(parent)
        Q_UNUSED(option)
        Q_UNUSED(index)
        return nullptr;
    }
};

#endif // READONLYDELEGATE_H
