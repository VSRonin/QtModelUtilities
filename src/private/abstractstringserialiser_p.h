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

#ifndef ABSTRACTMODELSERIALISER_P_H
#define ABSTRACTMODELSERIALISER_P_H

#include "abstractstringserialiser.h"
#include "abstractmodelserialiser_p.h"
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
class QTextCodec;
#endif
class AbstractStringSerialiserPrivate : public AbstractModelSerialiserPrivate
{
    Q_DISABLE_COPY(AbstractStringSerialiserPrivate)
    Q_DECLARE_PUBLIC(AbstractStringSerialiser)
protected:
    AbstractStringSerialiserPrivate(AbstractStringSerialiser *q);
    QString variantToString(const QVariant &val) const;
    QVariant stringToVariant(const QString &val) const;
#ifdef QT_GUI_LIB
    static QImage loadImageVariant(int type, const QString &val);
    static QString saveImageVariant(const QImage &val);
#endif
    QVariant loadVariant(int type, const QString &val) const;
    QString saveVariant(const QVariant &val) const;
    static int guessDecimals(double val);
    static QString guessDecimalsString(double val, QLocale *loca = Q_NULLPTR);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QTextCodec *m_textCodec;
#endif
};

#endif // ABSTRACTMODELSERIALISER_P_H
