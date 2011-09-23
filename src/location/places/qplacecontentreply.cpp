/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
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

#include "qplacecontentreply.h"
#include "qplacereply_p.h"

QT_BEGIN_NAMESPACE
class QPlaceContentReplyPrivate : public QPlaceReplyPrivate
{
public:
    QPlaceContentReplyPrivate()
    :   totalCount(0)
    { }

    QPlaceContent::Collection contentCollection;
    int totalCount;
};

QT_END_NAMESPACE

QT_USE_NAMESPACE

/*!
    \class QPlaceConentReply

    \brief The QPlaceContentReply class manages a content retrieval operation started by an
    instance of QPlaceManager.

    \inmodule QtLocation

    \ingroup places-main
*/

/*!
    Constructs a content reply with a given \a parent.
*/
QPlaceContentReply::QPlaceContentReply(QObject *parent)
    : QPlaceReply(new QPlaceContentReplyPrivate, parent)
{
}

/*!
    Destroys the reply.
*/
QPlaceContentReply::~QPlaceContentReply()
{
}

 /*!
    Returns the collection of content retrieved.
*/
QPlaceContent::Collection QPlaceContentReply::content() const
{
    Q_D(const QPlaceContentReply);
    return d->contentCollection;
}

/*!
    Returns the type of reply.
*/
QPlaceReply::Type QPlaceContentReply::type() const
{
    return QPlaceReply::ContentReply;
}

/*!
    Sets the \a content of the reply.
*/
void QPlaceContentReply::setContent(const QPlaceContent::Collection &content)
{
    Q_D(QPlaceContentReply);
    d->contentCollection = content;
}

/*!
    Returns the total number of content objects for a place.  If the total number of
    content objects cannot be counted, a value of -1 is returned.  This count only
    refers to the total count for a single content type i.e. the content type that
    was specified when content was requested with the QPlaceManager.
*/
int QPlaceContentReply::totalCount() const
{
    Q_D(const QPlaceContentReply);
    return d->totalCount;
}

/*!
    Sets the \a total number of content objects for a place.
*/
void QPlaceContentReply::setTotalCount(int total)
{
    Q_D(QPlaceContentReply);
    d->totalCount = total;
}