/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativelocationtestmodel_p.h"
#include <qgeocoordinate.h>
#include <QtWidgets/QApplication>
#include <QtCore/QTime>
#include <QtCore>

QDeclarativeLocationTestModel::QDeclarativeLocationTestModel(QObject *parent):
     QAbstractListModel(parent),
    delay_(0),
    datacount_(0),
    componentCompleted_(false),
    crazyLevel_(0),
    crazyMode_(false)
{
    // seed crazy random generator
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()) + QCoreApplication::applicationPid());
    timer_.setSingleShot(true);
    connect(&timer_, SIGNAL(timeout()), this, SLOT(timerFired()));
    // Establish role names so that they can be queried from this model
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(TestDataRole, "modeldata");
    setRoleNames(roleNames);
}

QDeclarativeLocationTestModel::~QDeclarativeLocationTestModel()
{
    if (timer_.isActive())
        timer_.stop();
    if (!dataobjects_.isEmpty()) {
        qDeleteAll(dataobjects_);
        dataobjects_.clear();
    }
}

void QDeclarativeLocationTestModel::timerFired()
{
    //qDebug() << "timer fired" ;
    repopulate();
    if (crazyMode_) {
        //qDebug() << "raw randomw value: " << qrand();
        int delay = (qAbs(qrand()) % crazyLevel_); // writing software is exact science
        delay = qMax(1000, delay); // 3 ms at minimum
        qDebug() << "starting timer with : " << delay;
        timer_.start(delay);
    }
}

void QDeclarativeLocationTestModel::componentComplete()
{
    componentCompleted_ = true;
    scheduleRepopulation();
}


int QDeclarativeLocationTestModel::datacount() const
{
    return datacount_;
}

void QDeclarativeLocationTestModel::setDatacount(int datacount)
{
    if (datacount_ == datacount)
        return;
    datacount_ = datacount;
    emit datacountChanged();
    scheduleRepopulation();
}

int QDeclarativeLocationTestModel::delay() const
{
    return delay_;
}

void QDeclarativeLocationTestModel::setDelay(int delay)
{
    if (delay_ == delay)
        return;
    delay_ = delay;
    emit delayChanged();
}

QString QDeclarativeLocationTestModel::datatype() const
{
    return datatype_;
}

void QDeclarativeLocationTestModel::setDatatype(QString datatype)
{
    if (datatype_ == datatype)
        return;
    datatype_ = datatype;
    emit datatypeChanged();
    scheduleRepopulation();
}

int QDeclarativeLocationTestModel::crazyLevel() const
{
    return crazyLevel_;
}

void QDeclarativeLocationTestModel::setCrazyLevel(int level)
{
    if (level == crazyLevel_)
        return;
    crazyLevel_ = level;
    reset();
    scheduleRepopulation();
    emit crazyLevelChanged();
}

bool QDeclarativeLocationTestModel::crazyMode() const
{
    return crazyMode_;
}

void QDeclarativeLocationTestModel::setCrazyMode(bool mode)
{
    if (mode == crazyMode_)
        return;
    crazyMode_ = mode;
    //if (!crazyMode_)
        //reset();
    //else
    if (crazyMode_)
        scheduleRepopulation();
    emit crazyModeChanged();
}

// only coordinate datatype for now to get started with,
// refactor if more usecases for the model emerge.
void QDeclarativeLocationTestModel::repopulate()
{
    double latitude = -30;
    double longitude = 153;
    beginResetModel();
    if (!dataobjects_.isEmpty()) {
        qDeleteAll(dataobjects_);
        dataobjects_.clear();
    }
    int datacount = datacount_;
    if (crazyMode_)
        datacount = (qAbs(qrand()) % datacount_);

    qDebug() << "generating random content: " << datacount;

    for (int i = 0; i < datacount; ++i) {
        DataObject* dataobject = new DataObject;
        QDeclarativeCoordinate* coordinate = new QDeclarativeCoordinate(QGeoCoordinate(latitude,longitude));
        dataobject->coordinate_ = coordinate;
        dataobjects_.append(dataobject);
        longitude -= 0.2;
        latitude += 0.2;
    }
    endResetModel();
}

void QDeclarativeLocationTestModel::reset()
{
    if (timer_.isActive())
        timer_.stop();
    beginResetModel();
    if (!dataobjects_.isEmpty()) {
        qDeleteAll(dataobjects_);
        dataobjects_.clear();
    }
    endResetModel();
}

void QDeclarativeLocationTestModel::scheduleRepopulation()
{
    if (!componentCompleted_)
        return;
    if (datacount_ <= 0) {
        qDebug() << __FUNCTION__ << "won't schedule model, invalid datacount: " << datacount_;
        return;
    }

    if (timer_.isActive())
        timer_.stop();

    if (crazyMode_) {
        // start generating arbitrary amount of data at arbitrary intervals
        int delay = (qAbs(qrand()) % crazyLevel_); // writing software is exact science
        delay = qMax(3, delay); // 3 ms at minimum
        qDebug() << "starting timer with : " << delay;
        timer_.start(delay);
    } else {
        // just update
        if (delay_ > 0)
            timer_.start(delay_);
        else
            repopulate();
    }
}

int QDeclarativeLocationTestModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return dataobjects_.count();
}

// Returns the stored under the given role for the item referred to by the index.
QVariant QDeclarativeLocationTestModel::data(const QModelIndex& index, int role) const
{
    switch (role) {
    case TestDataRole:
        if (dataobjects_.at(index.row())) {
            return QVariant::fromValue(qobject_cast<QObject*>(dataobjects_.at(index.row())));
        }
        break;
    }
    return QVariant();
}