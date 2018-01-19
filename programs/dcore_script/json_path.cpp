//
// Created by Milan Franc on 10/01/2018.
//

#include "json_path.h"

json_path::json_path(const char* text)
{
   //parse text to Json
   m_value = nlohmann::json::parse(text);
}

json_path::json_path(const nlohmann::json& object)
{
   m_value = object;
}

nlohmann::json* json_path::find(const std::string& search)
{
   m_line_stream.clear();
   m_line_stream.str(search);

   std::vector<TokenPair> tokens;

   int ret;
   ETokenType token_type;
   std::string token;
   while(!m_line_stream.eof()) {

      token_type = getToken(token);
      if (token_type == eTokenEOL)
         break;

      tokens.push_back(TokenPair(token_type, token));

      if (token_type == eTokenSymbol && token == "[") {
         ret = read_bracket_token(token);
         if (ret == -1)
            throw std::runtime_error("parse error.");

         tokens.push_back(TokenPair(eTokenText, token));
      }

   }

   if (tokens.empty()) {
      throw std::runtime_error("parse error");
   }

   //Execute...

   std::vector<TokenPair>::iterator it = tokens.begin();
   nlohmann::json* current_object = nullptr;
   nlohmann::json result;

   if (it->type == eTokenSymbol && it->token == "$") {
      current_object = &m_value;  //root object
      ++it;
   }

   do
   {

      if (it->type == eTokenSymbol && it->token == ".") {
         ++it;

         if (it->type != eTokenText) {
            throw std::runtime_error("invalid token");
         }

         auto find = current_object->find(it->token);
         if (find == current_object->end())
            throw std::runtime_error("item not found!");

         ++it;
         current_object = & (*find);
      }
      else if (it->type == eTokenSymbol && it->token == "[") {

         if (current_object->type() != nlohmann::json::value_t::array)
            throw std::runtime_error("not an array!");

         ++it;

         if (it->type == eTokenText && it->token == "*") {
            //all items...
            // not supported yet..
            throw std::runtime_error("all items from array is not supported yet.");

            ++it;
            if (it->type == eTokenSymbol && it->token == "]") {
               ++it;
            }
         }
         else if (it->type == eTokenText && is_number(it->token))  {

            unsigned int iNumber = std::stoi(it->token);

            current_object = & (current_object->at(iNumber));

            ++it;
            if (it->type == eTokenSymbol && it->token == "]") {
               ++it;
            }
         }
         else {
            throw std::runtime_error("unknown token or parse error.");

         }
      }
      else {
         // ???
         throw std::runtime_error("unknown token");

      }

   } while(it != tokens.end());

   return current_object;
}

int json_path::read_text_token(std::string& result)
{
   result.clear();

   int chr;
   while(!m_line_stream.eof()) {
      chr = m_line_stream.peek();
      if (chr == -1) {
         return -1;
      }

      if (!isalnum(chr)) {
         break;
      }

      result += m_line_stream.get();
   }

   return 0;
}

int json_path::read_bracket_token(std::string& result)
{
   result.clear();

   int chr;
   while(!m_line_stream.eof()) {
      chr = m_line_stream.peek();
      if (chr == -1) {
         return -1;
      }

      if (chr == ']') {
         break;
      }

      result += m_line_stream.get();
   }

   return 0;
}

json_path::ETokenType json_path::getToken(std::string& token)
{
   ETokenType tok_type = eTokenUnknown;

   token.clear();
   int chr, ret_val, stack_count = 1;
   while(!m_line_stream.eof()) {
      chr = m_line_stream.peek();
      if (chr == -1) {
         return eTokenEOL;
      }

      if (chr == '$' || chr == '@' || chr == '.' || chr == '[' || chr == ']' || chr == '*') {
         tok_type = eTokenSymbol;
         token += m_line_stream.get();
         break;
      }
      else if (isalnum(chr)) {
         tok_type = eTokenText;
         read_text_token(token);
         break;
      }
   }

   return tok_type;
}

bool json_path::is_number(const std::string& val)
{
   for(const char& chr : val) {
      if (std::isdigit(chr) == 0)
         return false;
   }

   return true;
}