#ifndef TEMPLATEDEMOVIEW_H
#define TEMPLATEDEMOVIEW_H
#include <QTreeView>
#include <QTableView>
#include <type_traits>
#include <QAction>
#include <QMenu>
template<class BaseView>
class DemoView : public BaseView
{
    Q_DISABLE_COPY_MOVE(DemoView)
    static_assert(std::is_base_of<QAbstractItemView, BaseView>::value, "Template argument must be a QAbstractItemView");

public:
    DemoView(QWidget *parent = nullptr)
        : BaseView(parent)
    {
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        QObject::connect(this, &DemoView::customContextMenuRequested, this, &DemoView::onContextMenuRequested);
        contextMenu = new QMenu(this);
        a_insertRowEnd = new QAction(QObject::tr("Append Row(s)"), this);
        contextMenu->addAction(a_insertRowEnd);
        a_insertColumnEnd = new QAction(QObject::tr("Append Column(s)"), this);
        contextMenu->addAction(a_insertColumnEnd);
    }
    ~DemoView() { }

private:
    void onContextMenuRequested(const QPoint &pos)
    {
        const QModelIndex contextIndex = this->indexAt(pos);
        a_insertRowEnd->setVisible(!contextIndex.isValid());
        a_insertColumnEnd->setVisible(!contextIndex.isValid());
        contextMenu->popup(this->mapToGlobal(pos));
    }
    QAction *a_insertRowEnd;
    QAction *a_insertColumnEnd;
    QMenu *contextMenu;
};
using DemoTreeView = DemoView<QTreeView>;
using DemoTableView = DemoView<QTableView>;
#endif // TEMPLATEDEMOVIEW_H
