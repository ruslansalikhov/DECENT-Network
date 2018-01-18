//
// Created by Milan Franc on 06/12/2017.
//

#ifndef DECENT_CMD_INTERPRET_H
#define DECENT_CMD_INTERPRET_H

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <memory>  //for std::shared_ptr


#include <graphene/app/api.hpp>
#include <graphene/chain/protocol/protocol.hpp>
#include <graphene/egenesis/egenesis.hpp>
#include <graphene/utilities/key_conversion.hpp>
#include <graphene/wallet/wallet.hpp>
#include <decent/package/package.hpp>
#include <graphene/utilities/dirhelper.hpp>

#include <fc/io/json.hpp>
#include <fc/io/stdio.hpp>
#include <fc/network/http/server.hpp>
#include <fc/network/http/websocket.hpp>
#include <fc/rpc/cli.hpp>
#include <fc/rpc/http_api.hpp>
#include <fc/rpc/websocket_api.hpp>
#include <fc/smart_ref_impl.hpp>
#include <fc/log/console_appender.hpp>
#include <fc/log/file_appender.hpp>
#include <fc/log/logger.hpp>
#include <fc/log/logger_config.hpp>
#include <fc/interprocess/signals.hpp>


namespace decent {
    class wallet_api;
};

class script_cli;
class script_int_func;

class DcScriptEngine
{
public:
   DcScriptEngine();

   void open(const std::string& filename);
   void set_wallet_api(std::shared_ptr<script_cli> script_cli);
   void set_internal_funcs(std::shared_ptr<script_int_func> inter_funcs);

   int interpret();

   static std::string encode_param(const std::string& param);
   static std::string decode_param(const std::string& param);



private:
   enum ETokenType {
      eTokenUnk = 0,
      eTokenAlphaNum,  //A-z,0-9 and underline
      eTokenSymbol,    //brackets, comma, dolar, equal
      eTokenWhitespace,
      eTokenOther,
      eTokenText,
      eTokenEscapeText,

   };

   struct TokenPair {
      ETokenType type;
      std::string token;

      TokenPair() : type(eTokenUnk) {}
      TokenPair(ETokenType _type, const std::string& _token) : type(_type), token(_token) {}
   };

   int parse_token(std::string& token, ETokenType& token_type);
   int read_text_token(std::string& token);
   int parse_token_to_bracket(std::vector<TokenPair>& out_tokens);
   int parse_line(const std::string& line, std::vector<TokenPair>& result);

   int convert_variables(std::vector<TokenPair>& inout_tokens, const std::map<std::string, std::string>& variables);

   int parse_line_tokens(std::vector<TokenPair>& tokens, std::string& fn_name, std::string& result_name, std::vector<std::string>& params);

   int execute_line(const std::string& fn_name, const std::vector<std::string>& params, std::string& result);


   int ignore_whitespace();
   int read_alphanum(std::string& token);

private:
   std::fstream m_cmd_file;

   std::stringstream m_line_stream;

   std::shared_ptr<script_cli> m_script_cli;
   std::shared_ptr<script_int_func> m_int_funcs;
};

///////////////////////////////////////////////////////////////////////////////////////////

/**
 *  Provides a simple wrapper for RPC calls to a given interface.
 */
class script_cli : public fc::api_connection {
public:

   virtual variant send_call(fc::api_id_type api_id, string method_name, fc::variants args = fc::variants()) {
      FC_ASSERT(false);
   }

   virtual variant send_callback(uint64_t callback_id, fc::variants args = fc::variants()) {
      FC_ASSERT(false);
   }

   virtual void send_notice(uint64_t callback_id, fc::variants args = fc::variants()) {
      FC_ASSERT(false);
   }

   void Initialize()
   {
      _func_names = get_method_names(0);
   }

   void execute(const std::string& func_name, const std::vector<std::string>& args, std::string& result)
   {
      try
      {
         fc::variants fc_args;
         for(const std::string& val : args) {
            fc_args.push_back(val.c_str());
         }

         auto fc_result = receive_call( 0, func_name, fc_args );
         auto itr = _result_formatters.find( func_name );
         if( itr == _result_formatters.end() )
         {
            if (fc_result.is_array()) {
               result = fc::json::to_pretty_string(fc_result);
            }
            else {
               result = fc_result.as_string();
            }
         }
         else {
            result = itr->second( fc_result, fc_args );

            //std::cout << itr->second( result, args ) << "\n";
         }

      }
      catch ( const fc::exception& e )
      {
         std::cout << e.to_detail_string() << "\n";
      }
      catch ( const std::exception& ex)
      {
         std::cout << ex.what() << "\n";
      }
   }

   void format_result(const std::string& method, std::function<string(variant, const fc::variants &)> formatter) {
      _result_formatters[method] = formatter;
   }

   bool check_function(const std::string& fn_name);

private:
   std::map<std::string, std::function<string(variant, const fc::variants &)> > _result_formatters;
   std::vector<std::string> _func_names;

};




int interpret_commands(const std::string& filename, std::shared_ptr<script_cli> client);


#endif //DECENT_CMD_INTERPRET_H
