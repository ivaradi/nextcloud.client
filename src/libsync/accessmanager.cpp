/*
 * Copyright (C) by Krzesimir Nowak <krzesimir@endocode.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#include <QLoggingCategory>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QAuthenticator>
#include <QSslConfiguration>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkInformation>
#include <QUuid>

#include "cookiejar.h"
#include "accessmanager.h"
#include "common/utility.h"
#include "httplogger.h"

namespace OCC {

Q_LOGGING_CATEGORY(lcAccessManager, "nextcloud.sync.accessmanager", QtInfoMsg)

AccessManager::AccessManager(QObject *parent)
    : QNetworkAccessManager(parent)
{
    setCookieJar(new CookieJar);
    connect(this, &QNetworkAccessManager::authenticationRequired, this, [](QNetworkReply *reply, QAuthenticator *authenticator) {
        Q_UNUSED(reply)

        if (authenticator->user().isEmpty()) {
            qCWarning(lcAccessManager) << "Server requested authentication and we didn't provide a user";
            authenticator->setUser(QUuid::createUuid().toString());
        }
    });
}

QByteArray AccessManager::generateRequestId()
{
    return QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
}

QNetworkReply *AccessManager::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    QNetworkRequest newRequest(request);

    // Respect request specific user agent if any
    if (!newRequest.header(QNetworkRequest::UserAgentHeader).isValid()) {
        newRequest.setHeader(QNetworkRequest::UserAgentHeader, Utility::userAgentString());
    }

    // Some firewalls reject requests that have a "User-Agent" but no "Accept" header
    newRequest.setRawHeader(QByteArray("Accept"), "*/*");

    QByteArray verb = newRequest.attribute(QNetworkRequest::CustomVerbAttribute).toByteArray();
    // For PROPFIND (assumed to be a WebDAV op), set xml/utf8 as content type/encoding
    // This needs extension
    if (verb == "PROPFIND") {
        newRequest.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml; charset=utf-8"));
    }

    // Generate a new request id
    QByteArray requestId = generateRequestId();
    qInfo(lcAccessManager) << op << verb << newRequest.url().toString() << "has X-Request-ID" << requestId;
    newRequest.setRawHeader("X-Request-ID", requestId);

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 4)
    // only enable HTTP2 with Qt 5.9.4 because old Qt have too many bugs (e.g. QTBUG-64359 is fixed in >= Qt 5.9.4)
    if (newRequest.url().scheme() == "https") { // Not for "http": QTBUG-61397
        // http2 seems to cause issues, as with our recommended server setup we don't support http2, disable it by default for now
        static const bool http2EnabledEnv = qEnvironmentVariableIntValue("OWNCLOUD_HTTP2_ENABLED") == 1;

        newRequest.setAttribute(QNetworkRequest::Http2AllowedAttribute, http2EnabledEnv);
    }
#endif

    const auto reply = QNetworkAccessManager::createRequest(op, newRequest, outgoingData);
    HttpLogger::logRequest(reply, op, outgoingData);
    return reply;
}

} // namespace OCC
