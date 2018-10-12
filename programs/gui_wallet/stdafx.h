#pragma once
#define STDAFX_H

#include <QtWidgets/QtWidgets>

#include <json.hpp>

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>

#include <fc/interprocess/signals.hpp>
#include <fc/thread/thread.hpp>

#include <graphene/account_history/account_history_plugin.hpp>
#include <graphene/chain/content_object.hpp>
#include <graphene/chain/transaction_detail_object.hpp>
#include <graphene/messaging/messaging.hpp>
#include <graphene/miner/miner.hpp>
#include <graphene/seeding/seeding.hpp>
#include <graphene/utilities/dirhelper.hpp>

#include <decent/config/decent_log_config.hpp>
#include <decent/wallet_utility/wallet_utility.hpp>

#ifdef _MSC_VER
#include <windows.h>
#endif
