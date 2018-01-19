//
// Created by Milan Franc on 10/01/2018.
//

#ifndef DECENT_JSON_PATH_H
#define DECENT_JSON_PATH_H

#include <string>
#include <sstream>
#include <json.hpp>


class json_path
{
public:
   json_path(const char* text);
   json_path(const nlohmann::json& object);

   nlohmann::json* find(const std::string& search);

   nlohmann::json& ref() { return m_value; }
   const nlohmann::json& ref() const { return m_value; }

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

private:
   nlohmann::json m_value;

   std::stringstream m_line_stream;
};


#endif //DECENT_JSON_PATH_H
