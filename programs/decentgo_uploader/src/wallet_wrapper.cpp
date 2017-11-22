#include "wallet_wrapper.h"

#include <graphene/utilities/dirhelper.hpp>
#include <fc/rpc/websocket_api.hpp>
#include <fc/network/http/websocket.hpp>
#include <fc/filesystem.hpp>

#include <fc/rpc/api_connection.hpp>
#include <graphene/chain/protocol/types.hpp>
#include <graphene/app/api.hpp>

#include <stdexcept>

namespace decent
{
    using wallet_api = graphene::wallet::wallet_api;
    using wallet_api_ptr = std::shared_ptr<wallet_api>;
    using wallet_data = graphene::wallet::wallet_data;

    using websocket_client = fc::http::websocket_client;
    using websocket_client_ptr = std::shared_ptr<websocket_client>;
    using websocket_connection_ptr = fc::http::websocket_connection_ptr;
    using websocket_api_connection = fc::rpc::websocket_api_connection;
    using websocket_api_connection_ptr = std::shared_ptr<websocket_api_connection>;
    using fc_api = fc::api<wallet_api>;
    using fc_api_ptr = std::shared_ptr<fc_api>;
    using fc_remote_api = fc::api<graphene::app::login_api>;
    using fc_remote_api_ptr = std::shared_ptr<fc_remote_api>;

}

namespace private_detail
{
    class WalletAPIConnection : public fc::api_connection {  // no idea yet why deriving
    public:
       WalletAPIConnection() {}
       virtual ~WalletAPIConnection() {}

       virtual fc::variant send_call(fc::api_id_type api_id, string method_name, fc::variants args = fc::variants()) { FC_ASSERT(false); }
       virtual fc::variant send_callback(uint64_t callback_id, fc::variants args = fc::variants()) { FC_ASSERT(false); }
       virtual void send_notice(uint64_t callback_id, fc::variants args = fc::variants()) { FC_ASSERT(false); }
    };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

WalletWrapper::WalletWrapper()
{

}

WalletWrapper::~WalletWrapper()
{

}

bool WalletWrapper::Connect()
{
   graphene::wallet::wallet_data wdata;
   fc::path wallet_file(decent_path_finder::instance().get_decent_home() / "wallet.json");
   if (fc::exists(wallet_file))
      wdata = fc::json::from_file(wallet_file).as<graphene::wallet::wallet_data>();
   else
      wdata.chain_id = graphene::chain::chain_id_type("0000000000000000000000000000000000000000000000000000000000000000");

   //  most probably this needs to get out to somewhere else
   //graphene::package::package_manager::instance().set_libtorrent_config(wdata.libtorrent_config_path);

   decent::websocket_client_ptr ptr_ws_client(new decent::websocket_client());
   decent::websocket_connection_ptr ptr_ws_connection = ptr_ws_client->connect(wdata.ws_server);

   //  capture ptr_ws_connection and ptr_ws_client own the lifetime
   decent::websocket_api_connection_ptr ptr_api_connection =
         decent::websocket_api_connection_ptr(new decent::websocket_api_connection(*ptr_ws_connection),
                                      [ptr_ws_connection, ptr_ws_client](decent::websocket_api_connection* &p_api_connection) mutable
                                      {
                                          delete p_api_connection;
                                          p_api_connection = nullptr;
                                          ptr_ws_connection.reset();
                                          ptr_ws_client.reset();
                                      });

   decent::fc_remote_api_ptr ptr_remote_api =
         decent::fc_remote_api_ptr(new decent::fc_remote_api(ptr_api_connection->get_remote_api<graphene::app::login_api>(1)));
   if (! (*ptr_remote_api)->login(wdata.ws_user, wdata.ws_password)) {
      throw std::runtime_error("fc::api<graphene::app::login_api>::login");
   }

   //  capture ptr_api_connection and ptr_remote_api too. encapsulate all inside wallet_api
   m_ptr_wallet_api.reset(new decent::wallet_api(wdata, *ptr_remote_api),
                          [ptr_api_connection, ptr_remote_api](decent::wallet_api* &p_wallet_api) mutable
                          {
                              delete p_wallet_api;
                              p_wallet_api = nullptr;
                              ptr_api_connection.reset();
                              ptr_remote_api.reset();
                          });

   m_ptr_wallet_api->set_wallet_filename(wallet_file.generic_string());
   m_ptr_wallet_api->load_wallet_file();

   decent::fc_api_ptr ptr_fc_api = decent::fc_api_ptr(new decent::fc_api(m_ptr_wallet_api.get()));

//   for (auto& name_formatter : m_ptr_wallet_api->get_result_formatters())
//      m_result_formatters[name_formatter.first] = name_formatter.second;


   m_ptr_fc_api_connection = std::shared_ptr<private_detail::WalletAPIConnection>(new private_detail::WalletAPIConnection(),
                                                    [ptr_fc_api](private_detail::WalletAPIConnection* &pWAPICon) mutable
                                                    {
                                                        delete pWAPICon;
                                                        pWAPICon = nullptr;
                                                        ptr_fc_api.reset();
                                                    });
   m_ptr_fc_api_connection->register_api(*ptr_fc_api);

   return true;
}

bool WalletWrapper::Connected()
{
   return m_ptr_wallet_api.get() != nullptr;
}

bool WalletWrapper::IsNew()
{
   return m_ptr_wallet_api->is_new();
}

bool WalletWrapper::IsLocked()
{
   return m_ptr_wallet_api->is_locked();
}

std::chrono::system_clock::time_point WalletWrapper::HeadBlockTime()
{
   fc::time_point_sec time = m_ptr_wallet_api->head_block_time();
   return std::chrono::system_clock::from_time_t(time.sec_since_epoch());
}

void WalletWrapper::SetPassword(const std::string& str_password)
{
   m_ptr_wallet_api->set_password(str_password);
}

void WalletWrapper::Unlock(const std::string& str_password)
{
   m_ptr_wallet_api->unlock(str_password);
}

void WalletWrapper::LoadAssetInfo(string& str_symbol, uint8_t& precision, const graphene::chain::asset_id_type id)
{
   //TODO..
}

void WalletWrapper::SaveWalletFile()
{
   fc::path wallet_file(decent_path_finder::instance().get_decent_home() / "wallet.json");
   std::string str_file = wallet_file.to_native_ansi_path();

   m_ptr_wallet_api->save_wallet_file(str_file);
}


