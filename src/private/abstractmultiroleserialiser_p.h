
#ifndef abstractmultiroleserialiser_p_h__
#define abstractmultiroleserialiser_p_h__

#include "private/abstractmodelserialiser_p.h"
class AbstractMultiRoleSerialiserPrivate : public AbstractModelSerialiserPrivate
{
    Q_DISABLE_COPY(AbstractMultiRoleSerialiserPrivate);
    Q_DECLARE_PUBLIC(AbstractMultiRoleSerialiser)
protected:
    AbstractMultiRoleSerialiserPrivate(AbstractMultiRoleSerialiser* q);
    QList<int> m_rolesToSave;
};
#endif // abstractmultiroleserialiser_p_h__