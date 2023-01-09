#ifndef TEMPLATEDEMOVIEW_H
#define TEMPLATEDEMOVIEW_H
#include <QTreeView>
#include <QTableView>
#include <type_traits>
#include <QAction>
#include <QMenu>
#include <QInputDialog>
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
        this->setSelectionMode(QAbstractItemView::ExtendedSelection);
        this->setSelectionBehavior(QAbstractItemView::SelectItems);
        QObject::connect(this, &DemoView::customContextMenuRequested, this, &DemoView::onContextMenuRequested);
        contextMenu = new QMenu(this);
        a_insertRowEnd = new QAction(QObject::tr("Append Row(s)"), this);
        QObject::connect(a_insertRowEnd, &QAction::triggered, this, &DemoView::onInsertRowEnd);
        contextMenu->addAction(a_insertRowEnd);
        a_insertColumnEnd = new QAction(QObject::tr("Append Column(s)"), this);
        QObject::connect(a_insertColumnEnd, &QAction::triggered, this, &DemoView::onInsertColumnEnd);
        contextMenu->addAction(a_insertColumnEnd);
        a_insertRowBefore = new QAction(QObject::tr("Insert Row(s) Above"), this);
        QObject::connect(a_insertRowBefore, &QAction::triggered, this, &DemoView::onInsertRowBefore);
        contextMenu->addAction(a_insertRowBefore);
        a_insertRowAfter = new QAction(QObject::tr("Insert Row(s) Below"), this);
        QObject::connect(a_insertRowAfter, &QAction::triggered, this, &DemoView::onInsertRowAfter);
        contextMenu->addAction(a_insertRowAfter);
        a_insertColumnBefore = new QAction(QObject::tr("Insert Column(s) Left"), this);
        QObject::connect(a_insertColumnBefore, &QAction::triggered, this, &DemoView::onInsertColumnBefore);
        contextMenu->addAction(a_insertColumnBefore);
        a_insertColumnAfter = new QAction(QObject::tr("Insert Column(s) Right"), this);
        QObject::connect(a_insertColumnAfter, &QAction::triggered, this, &DemoView::onInsertColumnAfter);
        contextMenu->addAction(a_insertColumnAfter);
        if constexpr (std::is_base_of<QTreeView, BaseView>::value) {
            a_insertChildRowEnd = new QAction(QObject::tr("Append Child Row(s)"), this);
            QObject::connect(a_insertChildRowEnd, &QAction::triggered, this, &DemoView::onInsertChildRowEnd);
            contextMenu->addAction(a_insertChildRowEnd);
            a_insertChildColumnEnd = new QAction(QObject::tr("Append Child Column(s)"), this);
            QObject::connect(a_insertChildColumnEnd, &QAction::triggered, this, &DemoView::onInsertChildColumnEnd);
            contextMenu->addAction(a_insertChildColumnEnd);
        }
        a_RemoveRow = new QAction(QObject::tr("Remove Row(s)"), this);
        QObject::connect(a_RemoveRow, &QAction::triggered, this, &DemoView::onRemoveRow);
        contextMenu->addAction(a_RemoveRow);
        a_RemoveCol = new QAction(QObject::tr("Remove Column(s)"), this);
        QObject::connect(a_RemoveCol, &QAction::triggered, this, &DemoView::onRemoveColumn);
        contextMenu->addAction(a_RemoveCol);
    }
    ~DemoView() { }

private:
    void onContextMenuRequested(const QPoint &pos)
    {
        menuIndexes = this->selectionModel()->selectedIndexes();
        a_insertRowEnd->setVisible(menuIndexes.isEmpty());
        a_insertColumnEnd->setVisible(menuIndexes.isEmpty());
        a_insertRowBefore->setVisible(!menuIndexes.isEmpty());
        a_insertRowAfter->setVisible(!menuIndexes.isEmpty());
        a_insertColumnBefore->setVisible(!menuIndexes.isEmpty());
        a_insertColumnAfter->setVisible(!menuIndexes.isEmpty());
        a_RemoveRow->setVisible(!menuIndexes.isEmpty());
        a_RemoveCol->setVisible(!menuIndexes.isEmpty());
        if constexpr (std::is_base_of<QTreeView, BaseView>::value) {
            a_insertChildRowEnd->setVisible(!menuIndexes.isEmpty());
            a_insertChildColumnEnd->setVisible(!menuIndexes.isEmpty());
        }
        contextMenu->popup(this->mapToGlobal(pos));
    }
    void onInsertRowEnd()
    {
        bool okDialog = false;
        const int numRows = QInputDialog::getInt(this, QObject::tr("Number of Rows"), QObject::tr("Select the number of rows to insert"), 1, 1,
                                                 std::numeric_limits<int>::max() / 2, 1, &okDialog);
        if (!okDialog)
            return;
        this->model()->insertRows(this->model()->rowCount(), numRows);
    }
    void onInsertColumnEnd()
    {
        bool okDialog = false;
        const int numCols = QInputDialog::getInt(this, QObject::tr("Number of Columns"), QObject::tr("Select the number of columns to insert"), 1, 1,
                                                 std::numeric_limits<int>::max() / 2, 1, &okDialog);
        if (!okDialog)
            return;
        this->model()->insertColumns(this->model()->columnCount(), numCols);
    }
    void onInsertColumnBefore()
    {
        Q_ASSERT(!menuIndexes.isEmpty());
        bool okDialog = false;
        const int numCols = QInputDialog::getInt(this, QObject::tr("Number of Columns"), QObject::tr("Select the number of columns to insert"), 1, 1,
                                                 std::numeric_limits<int>::max() / 2, 1, &okDialog);
        if (!okDialog)
            return;
        for (const QModelIndex &menuIndex : qAsConst(menuIndexes))
            this->model()->insertColumns(menuIndex.column(), numCols, menuIndex.parent());
    }
    void onInsertColumnAfter()
    {
        Q_ASSERT(!menuIndexes.isEmpty());
        bool okDialog = false;
        const int numCols = QInputDialog::getInt(this, QObject::tr("Number of Columns"), QObject::tr("Select the number of columns to insert"), 1, 1,
                                                 std::numeric_limits<int>::max() / 2, 1, &okDialog);
        if (!okDialog)
            return;
        for (const QModelIndex &menuIndex : qAsConst(menuIndexes))
            this->model()->insertColumns(menuIndex.column() + 1, numCols, menuIndex.parent());
    }
    void onInsertRowBefore()
    {
        Q_ASSERT(!menuIndexes.isEmpty());
        bool okDialog = false;
        const int numRows = QInputDialog::getInt(this, QObject::tr("Number of Rows"), QObject::tr("Select the number of rows to insert"), 1, 1,
                                                 std::numeric_limits<int>::max() / 2, 1, &okDialog);
        if (!okDialog)
            return;
        for (const QModelIndex &menuIndex : qAsConst(menuIndexes))
            this->model()->insertRows(menuIndex.row(), numRows, menuIndex.parent());
    }
    void onInsertRowAfter()
    {
        Q_ASSERT(!menuIndexes.isEmpty());
        bool okDialog = false;
        const int numRows = QInputDialog::getInt(this, QObject::tr("Number of Rows"), QObject::tr("Select the number of rows to insert"), 1, 1,
                                                 std::numeric_limits<int>::max() / 2, 1, &okDialog);
        if (!okDialog)
            return;
        for (const QModelIndex &menuIndex : qAsConst(menuIndexes))
            this->model()->insertRows(menuIndex.row() + 1, numRows, menuIndex.parent());
    }
    void onInsertChildRowEnd()
    {
        Q_ASSERT(!menuIndexes.isEmpty());
        bool okDialog = false;
        const int numRows = QInputDialog::getInt(this, QObject::tr("Number of Rows"), QObject::tr("Select the number of rows to insert"), 1, 1,
                                                 std::numeric_limits<int>::max() / 2, 1, &okDialog);
        if (!okDialog)
            return;
        for (const QModelIndex &menuIndex : qAsConst(menuIndexes))
            this->model()->insertRows(this->model()->rowCount(menuIndex), numRows, menuIndex);
    }
    void onInsertChildColumnEnd()
    {
        Q_ASSERT(!menuIndexes.isEmpty());
        bool okDialog = false;
        const int numCols = QInputDialog::getInt(this, QObject::tr("Number of Columns"), QObject::tr("Select the number of columns to insert"), 1, 1,
                                                 std::numeric_limits<int>::max() / 2, 1, &okDialog);
        if (!okDialog)
            return;
        for (const QModelIndex &menuIndex : qAsConst(menuIndexes))
            this->model()->insertColumns(this->model()->columnCount(menuIndex), numCols, menuIndex);
    }
    void onRemoveRow()
    {
        Q_ASSERT(!menuIndexes.isEmpty());
        QMap<QModelIndex, std::pair<int, int>> minMaxRows;
        for (const QModelIndex &menuIndex : qAsConst(menuIndexes)) {
            auto minMaxRowsIter = minMaxRows.find(menuIndex.parent());
            if (minMaxRowsIter == minMaxRows.end())
                minMaxRows.insert(menuIndex.parent(), std::make_pair(menuIndex.row(), menuIndex.row()));
            else if (menuIndex.row() < minMaxRowsIter->first)
                minMaxRowsIter->first = menuIndex.row();
            else if (menuIndex.row() > minMaxRowsIter->second)
                minMaxRowsIter->second = menuIndex.row();
        }
        for (auto i = minMaxRows.cbegin(), iEnd = minMaxRows.cend(); i != iEnd; ++i)
            this->model()->removeRows(i->first, i->second - i->first + 1, i.key());
    }
    void onRemoveColumn()
    {
        Q_ASSERT(!menuIndexes.isEmpty());
        QMap<QModelIndex, std::pair<int, int>> minMaxCols;
        for (const QModelIndex &menuIndex : qAsConst(menuIndexes)) {
            auto minMaxColsIter = minMaxCols.find(menuIndex.parent());
            if (minMaxColsIter == minMaxCols.end())
                minMaxCols.insert(menuIndex.parent(), std::make_pair(menuIndex.column(), menuIndex.column()));
            else if (menuIndex.row() < minMaxColsIter->first)
                minMaxColsIter->first = menuIndex.column();
            else if (menuIndex.row() > minMaxColsIter->second)
                minMaxColsIter->second = menuIndex.column();
        }
        for (auto i = minMaxCols.cbegin(), iEnd = minMaxCols.cend(); i != iEnd; ++i)
            this->model()->removeColumns(i->first, i->second - i->first + 1, i.key());
    }
    QModelIndexList menuIndexes;
    QAction *a_insertRowEnd = nullptr;
    QAction *a_insertColumnEnd = nullptr;
    QAction *a_insertRowBefore = nullptr;
    QAction *a_insertRowAfter = nullptr;
    QAction *a_insertColumnBefore = nullptr;
    QAction *a_insertColumnAfter = nullptr;
    QAction *a_insertChildRowEnd = nullptr;
    QAction *a_insertChildColumnEnd = nullptr;
    QAction *a_RemoveRow = nullptr;
    QAction *a_RemoveCol = nullptr;
    QMenu *contextMenu = nullptr;
};
using DemoTreeView = DemoView<QTreeView>;
using DemoTableView = DemoView<QTableView>;
#endif // TEMPLATEDEMOVIEW_H
