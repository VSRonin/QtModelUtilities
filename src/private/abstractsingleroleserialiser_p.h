
#ifndef abstractsingleroleserialiser_p_h__
#define abstractsingleroleserialiser_p_h__
#include "private/abstractmodelserialiser_p.h"
#include "abstractsingleroleserialiser.h"
class AbstractSingleRoleSerialiserPrivate : public AbstractModelSerialiserPrivate
{
    Q_DISABLE_COPY(AbstractSingleRoleSerialiserPrivate);
    Q_DECLARE_PUBLIC(AbstractSingleRoleSerialiser)
protected:
    AbstractSingleRoleSerialiserPrivate(AbstractSingleRoleSerialiser* q);
    int m_roleToSave;
};
#endif // abstractsingleroleserialiser_p_h__