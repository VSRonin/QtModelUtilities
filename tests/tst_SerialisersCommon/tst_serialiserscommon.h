#ifndef tst_serialisers_common_h__
#define tst_serialisers_common_h__
#include <QModelIndex>
#include <QStringListModel>
#ifdef QT_GUI_LIB
#    include <QStandardItemModel>
#endif
class AbstractModelSerialiser;
class AbstractStringSerialiser;
class tst_SerialiserCommon
{
protected:
    void saveLoadByteArray(AbstractModelSerialiser *serialiser, const QAbstractItemModel *sourceModel, QAbstractItemModel *destinationModel,
                           bool multiRole = true) const;
    void saveLoadFile(AbstractModelSerialiser *serialiser, const QAbstractItemModel *sourceModel, QAbstractItemModel *destinationModel,
                      bool multiRole = true) const;
    void saveLoadString(AbstractStringSerialiser *serialiser, const QAbstractItemModel *sourceModel, QAbstractItemModel *destinationModel,
                        bool multiRole = true) const;
    void checkModelEqual(const QAbstractItemModel *a, const QAbstractItemModel *b, const QModelIndex &aParent = QModelIndex(),
                         const QModelIndex &bParent = QModelIndex()) const;
    const QAbstractItemModel *createStringModel(QObject *parent = nullptr) const;
#ifdef QT_GUI_LIB
    void insertBranch(QAbstractItemModel *model, const QModelIndex &parent, bool multiRoles, int subBranches) const;
    const QAbstractItemModel *createComplexModel(bool tree, bool multiRoles, QObject *parent = nullptr) const;
#endif
};
#endif
