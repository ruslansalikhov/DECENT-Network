//
// Created by Milan Franc on 10/01/2018.
//

#ifndef DECENT_JSON_PATH_H
#define DECENT_JSON_PATH_H

#include <string>
#include <sstream>
#include <json.hpp>

//nlohmann::json


class json_path
{
public:
   json_path(const std::string& text);

   nlohmann::json find(const std::string& search);

private:
   enum ETokenType {
      eTokenUnknown = 0,
      eTokenEOL,
      eTokenSymbol,
      eTokenText
   };

   struct TokenPair {
      ETokenType type;
      std::string token;

      TokenPair() : type(eTokenUnknown) {}
      TokenPair(ETokenType _type, const std::string& _token) : type(_type), token(_token) {}
   };

private:
   ETokenType getToken(std::string& token);
   int read_text_token(std::string& result);
   int read_bracket_token(std::string& result);
   bool is_number(const std::string& val);


   std::stringstream m_line_stream;

   nlohmann::json m_value;
};


#endif //DECENT_JSON_PATH_H
