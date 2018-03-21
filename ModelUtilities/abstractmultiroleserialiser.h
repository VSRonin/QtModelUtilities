
#ifndef abstractmultiroleserialiser_h__
#define abstractmultiroleserialiser_h__

#include "modelutilities_global.h"
#include "abstractmodelserialiser.h"
class AbstractMultiRoleSerialiserPrivate;
class MODELUTILITIES_EXPORT AbstractMultiRoleSerialiser : public AbstractModelSerialiser
{
    Q_GADGET
    Q_PROPERTY(QList<int> rolesToSave READ rolesToSave WRITE setRoleToSave RESET resetRoleToSave)
    Q_DECLARE_PRIVATE(AbstractMultiRoleSerialiser)
public:
    AbstractMultiRoleSerialiser(QAbstractItemModel* model = Q_NULLPTR);
    AbstractMultiRoleSerialiser(const QAbstractItemModel* model);
    AbstractMultiRoleSerialiser(const AbstractMultiRoleSerialiser& other);
    AbstractMultiRoleSerialiser& operator=(const AbstractMultiRoleSerialiser& other);
#ifdef Q_COMPILER_RVALUE_REFS
    AbstractMultiRoleSerialiser(AbstractMultiRoleSerialiser&& other) Q_DECL_NOEXCEPT;
    AbstractMultiRoleSerialiser& operator=(AbstractMultiRoleSerialiser&& other);
#endif // Q_COMPILER_RVALUE_REFS
    virtual ~AbstractMultiRoleSerialiser() Q_DECL_OVERRIDE = 0;
    const QList<int>& rolesToSave() const;
    void setRoleToSave(const QList<int>& val);
    Q_INVOKABLE void addRoleToSave(int role);
    Q_INVOKABLE void removeRoleToSave(int role);
    Q_INVOKABLE void clearRoleToSave();
    void resetRoleToSave();
    static QList<int> modelDefaultRoles();
protected:
    AbstractMultiRoleSerialiser(AbstractMultiRoleSerialiserPrivate& d);
};
#endif // abstractmultiroleserialiser_h__
