#ifndef modelutilities_common_p_h__
#define modelutilities_common_p_h__

#include <QVariant>
#ifdef OPTIMISE_FOR_MANY_ROLES
#include <QHash>
using RolesContainer = QHash<int,QVariant>;
#else
#include <QMap>
using RolesContainer = QMap<int, QVariant>;
#endif // OPTIMISE_FOR_MANY_ROLES
inline const RolesContainer& convertToContainer(const RolesContainer& other) { return other; }
template < class T >
RolesContainer convertToContainer(const T& other){
    RolesContainer result;
    for (auto i = other.begin(), otherEnd = other.end(); i != otherEnd; ++i)
        result.insert(i.key(), i.value());
    return result;
}
template < class T, class = typename std::enable_if<std::is_same<T, RolesContainer>::value>::type>
const RolesContainer& convertFromContainer(const RolesContainer& other)
{
    return other;
}
template < class T, class = typename std::enable_if<!std::is_same<T, RolesContainer>::value>::type >
T convertFromContainer(const RolesContainer& other)
{
    T result;
    for (auto i = other.begin(), otherEnd = other.end(); i != otherEnd; ++i)
        result.insert(i.key(), i.value());
    return result;
}
#endif // modelutilities_common_p_h__
