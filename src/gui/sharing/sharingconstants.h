/*
 * SPDX-FileCopyrightText: 2026 Nextcloud GmbH and Nextcloud contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "common/qtcompat.h"

#include QLatin1StringViewOrStringHeader

namespace OCC::Gui::Sharing::SourceTypeClasses
{

inline constexpr auto node = QLatin1StringOrView{"OCA\\Files\\Sharing\\Source\\NodeShareSourceType"};

}

namespace OCC::Gui::Sharing::RecipientTypeClasses
{

inline constexpr auto email = QLatin1StringOrView{"OC\\Core\\Sharing\\Recipient\\EmailShareRecipientType"};
inline constexpr auto group = QLatin1StringOrView{"OC\\Core\\Sharing\\Recipient\\GroupShareRecipientType"};
inline constexpr auto team = QLatin1StringOrView{"OC\\Core\\Sharing\\Recipient\\TeamShareRecipientType"};
inline constexpr auto token = QLatin1StringOrView{"OC\\Core\\Sharing\\Recipient\\TokenShareRecipientType"};
inline constexpr auto user = QLatin1StringOrView{"OC\\Core\\Sharing\\Recipient\\UserShareRecipientType"};

}
