
#ifndef abstractsingleroleserialiser_h__
#define abstractsingleroleserialiser_h__

#include "modelutilities_global.h"
#include "abstractmodelserialiser.h"
class AbstractSingleRoleSerialiserPrivate;
class MODELUTILITIES_EXPORT AbstractSingleRoleSerialiser : public AbstractModelSerialiser
{
    Q_GADGET
    Q_PROPERTY(int roleToSave READ roleToSave WRITE setRoleToSave)
    Q_DECLARE_PRIVATE(AbstractSingleRoleSerialiser)
    Q_DISABLE_COPY(AbstractSingleRoleSerialiser)
public:
    AbstractSingleRoleSerialiser(QAbstractItemModel* model = Q_NULLPTR);
    AbstractSingleRoleSerialiser(const QAbstractItemModel* model);
    ~AbstractSingleRoleSerialiser() = 0;
    int roleToSave() const;
    void setRoleToSave(int val);
protected:
    AbstractSingleRoleSerialiser(AbstractSingleRoleSerialiserPrivate& d);
};
#endif // abstractsingleroleserialiser_h__
