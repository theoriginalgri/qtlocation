
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
#ifndef ERROR_MESSAGES_H
#define ERROR_MESSAGES_H

#include <QString>

QT_BEGIN_NAMESPACE

extern const char CONTEXT_NAME[];

extern const char PLUGIN_DOESNOT_SUPPORT_MAPPING[];
extern const char PLUGIN_DOESNOT_SUPPORT_ROUTING[];
extern const char PLUGIN_DOESNOT_SUPPORT_GEOCODING[];
extern const char PLUGIN_DOESNOT_SUPPORT_PLACES[];
extern const char PLUGIN_DOESNOT_SUPPORT_PLACES2[];

extern const char PLUGIN_PROPERTY_NOT_SET[];
extern const char PLUGIN_NOT_VALID[];
extern const char PLUGIN_NOT_ASSIGNED_TO_PLACE[];
extern const char PLUGIN_NOT_ASSIGNED_TO_PLACE_ICON[];
extern const char CATEGORIES_NOT_INITIALIZED[];
extern const char NOT_INSTANTIABLE_BY_DEVELOPER[];

extern const char UNSUPPORTED_BOUND_TYPE[];
extern const char UNSUPPORTED_QUERY_TYPE[];
extern const char PLUGIN_SET_ONCE[];

extern const char PINCH_ROTATE_GESTURE_ACTIVATED[];
extern const char PINCH_TILT_GESTURE_ACTIVATED[];

extern const char INDEX_INVALID[];
extern const char INDEX_OUT_OF_RANGE[];
extern const char MODEL_INDEX_INVALID[];
extern const char MODEL_INDEX_OUT_OF_RANGE[];

extern const char GEOCODE_PLUGIN_NOT_SET[];
extern const char GEOCODE_MGR_NOT_SET[];
extern const char GEOCODE_QUERY_NOT_SET[];

extern const char ROUTE_PLUGIN_NOT_SET[];
extern const char ROUTE_MGR_NOT_SET[];
extern const char ROUTE_QUERY_NOT_SET[];
extern const char ROUTE_WAYPOINTS_NOT_SET[];

extern const char CANNOT_REMOVE_AREA[];
extern const char CANNOT_ADD_NULL_WAYPOINT[];
extern const char CANNOT_ADD_INVALID_WAYPOINT[];
extern const char CANNOT_REMOVE_WAYPOINT[];
extern const char COORD_NOT_BELONG_TO[];
extern const char MISSED_NMEA_FILE[];

QT_END_NAMESPACE

#endif // ERROR_MESSAGES_H