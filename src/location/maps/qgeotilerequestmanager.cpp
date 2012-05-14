/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qgeotilerequestmanager_p.h"

#include <QSharedPointer>
#include "qgeotilespec.h"
#include "qgeotiledmapdata_p.h"
#include "qgeotiledmappingmanagerengine.h"
#include "qgeotilecache_p.h"

QT_BEGIN_NAMESPACE

class RetryFuture;

class QGeoTileRequestManagerPrivate
{
public:
    explicit QGeoTileRequestManagerPrivate(QGeoTiledMapData *map);
    ~QGeoTileRequestManagerPrivate();

    QGeoTiledMapData *map_;
    QGeoTileCache *cache_;

    QList<QSharedPointer<QGeoTileTexture> > requestTiles(const QSet<QGeoTileSpec> &tiles);
    void tileError(const QGeoTileSpec &tile, const QString &errorString);

    QHash<QGeoTileSpec, int> retries_;
    QHash<QGeoTileSpec, QSharedPointer<RetryFuture> > futures_;
    QSet<QGeoTileSpec> visible_;
    QSet<QGeoTileSpec> requested_;

    void tileFetched(QSharedPointer<QGeoTileTexture> texture);
};

QGeoTileRequestManager::QGeoTileRequestManager(QGeoTiledMapData *map)
    : d_ptr(new QGeoTileRequestManagerPrivate(map))
{
}

QGeoTileRequestManager::~QGeoTileRequestManager()
{
    delete d_ptr;
}

QList<QSharedPointer<QGeoTileTexture> > QGeoTileRequestManager::requestTiles(const QSet<QGeoTileSpec> &tiles)
{
    Q_D(QGeoTileRequestManager);
    return d->requestTiles(tiles);
}

void QGeoTileRequestManager::tileFetched(QSharedPointer<QGeoTileTexture> texture)
{
    Q_D(QGeoTileRequestManager);
    d->tileFetched(texture);
}

void QGeoTileRequestManager::tileError(const QGeoTileSpec &tile, const QString &errorString)
{
    Q_D(QGeoTileRequestManager);
    d->tileError(tile, errorString);
}

QGeoTileRequestManagerPrivate::QGeoTileRequestManagerPrivate(QGeoTiledMapData *map)
    : map_(map)
{
}

QGeoTileRequestManagerPrivate::~QGeoTileRequestManagerPrivate()
{
}

QList<QSharedPointer<QGeoTileTexture> > QGeoTileRequestManagerPrivate::requestTiles(const QSet<QGeoTileSpec> &tiles)
{
    QSet<QGeoTileSpec> cancelTiles = requested_ - tiles;
    QSet<QGeoTileSpec> requestTiles = tiles - visible_ - requested_;
    QSet<QGeoTileSpec> cached;

    typedef QSet<QGeoTileSpec>::const_iterator iter;

    QList<QSharedPointer<QGeoTileTexture> > cachedTex;

    QGeoTiledMappingManagerEngine *engine = map_ ?
                static_cast<QGeoTiledMappingManagerEngine*>(map_->engine()) : 0;

    // remove tiles in cache from request tiles
    if (engine){
        iter i = requestTiles.constBegin();
        iter end = requestTiles.constEnd();
        for (; i != end; ++i) {
            QGeoTileSpec tile = *i;
            QSharedPointer<QGeoTileTexture> tex = engine->getTileTexture(tile);
            if (tex) {
                cachedTex << tex;
                cached.insert(tile);
            }
        }
    }

    requestTiles -= cached;

    visible_ = tiles;

    requested_ -= cancelTiles;
    requested_ += requestTiles;

    if (!requestTiles.isEmpty() || !cancelTiles.isEmpty()) {
        if (engine) {
            engine->updateTileRequests(map_, requestTiles, cancelTiles);

            // Remove any cancelled tiles from the error retry hash to avoid
            // re-using the numbers for a totally different request cycle.
            iter i = cancelTiles.constBegin();
            iter end = cancelTiles.constEnd();
            for (; i != end; ++i) {
                retries_.remove(*i);
                futures_.remove(*i);
            }
        }
    }

    return cachedTex;
}

void QGeoTileRequestManagerPrivate::tileFetched(QSharedPointer<QGeoTileTexture> texture)
{
    map_->newTileFetched(texture);
    requested_.remove(texture->spec);
    retries_.remove(texture->spec);
    futures_.remove(texture->spec);
}

// Represents a tile that needs to be retried after a certain period of time
class RetryFuture : public QObject
{
    Q_OBJECT
public:
    RetryFuture(const QGeoTileSpec &tile, QGeoTiledMapData *map, QObject *parent=0);

public Q_SLOTS:
    void retry();

private:
    QGeoTileSpec tile_;
    QGeoTiledMapData *map_;
};

RetryFuture::RetryFuture(const QGeoTileSpec &tile, QGeoTiledMapData *map, QObject *parent)
    : QObject(parent), tile_(tile), map_(map)
{}

void RetryFuture::retry()
{
    QSet<QGeoTileSpec> requestTiles;
    QSet<QGeoTileSpec> cancelTiles;
    requestTiles.insert(tile_);
    if (map_)
    {
        QGeoTiledMappingManagerEngine *engine =
                static_cast<QGeoTiledMappingManagerEngine*>(map_->engine());
        engine->updateTileRequests(map_, requestTiles, cancelTiles);
    }
}

void QGeoTileRequestManagerPrivate::tileError(const QGeoTileSpec &tile, const QString &errorString)
{
    if (requested_.contains(tile)) {
        int count = retries_.value(tile, 0);
        retries_.insert(tile, count + 1);

        if (count >= 5) {
            qWarning("QGeoTileRequestManager: Failed to fetch tile (%d,%d,%d) 5 times, giving up. "
                     "Last error message was: '%s'",
                     tile.x(), tile.y(), tile.zoom(), qPrintable(errorString));
            requested_.remove(tile);
            retries_.remove(tile);
            futures_.remove(tile);

        } else {
            // Exponential time backoff when retrying
            int delay = (1 << count) * 500;

            QSharedPointer<RetryFuture> future(new RetryFuture(tile, map_));
            futures_.insert(tile, future);

            QTimer::singleShot(delay, future.data(), SLOT(retry()));
            // Passing .data() to singleShot is ok -- Qt will clean up the
            // connection if the target qobject is deleted
        }
    }
}

#include "qgeotilerequestmanager.moc"

QT_END_NAMESPACE