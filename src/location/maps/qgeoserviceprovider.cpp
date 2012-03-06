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

#include "qgeoserviceprovider.h"
#include "qgeoserviceprovider_p.h"
#include "qgeoserviceproviderfactory.h"

#include "qgeocodingmanager.h"
#include "qgeomappingmanager.h"
#include "qgeoroutingmanager.h"
#include "qplacemanager.h"
#include "qgeocodingmanagerengine.h"
#include "qgeomappingmanagerengine.h"
#include "qgeoroutingmanagerengine.h"
#include "qplacemanagerengine.h"
#include "qplacemanagerengine_p.h"

#include <QList>
#include <QString>
#include <QVariant>

#include <QPluginLoader>
#include <QDebug>
#include <QStringList>
#include <QCoreApplication>
#include <QObject>
#include <QMetaObject>
#include <QMetaEnum>
#include <QProcess>
#include <QEventLoop>
#include <QString>
#include <QtCore/private/qfactoryloader_p.h>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_LIBRARY
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
        ("org.qt-project.qt.geoservice.serviceproviderfactory/5.0",
         QLatin1String("/geoservices")))
#endif

/*!
    \class QGeoServiceProvider
    \inmodule QtLocation
    \ingroup QtLocation-common
    \since QtLocation 5.0

    \brief The QGeoServiceProvider class aggregates access to services which provide
    geographical information.

    The Maps and Navigation API allows people to access various kinds of
    geographical information, including functionality to perform geocoding,
    routing and the display of maps.  The QGeoServiceProvider aggregates the
    access to a set of these services that are provided by a single vendor.

    It is possible to mix and match service providers for the various domains,
    so that a geocoding manager from one service provider can be used with
    a geographic routing manager from another service provider.

    This is not recommended unless the client is able to verify that the
    data provided by the different services are compatible, as differences
    in the underlying data sets could cause serious incongruences between
    the services.

    Subclasses of QGeoServiceProvider guarantee that the different services
    that they provide are interoperable.

    At this point only the Nokia Services plugin is packaged with Qt,
    which is accessible using the provider name "nokia".
*/

/*!
\enum QGeoServiceProvider::Error

Describes an error related to the loading and setup of a service provider
plugin.

\value NoError
No error has occurred.

\value NotSupportedError
The plugin does not support this functionality.

\value UnknownParameterError
The plugin did not recognize one of the parameters it was given.

\value MissingRequiredParameterError
The plugin did not find one of the parameters it was expecting.

\value ConnectionError
The plugin could not connect to its backend service or database.
*/

/*!
    Returns a list of names of the available service providers, for use with
    the QGeoServiceProvider constructors.
*/
QStringList QGeoServiceProvider::availableServiceProviders()
{
    return QGeoServiceProviderPrivate::plugins().keys();
}

/*!
    Constructs a QGeoServiceProvider whose backend has the name \a
    providerName, using the provided \a parameters.

    If multiple plugins have the same \a providerName, the plugin with the
    highest reported providerVersion() will be used.

    If no plugin matching \a providerName was able to be loaded then error()
    and errorString() will provide details about why this is the case.
*/
QGeoServiceProvider::QGeoServiceProvider(const QString &providerName, const QMap<QString, QVariant> &parameters, bool experimental)
    : d_ptr(new QGeoServiceProviderPrivate())
{
    d_ptr->experimental = experimental;
    d_ptr->parameterMap = parameters;
    d_ptr->providerName = providerName;
    d_ptr->loadMeta();
}

/*!
    Destroys the service provider object.
*/
QGeoServiceProvider::~QGeoServiceProvider()
{
    delete d_ptr;
}

QGeoServiceProvider::RoutingFeatures QGeoServiceProvider::routingFeatures() const
{
    const QMetaObject *mo = QGeoServiceProvider::metaObject();
    const QMetaEnum en = mo->enumerator(
                mo->indexOfEnumerator("RoutingFeatures"));

    RoutingFeatures ret = NoRoutingFeatures;
    if (d_ptr->metaData.contains(QStringLiteral("Features"))
            && d_ptr->metaData.value(QStringLiteral("Features")).isArray()) {
        QJsonArray features = d_ptr->metaData.value(QStringLiteral("Features")).toArray();
        foreach (QJsonValue v, features) {
            int val = en.keyToValue(v.toString().toAscii().constData());
            if (v.isString() && val != -1) {
                ret |= RoutingFeature(val);
            }
        }
    }

    return ret;
}

QGeoServiceProvider::GeocodingFeatures QGeoServiceProvider::geocodingFeatures() const
{
    const QMetaObject *mo = QGeoServiceProvider::metaObject();
    const QMetaEnum en = mo->enumerator(
                mo->indexOfEnumerator("GeocodingFeatures"));

    GeocodingFeatures ret = NoGeocodingFeatures;
    if (d_ptr->metaData.contains(QStringLiteral("Features"))
            && d_ptr->metaData.value(QStringLiteral("Features")).isArray()) {
        QJsonArray features = d_ptr->metaData.value(QStringLiteral("Features")).toArray();
        foreach (QJsonValue v, features) {
            int val = en.keyToValue(v.toString().toAscii().constData());
            if (v.isString() && val != -1) {
                ret |= GeocodingFeature(val);
            }
        }
    }

    return ret;
}

QGeoServiceProvider::MappingFeatures QGeoServiceProvider::mappingFeatures() const
{
    const QMetaObject *mo = QGeoServiceProvider::metaObject();
    const QMetaEnum en = mo->enumerator(
                mo->indexOfEnumerator("MappingFeatures"));

    MappingFeatures ret = NoMappingFeatures;
    if (d_ptr->metaData.contains(QStringLiteral("Features"))
            && d_ptr->metaData.value(QStringLiteral("Features")).isArray()) {
        QJsonArray features = d_ptr->metaData.value(QStringLiteral("Features")).toArray();
        foreach (QJsonValue v, features) {
            int val = en.keyToValue(v.toString().toAscii().constData());
            if (v.isString() && val != -1) {
                ret |= MappingFeature(val);
            }
        }
    }

    return ret;
}

QGeoServiceProvider::PlacesFeatures QGeoServiceProvider::placesFeatures() const
{
    const QMetaObject *mo = QGeoServiceProvider::metaObject();
    const QMetaEnum en = mo->enumerator(
                mo->indexOfEnumerator("PlacesFeatures"));

    PlacesFeatures ret = NoPlacesFeatures;
    if (d_ptr->metaData.contains(QStringLiteral("Features"))
            && d_ptr->metaData.value(QStringLiteral("Features")).isArray()) {
        QJsonArray features = d_ptr->metaData.value(QStringLiteral("Features")).toArray();
        foreach (QJsonValue v, features) {
            int val = en.keyToValue(v.toString().toAscii().constData());
            if (v.isString() && val != -1) {
                ret |= PlacesFeature(val);
            }
        }
    }

    return ret;
}

/*!
    Returns the QGeocodingManager made available by the service
    provider.

    This function will return 0 if the service provider does not provide
    any geocoding services.

    This function will attempt to construct a QGeocodingManager instance
    when it is called for the first time.  If the attempt is successful the
    QGeocodingManager will be cached, otherwise each call of this function
    will attempt to construct a QGeocodingManager instance until the
    construction is successful.

    The QGeocodingManager is owned by this QGeoServiceProvider and should not
    be deleted separately. Users should assume that deleting the
    QGeoServiceProvider renders the pointer returned by this method invalid.

    After this function has been called, error() and errorString() will
    report any errors which occurred during the construction of the
    QGeocodingManager.
*/
QGeocodingManager* QGeoServiceProvider::geocodingManager() const
{
    if (!d_ptr->factory)
        d_ptr->loadPlugin(d_ptr->parameterMap);

    if (!d_ptr->factory || d_ptr->geocodeError != QGeoServiceProvider::NoError)
        return 0;

    if (!d_ptr->geocodingManager) {
        QGeocodingManagerEngine *engine = d_ptr->factory->createGeocodingManagerEngine(d_ptr->parameterMap,
                                          &(d_ptr->geocodeError),
                                          &(d_ptr->geocodeErrorString));
        if (engine) {
            engine->setManagerName(
                        d_ptr->metaData.value(QStringLiteral("Provider")).toString());
            engine->setManagerVersion(
                        int(d_ptr->metaData.value(QStringLiteral("Version")).toDouble()));
            d_ptr->geocodingManager = new QGeocodingManager(engine);
        } else {
            d_ptr->geocodeError = QGeoServiceProvider::NotSupportedError;
            d_ptr->geocodeErrorString = QLatin1String("The service provider does not support geocodingManager().");
        }

        if (d_ptr->geocodeError != QGeoServiceProvider::NoError) {
            if (d_ptr->geocodingManager)
                delete d_ptr->geocodingManager;
            d_ptr->geocodingManager = 0;
            d_ptr->error = d_ptr->geocodeError;
            d_ptr->errorString = d_ptr->geocodeErrorString;
        }

        if (d_ptr->geocodingManager && d_ptr->localeSet)
            d_ptr->geocodingManager->setLocale(d_ptr->locale);
    }

    if (d_ptr->geocodingManager) {
        d_ptr->error = QGeoServiceProvider::NoError;
        d_ptr->errorString.clear();
    }

    return d_ptr->geocodingManager;
}

/*!
    Returns the QGeoMappingManager made available by the service provider.

    This function will return 0 if the service provider does not provide
    any mapping services.

    This function will attempt to construct a QGeoMappingManager instance
    when it is called for the first time.  If the attempt is successful the
    QGeoMappingManager will be cached, otherwise each call of this function
    will attempt to construct a QGeoMappingManager instance until the
    construction is successful.

    The QGeoMappingManager is owned by this QGeoServiceProvider and should not
    be deleted separately. Users should assume that deleting the
    QGeoServiceProvider renders the pointer returned by this method invalid.

    After this function has been called, error() and errorString() will
    report any errors which occurred during the construction of the
    QGeoMappingManager.
*/
QGeoMappingManager* QGeoServiceProvider::mappingManager() const
{
    if (!d_ptr->factory)
        d_ptr->loadPlugin(d_ptr->parameterMap);

    if (!d_ptr->factory || (d_ptr->mappingError != QGeoServiceProvider::NoError))
        return 0;

    if (!d_ptr->mappingManager) {
        QGeoMappingManagerEngine *engine = d_ptr->factory->createMappingManagerEngine(d_ptr->parameterMap,
                                           &(d_ptr->mappingError),
                                           &(d_ptr->mappingErrorString));

        if (engine) {
            engine->setManagerName(
                        d_ptr->metaData.value(QStringLiteral("Provider")).toString());
            engine->setManagerVersion(
                        int(d_ptr->metaData.value(QStringLiteral("Version")).toDouble()));
            d_ptr->mappingManager = new QGeoMappingManager(engine);
        } else {
            d_ptr->mappingError = QGeoServiceProvider::NotSupportedError;
            d_ptr->mappingErrorString = QLatin1String("The service provider does not support mappingManager().");
        }

        if (d_ptr->mappingError != QGeoServiceProvider::NoError) {
            if (d_ptr->mappingManager)
                delete d_ptr->mappingManager;
            d_ptr->mappingManager = 0;
            d_ptr->error = d_ptr->mappingError;
            d_ptr->errorString = d_ptr->mappingErrorString;
        }

        if (d_ptr->mappingManager && d_ptr->localeSet)
            d_ptr->mappingManager->setLocale(d_ptr->locale);
    }

    if (d_ptr->mappingManager) {
        d_ptr->error = QGeoServiceProvider::NoError;
        d_ptr->errorString.clear();
    }

    return d_ptr->mappingManager;
}

/*!
    Returns the QGeoRoutingManager made available by the service provider.

    This function will return 0 if the service provider does not provide
    any geographic routing services.

    This function will attempt to construct a QGeoRoutingManager instance
    when it is called for the first time.  If the attempt is successful the
    QGeoRoutingManager will be cached, otherwise each call of this function
    will attempt to construct a QGeoRoutingManager instance until the
    construction is successful.

    The QGeoRoutingManager is owned by this QGeoServiceProvider and should not
    be deleted separately. Users should assume that deleting the
    QGeoServiceProvider renders the pointer returned by this method invalid.

    After this function has been called, error() and errorString() will
    report any errors which occurred during the construction of the
    QGeoRoutingManager.
*/
QGeoRoutingManager* QGeoServiceProvider::routingManager() const
{
    if (!d_ptr->factory)
        d_ptr->loadPlugin(d_ptr->parameterMap);

    if (!d_ptr->factory || (d_ptr->routingError != QGeoServiceProvider::NoError))
        return 0;

    if (!d_ptr->routingManager) {
        QGeoRoutingManagerEngine *engine = d_ptr->factory->createRoutingManagerEngine(d_ptr->parameterMap,
                                           &(d_ptr->routingError),
                                           &(d_ptr->routingErrorString));

        if (engine) {
            engine->setManagerName(
                        d_ptr->metaData.value(QStringLiteral("Provider")).toString());
            engine->setManagerVersion(
                        int(d_ptr->metaData.value(QStringLiteral("Version")).toDouble()));
            d_ptr->routingManager = new QGeoRoutingManager(engine);
        } else {
            d_ptr->routingError = QGeoServiceProvider::NotSupportedError;
            d_ptr->routingErrorString = QLatin1String("The service provider does not support routingManager().");
        }

        if (d_ptr->routingError != QGeoServiceProvider::NoError) {
            if (d_ptr->routingManager)
                delete d_ptr->routingManager;
            d_ptr->routingManager = 0;
            d_ptr->error = d_ptr->routingError;
            d_ptr->errorString = d_ptr->routingErrorString;
        }

        if (d_ptr->routingManager && d_ptr->localeSet)
            d_ptr->routingManager->setLocale(d_ptr->locale);
    }

    if (d_ptr->routingManager) {
        d_ptr->error = QGeoServiceProvider::NoError;
        d_ptr->errorString.clear();
    }

    return d_ptr->routingManager;
}

/*!
    Returns the QPlaceManager made available by the service provider.

    This function will return 0 if the service provider does not provide
    any place searching services.

    This function will attempt to construct a QPlaceManager instance
    when it is called for the first time.  If the attempt is successful the
    QPlaceManager will be cached, otherwise each call of this function
    will attempt to construct a QPlace instance until the
    construction is successful.

    The QGeoPlaceManager is owned by this QGeoServiceProvider and should not
    be deleted separately. Users should assume that deleting the
    QGeoServiceProvider renders the pointer returned by this method invalid.

    After this function has been called, error() and errorString() will
    report any errors which occurred during the construction of the QPlaceManager.
*/
QPlaceManager *QGeoServiceProvider::placeManager() const
{
    if (!d_ptr->factory)
        d_ptr->loadPlugin(d_ptr->parameterMap);

    if (!d_ptr->factory || (d_ptr->placeError != QGeoServiceProvider::NoError))
        return 0;

    if (!d_ptr->placeManager) {
        QPlaceManagerEngine *engine = d_ptr->factory->createPlaceManagerEngine(d_ptr->parameterMap,
                                           &(d_ptr->placeError),
                                           &(d_ptr->placeErrorString));

        if (engine) {
            engine->setManagerName(
                        d_ptr->metaData.value(QStringLiteral("Provider")).toString());
            engine->setManagerVersion(
                        int(d_ptr->metaData.value(QStringLiteral("Version")).toDouble()));
            d_ptr->placeManager = new QPlaceManager(engine);
            engine->d_ptr->manager = d_ptr->placeManager;

        } else {
            d_ptr->placeError = QGeoServiceProvider::NotSupportedError;
            d_ptr->placeErrorString = QLatin1String("The service provider does not support placeManager().");
        }

        if (d_ptr->placeError != QGeoServiceProvider::NoError) {
            if (d_ptr->placeManager)
                delete d_ptr->placeManager;
            d_ptr->placeManager = 0;
            d_ptr->error = d_ptr->placeError;
            d_ptr->errorString = d_ptr->placeErrorString;
        }

        if (d_ptr->placeManager && d_ptr->localeSet)
            d_ptr->placeManager->setLocale(d_ptr->locale);
    }

    if (d_ptr->placeManager) {
        d_ptr->error = QGeoServiceProvider::NoError;
        d_ptr->errorString.clear();
    }

    return d_ptr->placeManager;
}

/*!
    Returns an error code describing the error which occurred during the
    last operation that was performed by this class.
*/
QGeoServiceProvider::Error QGeoServiceProvider::error() const
{
    return d_ptr->error;
}

/*!
    Returns a string describing the error which occurred during the
    last operation that was performed by this class.
*/
QString QGeoServiceProvider::errorString() const
{
    return d_ptr->errorString;
}

/*!
    Sets whether experimental plugins are considered when locating the
    correct plugin library for this service provider.

    \bold{Important:} this will destroy any existing managers held by this
    service provider instance. You should be sure not to attempt to use any
    pointers that you have previously retrieved after calling this method.
*/
void QGeoServiceProvider::setAllowExperimental(bool allow)
{
    d_ptr->experimental = allow;
    d_ptr->unload();
    d_ptr->loadMeta();
}

/*!
    Sets the parameters used to construct individual manager classes for
    this service provider.

    \bold{Important:} this will destroy any existing managers held by this
    service provider instance. You should be sure not to attempt to use any
    pointers that you have previously retrieved after calling this method.
*/
void QGeoServiceProvider::setParameters(const QMap<QString, QVariant> &parameters)
{
    d_ptr->parameterMap = parameters;
    d_ptr->unload();
    d_ptr->loadMeta();
}

/*!
    Sets the locale used by this service provider. If the relevant features
    (see LocalizedMappingFeature etc), this will change the languages, units
    and other locale-specific attributes of the provider's data.
*/
void QGeoServiceProvider::setLocale(const QLocale &locale)
{
    d_ptr->locale = locale;
    d_ptr->localeSet = true;

    if (d_ptr->geocodingManager)
        d_ptr->geocodingManager->setLocale(locale);
    if (d_ptr->routingManager)
        d_ptr->routingManager->setLocale(locale);
    if (d_ptr->mappingManager)
        d_ptr->mappingManager->setLocale(locale);
    if (d_ptr->placeManager)
        d_ptr->placeManager->setLocale(locale);
}

/*******************************************************************************
*******************************************************************************/

QGeoServiceProviderPrivate::QGeoServiceProviderPrivate()
    : factory(0),
      experimental(false),
      geocodingManager(0),
      routingManager(0),
      mappingManager(0),
      placeManager(0),
      geocodeError(QGeoServiceProvider::NoError),
      routingError(QGeoServiceProvider::NoError),
      mappingError(QGeoServiceProvider::NoError),
      placeError(QGeoServiceProvider::NoError),
      error(QGeoServiceProvider::NoError),
      localeSet(false)
{
    metaData.insert(QStringLiteral("index"), -1);
}

QGeoServiceProviderPrivate::~QGeoServiceProviderPrivate()
{
    if (geocodingManager)
        delete geocodingManager;

    if (routingManager)
        delete routingManager;

    if (mappingManager)
        delete mappingManager;

    if (placeManager)
        delete placeManager;
}

void QGeoServiceProviderPrivate::unload()
{
    if (geocodingManager) {
        delete geocodingManager;
        geocodingManager = 0;
    }

    if (routingManager) {
        delete routingManager;
        routingManager = 0;
    }

    if (mappingManager) {
        delete mappingManager;
        mappingManager = 0;
    }

    if (placeManager) {
        delete placeManager;
        placeManager = 0;
    }

    factory = 0;
    error = QGeoServiceProvider::NoError;
    errorString = QLatin1String("");
    metaData = QJsonObject();
    metaData.insert(QStringLiteral("index"), -1);
}

void QGeoServiceProviderPrivate::loadMeta()
{
    factory = 0;
    metaData = QJsonObject();
    metaData.insert(QStringLiteral("index"), -1);
    error = QGeoServiceProvider::NotSupportedError;
    errorString = QString(QLatin1String("The geoservices provider %1 is not supported.")).arg(providerName);

    QList<QJsonObject> candidates = QGeoServiceProviderPrivate::plugins().values(providerName);

    int versionFound = -1;
    int idx = -1;

    // figure out which version of the plugin we want
    // (always latest unless experimental)
    for (int i = 0; i < candidates.size(); ++i) {
        QJsonObject meta = candidates[i];
        if (meta.contains(QStringLiteral("Version"))
                && meta.value(QStringLiteral("Version")).isDouble()
                && meta.contains(QStringLiteral("Experimental"))
                && meta.value(QStringLiteral("Experimental")).isBool()) {
            int ver = int(meta.value(QStringLiteral("Version")).toDouble());
            if (ver > versionFound && !(!experimental && meta.value(QStringLiteral("Experimental")).toBool())) {
                versionFound = ver;
                idx = i;
            }
        }
    }

    if (idx != -1) {
        error = QGeoServiceProvider::NoError;
        errorString = QStringLiteral("");
        metaData = candidates[idx];
    }
}

void QGeoServiceProviderPrivate::loadPlugin(const QMap<QString, QVariant> &parameters)
{
    Q_UNUSED(parameters)

    if (int(metaData.value(QStringLiteral("index")).toDouble()) < 0) {
        error = QGeoServiceProvider::NotSupportedError;
        errorString = QString(QLatin1String("The geoservices provider is not supported."));
        factory = 0;
        return;
    }

    error = QGeoServiceProvider::NoError;
    errorString = QLatin1String("");

    int idx = int(metaData.value(QStringLiteral("index")).toDouble());

    // load the actual plugin
    factory = qobject_cast<QGeoServiceProviderFactory*>(loader()->instance(idx));
}

QHash<QString, QJsonObject> QGeoServiceProviderPrivate::plugins(bool reload)
{
    static QHash<QString, QJsonObject> plugins;
    static bool alreadyDiscovered = false;

    if (reload  == true)
        alreadyDiscovered = false;

    if (!alreadyDiscovered) {
        loadPluginMetadata(plugins);
        alreadyDiscovered = true;
    }
    return plugins;
}

void QGeoServiceProviderPrivate::loadPluginMetadata(QHash<QString, QJsonObject> &list)
{
    QFactoryLoader *l = loader();
    QList<QJsonObject> meta = l->metaData();
    for (int i = 0; i < meta.size(); i++) {
        QJsonObject obj = meta.at(i).value(QStringLiteral("MetaData")).toObject();
        obj.insert(QStringLiteral("index"), i);
        list.insertMulti(obj.value(QStringLiteral("Provider")).toString(), obj);
    }
}


QT_END_NAMESPACE

