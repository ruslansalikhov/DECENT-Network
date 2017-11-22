#include <memory>

#include <graphene/wallet/wallet.hpp>

namespace private_detail {
    class WalletAPIConnection;
};

class WalletWrapper
{
public:
   WalletWrapper();
   ~WalletWrapper();

   bool Connect();

   bool Connected();
   bool IsNew();
   bool IsLocked();

   std::chrono::system_clock::time_point HeadBlockTime();
   void SetPassword(const std::string& str_password);
   void Unlock(const std::string& str_password);

   void LoadAssetInfo(string& str_symbol, uint8_t& precision, const graphene::chain::asset_id_type id = graphene::chain::asset_id_type() );
   void SaveWalletFile();

private:
   std::shared_ptr<graphene::wallet::wallet_api> m_ptr_wallet_api;
   std::shared_ptr<private_detail::WalletAPIConnection>  m_ptr_fc_api_connection;

};
