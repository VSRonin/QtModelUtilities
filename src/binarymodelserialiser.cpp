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

#include "binarymodelserialiser.h"
#include "private/binarymodelserialiser_p.h"
#include <QDataStream>
#include <QIODevice>

BinaryModelSerialiserPrivate::BinaryModelSerialiserPrivate(BinaryModelSerialiser *q)
    : AbstractModelSerialiserPrivate(q)
{ }

void BinaryModelSerialiserPrivate::writeBinaryElement(QDataStream &destination, const QModelIndex &parent) const
{
    Q_ASSERT(m_constModel);
    if (m_constModel->rowCount(parent) + m_constModel->columnCount(parent) == 0)
        return;
    destination << static_cast<qint32>(m_constModel->rowCount(parent)) << static_cast<qint32>(m_constModel->columnCount(parent));
    for (int i = 0; i < m_constModel->rowCount(parent); ++i) {
        for (int j = 0; j < m_constModel->columnCount(parent); ++j) {
            for (QList<int>::const_iterator singleRole = m_rolesToSave.constBegin(); singleRole != m_rolesToSave.constEnd(); ++singleRole) {
                const QVariant roleData = m_constModel->index(i, j, parent).data(*singleRole);
                if (roleData.isNull())
                    continue; // Skip empty roles
                destination << static_cast<qint32>(*singleRole) << roleData;
            }
            destination << static_cast<qint32>(-1);
            const bool hasChild = m_constModel->hasChildren(m_constModel->index(i, j, parent));
            destination << hasChild;
            if (hasChild)
                writeBinaryElement(destination, m_constModel->index(i, j, parent));
        }
    }
}

bool BinaryModelSerialiserPrivate::readBinaryElement(QDataStream &source, const QModelIndex &parent)
{
    Q_ASSERT(m_model);
    qint32 rowCount, colCount;
    source >> rowCount >> colCount;
    m_model->insertRows(0, rowCount, parent);
    m_model->insertColumns(0, colCount, parent);
    if(m_model->rowCount(parent) != rowCount)
        return false;
    if(m_model->columnCount(parent) != colCount)
        return false;
    qint32 tempRole = -1;
    QVariant tempData;
    bool hasChild = false;
    for (int i = 0; i < rowCount && source.status() == QDataStream::Ok; ++i) {
        for (int j = 0; j < colCount && source.status() == QDataStream::Ok; ++j) {
            const QModelIndex currIdx = m_model->index(i, j, parent);
            for (source >> tempRole; tempRole != -1 && source.status() == QDataStream::Ok; source >> tempRole) {
                source >> tempData;
                if (source.status() != QDataStream::Ok)
                    break;
                m_model->setData(currIdx, tempData, tempRole);
            }
            if (source.status() == QDataStream::Ok)
                source >> hasChild;
            if (source.status() != QDataStream::Ok)
                break;
            if (hasChild) {
                if (!readBinaryElement(source, currIdx)) {
                    m_model->removeRows(0, rowCount, parent);
                    m_model->removeColumns(0, colCount, parent);
                    return false;
                }
            }
        }
    }
    if (source.status() != QDataStream::Ok) {
        m_model->removeRows(0, rowCount, parent);
        m_model->removeColumns(0, colCount, parent);
        return false;
    }
    return true;
}

bool BinaryModelSerialiserPrivate::readBinary(QDataStream &reader)
{
    if (!m_model)
        return false;
    m_model->removeColumns(0, m_model->columnCount());
    m_model->removeRows(0, m_model->rowCount());
#if QT_VERSION >= 0x050700
    reader.startTransaction();
#endif
    reader.setVersion(QDataStream::Qt_5_0);
    qint32 steramVersion;
    reader >> steramVersion;
    if (steramVersion > QDataStream().version()) {
#if QT_VERSION >= 0x050700
        reader.rollbackTransaction();
#endif
        return false;
    }
    reader.setVersion(steramVersion);
    QString header;
    reader >> header;
    if (header != Magic_Model_Header) {
#if QT_VERSION >= 0x050700
        reader.rollbackTransaction();
#endif
        return false;
    }
    if (!readBinaryElement(reader)) {
#if QT_VERSION >= 0x050700
        reader.rollbackTransaction();
#endif
        return false;
    }
    qint32 tempRole = -1;
    QVariant roleData;
    for (int i = 0; i < m_model->columnCount() && reader.status() == QDataStream::Ok; ++i) {
        for (reader >> tempRole; tempRole != -1 && reader.status() == QDataStream::Ok; reader >> tempRole) {
            reader >> roleData;
            if (reader.status() != QDataStream::Ok)
                break;
            m_model->setHeaderData(i, Qt::Horizontal, roleData, tempRole);
        }
    }
    if (reader.status() != QDataStream::Ok) {
        m_model->removeColumns(0, m_model->columnCount());
        m_model->removeRows(0, m_model->rowCount());
#if QT_VERSION >= 0x050700
        reader.rollbackTransaction();
#endif
        return false;
    }

    for (int i = 0; i < m_model->rowCount() && reader.status() == QDataStream::Ok; ++i) {
        for (reader >> tempRole; tempRole != -1 && reader.status() == QDataStream::Ok; reader >> tempRole) {
            reader >> roleData;
            if (reader.status() != QDataStream::Ok)
                break;
            m_model->setHeaderData(i, Qt::Vertical, roleData, tempRole);
        }
    }
    if (reader.status() != QDataStream::Ok) {
        m_model->removeColumns(0, m_model->columnCount());
        m_model->removeRows(0, m_model->rowCount());
#if QT_VERSION >= 0x050700
        reader.rollbackTransaction();
#endif
        return false;
    }
#if QT_VERSION >= 0x050700
    return reader.commitTransaction();
#else
    return true;
#endif
}

bool BinaryModelSerialiserPrivate::writeBinary(QDataStream &writer) const
{
    if (!m_constModel)
        return false;
    const qint32 writerVersion = writer.version();
    writer.setVersion(QDataStream::Qt_5_0);
    writer << writerVersion;
    writer.setVersion(writerVersion);
    writer << Magic_Model_Header;
    writeBinaryElement(writer);
    for (int i = 0; i < m_constModel->columnCount(); ++i) {
        for (QList<int>::const_iterator singleRole = m_rolesToSave.constBegin(); singleRole != m_rolesToSave.constEnd(); ++singleRole) {
            const QVariant roleData = m_constModel->headerData(i, Qt::Horizontal, *singleRole);
            if (roleData.isNull())
                continue;
            writer << static_cast<qint32>(*singleRole) << roleData;
        }
        writer << static_cast<qint32>(-1);
    }
    for (int i = 0; i < m_constModel->rowCount(); ++i) {
        for (QList<int>::const_iterator singleRole = m_rolesToSave.constBegin(); singleRole != m_rolesToSave.constEnd(); ++singleRole) {
            const QVariant roleData = m_constModel->headerData(i, Qt::Vertical, *singleRole);
            if (roleData.isNull())
                continue;
            writer << static_cast<qint32>(*singleRole) << roleData;
        }
        writer << static_cast<qint32>(-1);
    }
    return writer.status() == QDataStream::Ok;
}

/*!
\reimp
*/
bool BinaryModelSerialiser::saveModel(QIODevice *destination) const
{
    if (!destination)
        return false;
    if (!destination->isOpen()) {
        if (!destination->open(QIODevice::WriteOnly))
            return false;
    }
    if (!destination->isWritable())
        return false;
    Q_D(const BinaryModelSerialiser);

    if (!d->m_constModel)
        return false;
    QDataStream witer(destination);
    witer.setVersion(d->m_streamVersion);
    return d->writeBinary(witer);
}

/*!
\reimp
*/
bool BinaryModelSerialiser::saveModel(QByteArray *destination) const
{
    if (!destination)
        return false;
    Q_D(const BinaryModelSerialiser);

    if (!d->m_constModel)
        return false;
    QDataStream witer(destination, QIODevice::WriteOnly);
    witer.setVersion(d->m_streamVersion);
    return d->writeBinary(witer);
}

/*!
Saves the model to the given \a stream
*/
bool BinaryModelSerialiser::saveModel(QDataStream &stream) const
{
    Q_D(const BinaryModelSerialiser);
    return d->writeBinary(stream);
}

/*!
\reimp
*/
bool BinaryModelSerialiser::loadModel(QIODevice *source)
{
    if (!source)
        return false;
    if (!source->isOpen()) {
        if (!source->open(QIODevice::ReadOnly))
            return false;
    }
    if (!source->isReadable())
        return false;
    Q_D(BinaryModelSerialiser);
    if (!d->m_model)
        return false;
    QDataStream reader(source);
    reader.setVersion(d->m_streamVersion);
    return d->readBinary(reader);
}

/*!
\reimp
*/
bool BinaryModelSerialiser::loadModel(const QByteArray &source)
{
    Q_D(BinaryModelSerialiser);

    if (!d->m_model)
        return false;
    QDataStream reader(source);
    reader.setVersion(streamVersion());
    return d->readBinary(reader);
}

/*!
Loads the model from the given \a stream

Data previously stored in the model will be removed
*/
bool BinaryModelSerialiser::loadModel(QDataStream &stream)
{
    Q_D(BinaryModelSerialiser);
    return d->readBinary(stream);
}



/*!
Constructs a serialiser operating over \a model
*/
BinaryModelSerialiser::BinaryModelSerialiser(QAbstractItemModel *model, QObject *parent)
    : AbstractModelSerialiser(*new BinaryModelSerialiserPrivate(this), parent)
{
    setModel(model);
}

/*!
\overload

the model will only be allowed to be saved, not loaded
*/
BinaryModelSerialiser::BinaryModelSerialiser(const QAbstractItemModel *model, QObject *parent)
    : AbstractModelSerialiser(*new BinaryModelSerialiserPrivate(this), parent)
{
    setModel(model);
}

/*!
\internal
*/
BinaryModelSerialiser::BinaryModelSerialiser(BinaryModelSerialiserPrivate &d, QObject *parent)
    : AbstractModelSerialiser(d, parent)
{ }

/*!
Destructor
*/
BinaryModelSerialiser::~BinaryModelSerialiser() = default;

#ifdef MS_DECLARE_STREAM_OPERATORS

QDataStream &operator<<(QDataStream &stream, const QAbstractItemModel &model)
{
    const QModelSerialiser mSer(&model);
    mSer.d_func()->writeBinary(stream);
    return stream;
}

QDataStream &operator>>(QDataStream &stream, QAbstractItemModel &model)
{
    QModelSerialiser mSer(&model);
    mSer.d_func()->readBinary(stream);
    return stream;
}

#endif

/*!
\class BinaryModelSerialiser

\brief Serialiser to save and load models in binary format
*/
