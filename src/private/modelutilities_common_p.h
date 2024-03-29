/****************************************************************************\
   Copyright 2018 Luca Beldi
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
\****************************************************************************/
#ifndef MODELUTILITIES_COMMON_P_H
#define MODELUTILITIES_COMMON_P_H

#include <QVariant>
#include <QVector>
#include <QList>
#ifdef OPTIMISE_FOR_MANY_ROLES
#    include <QHash>
using RolesContainer = QHash<int, QVariant>;
#else
#    include <QMap>
using RolesContainer = QMap<int, QVariant>;
#endif // OPTIMISE_FOR_MANY_ROLES
inline const RolesContainer &convertToContainer(const RolesContainer &other)
{
    return other;
}
template<class T>
RolesContainer convertToContainer(const T &other)
{
    RolesContainer result;
    for (auto i = other.begin(), otherEnd = other.end(); i != otherEnd; ++i)
        result.insert(i.key(), i.value());
    return result;
}
template<class T, class = typename std::enable_if<std::is_same<T, RolesContainer>::value>::type>
const RolesContainer &convertFromContainer(const RolesContainer &other)
{
    return other;
}
template<class T, class = typename std::enable_if<!std::is_same<T, RolesContainer>::value>::type>
T convertFromContainer(const RolesContainer &other)
{
    T result;
    for (auto i = other.begin(), otherEnd = other.end(); i != otherEnd; ++i)
        result.insert(i.key(), i.value());
    return result;
}
template<class T>
QVector<T> listToVector(const QList<T> &list)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return list;
#elif QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    return QVector<T>(list.constBegin(), list.constEnd());
#else
    return list.toVector();
#endif
}
#endif // MODELUTILITIES_COMMON_P_H
