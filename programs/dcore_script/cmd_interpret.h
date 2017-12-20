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

class DcScriptEngine
{
public:
   DcScriptEngine();

   void open(const std::string& filename);

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

};

int interpret_commands(const std::string& filename);


#endif //DECENT_CMD_INTERPRET_H
