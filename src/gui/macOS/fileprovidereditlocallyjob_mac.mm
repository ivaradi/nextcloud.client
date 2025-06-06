/*
 * Copyright (C) by Claudio Cambra <claudio.cambra@nextcloud.com>
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

#include "fileprovidereditlocallyjob.h"

#include <QLoggingCategory>

#include "account.h"
#include "accountstate.h"
#include "editlocallymanager.h"
#include "systray.h"

#include "macOS/fileprovider.h"
#include "macOS/fileproviderdomainmanager.h"

#import <Cocoa/Cocoa.h>
#import <FileProvider/FileProvider.h>

namespace OCC::Mac {

Q_LOGGING_CATEGORY(lcFileProviderEditLocallyMacJob, 
                   "nextcloud.gui.fileprovidereditlocallymac",
                   QtInfoMsg)

void FileProviderEditLocallyJob::openFileProviderFile(const QString &ocId)
{
    qCDebug(lcFileProviderEditLocallyMacJob) << "Opening file provider file with OC ID" << ocId;
    
    const auto nsOcId = ocId.toNSString();
    const auto userId = _accountState->account()->userIdAtHostWithPort();
    const auto ncDomainManager = FileProvider::instance()->domainManager();
    const auto voidDomain = ncDomainManager->domainForAccount(_accountState.data());
    
    NSFileProviderDomain *const domain = (NSFileProviderDomain *)voidDomain;
    if (domain == nil) {
        qCWarning(lcFileProviderEditLocallyMacJob) << "Could not get domain for account:" << userId;
        emit notAvailable();
    }

    NSFileProviderManager *const manager = [NSFileProviderManager managerForDomain:domain]; 
    if (manager == nil) {
        qCWarning(lcFileProviderEditLocallyMacJob) << "Could not get file provider manager"
                                                      "for domain of account:" << userId;;
        emit notAvailable();
    }

    [manager retain];
    [manager getUserVisibleURLForItemIdentifier:nsOcId
                              completionHandler:^(NSURL *const url, NSError *const error) {

        dispatch_async(dispatch_get_main_queue(), ^{
            Systray::instance()->destroyEditFileLocallyLoadingDialog();
        });

        if (error != nil) {
            const auto errorMessage = QString::fromNSString(error.localizedDescription);
            qCWarning(lcFileProviderEditLocallyMacJob) << "Error getting user visible URL for item:" << errorMessage;
            dispatch_async(dispatch_get_main_queue(), ^{
                emit notAvailable();
            });
        } else if (url != nil) {
            const auto itemLocalPath = QString::fromNSString(url.path);
            qCDebug(lcFileProviderEditLocallyMacJob) << "Got user visible URL for item:" << itemLocalPath;
            [NSWorkspace.sharedWorkspace openURL:url];
            dispatch_async(dispatch_get_main_queue(), ^{
                emit finished();
            });
        } else {
            qCWarning(lcFileProviderEditLocallyMacJob) << "Got nil user visible URL for item" << ocId;
            dispatch_async(dispatch_get_main_queue(), ^{
                emit notAvailable();
            });
        }
        [manager release];
    }];
}

} // namespace OCC::Mac
