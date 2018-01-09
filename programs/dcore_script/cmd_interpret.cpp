
#include "cmd_interpret.h"

#include <map>


bool script_cli::check_function(const std::string& fn_name)
{
   if (std::find(_func_names.begin(), _func_names.end(), fn_name) != _func_names.end() )
      return true;

   return false;
}

///////////////////////////////////////////////////////////

static std::string scr_fn_print(const std::vector<std::string>& args)
{
   cout << args.at(0) << endl;

   return std::string();
}

///////////////////////////////////////////////////////////

DcScriptEngine::DcScriptEngine()
{

}

void DcScriptEngine::open(const std::string& filename)
{
   m_cmd_file.open(filename.c_str(), std::ios::in);


}

void DcScriptEngine::set_wallet_api(std::shared_ptr<script_cli> script_cli)
{
   m_script_cli = script_cli;
}

int DcScriptEngine::ignore_whitespace()
{
   int chr;
   while(!m_line_stream.eof()) {
      chr = m_line_stream.peek();
      if (chr != ' ')
         break;

      m_line_stream.get();
   }

   return 0;
}

int DcScriptEngine::read_alphanum(std::string& token)
{
   token.clear();

   int chr;
   while(!m_line_stream.eof()) {
      chr = m_line_stream.peek();
      if(!isalnum(chr) && chr != '_')
         break;

      token += m_line_stream.get();
   }
   return 0;
}

int DcScriptEngine::parse_token(std::string& token, ETokenType& token_type)
{
   token.clear();
   token_type = eTokenUnk;

   int chr = -1;
   while(!m_line_stream.eof()) {
      chr = m_line_stream.peek();
      if (chr == -1) {
         return -1;
      }

      if (chr == '(' || chr == ')' || chr == ',' || chr == '$' || chr == '=' || chr == '\"') {
         token_type = eTokenSymbol;
         token += m_line_stream.get();
         break;
      }
      else if (chr == ' ') {    //ignore whitespace..
         ignore_whitespace();
      }
      else if (chr == '\\') {
         token += m_line_stream.get();
         token += m_line_stream.get();

         token_type = eTokenEscapeText;
         break;
      }
      else if (isalnum(chr) || chr == '_') {
         token_type = eTokenAlphaNum;
         read_alphanum(token);
         break;
      }
      else {
         chr = m_line_stream.get();

      }
   }

   return chr;
}

int DcScriptEngine::read_text_token(std::string& token)
{
   token.clear();

   int chr, ret_val;
   while(!m_line_stream.eof()) {
      chr = m_line_stream.peek();
      if (chr == -1) {
         return -1;
      }

      if (chr == '\"') {
         m_line_stream.get();
         break;
      }
      else if (chr == '\\') {      //escape sequence
         m_line_stream.get();
         token += m_line_stream.get();
      }
      else {
         token += m_line_stream.get();

      }
   }

   return 0;
}

int DcScriptEngine::parse_token_to_bracket(std::vector<TokenPair>& out_tokens)
{
   int chr, ret_val, stack_count = 1;
   while(!m_line_stream.eof()) {
      chr = m_line_stream.peek();
      if (chr == -1) {
         return -1;
      }

      if (chr == '(') {
         stack_count++;

         out_tokens.push_back(TokenPair(eTokenSymbol, "(" ));
         m_line_stream.get();
      }
      else if (chr == ')') {
         stack_count--;
         if (stack_count == 0)
            break;

         out_tokens.push_back(TokenPair(eTokenSymbol, ")" ));
         m_line_stream.get();
      }
      else {
         ETokenType type;
         std::string text;

         chr = parse_token(text, type);
         if (chr < 0)
            break;

         if (type == eTokenSymbol && text == "\"") {
            read_text_token(text);
            type = eTokenText;
         }

         out_tokens.push_back(TokenPair(type, text));
      }


//      else if (chr == '$') {
//         out_tokens.push_back(TokenPair(eTokenSymbol, "$" ));
//
//         std::string variable;
//         ret_val = read_alphanum(variable);
//         if (ret_val < 0 || variable.empty())
//            return -1;
//
//         out_tokens.push_back(TokenPair(eTokenAlphaNum, variable));
//      }

   }

   return chr;
}



int DcScriptEngine::parse_line(const std::string& line, std::vector<TokenPair>& result)
{
   if (line.at(0) == '#') {   //ignore this line
      result.clear();
      return 0;
   }

   m_line_stream.clear();
   m_line_stream.str(line);

   int ret;
   std::string token;
   ETokenType token_type;

   result.clear();

   for(;;) {
      ret = parse_token(token, token_type);
      if (ret < 0)
         break;

      if (token_type == eTokenSymbol && token == "(") {
         result.push_back(TokenPair(token_type, token) );

         ret = parse_token_to_bracket(result);
         if (ret < 0)
            break;

      }
      else {
         result.push_back(TokenPair(token_type, token) );
      }
   }

   return 0;
}

int DcScriptEngine::convert_variables(std::vector<TokenPair>& inout_tokens, const std::map<std::string, std::string>& variables)
{
   auto found = std::find_if(inout_tokens.begin(), inout_tokens.end(),
                             [&](const TokenPair& val) { return val.type == eTokenSymbol && val.token == "("; }  );

   if (found == inout_tokens.end())
      return -1;

   auto it = ++found;
   do
   {
      if (it->type == eTokenSymbol && it->token == "$") {
         std::vector<TokenPair>::iterator erase_pos = it;
         ++it;
         if (it->type == eTokenAlphaNum) {

            if (variables.find(it->token) != variables.end()) {
               std::string value = variables.at(it->token);

               it = inout_tokens.erase(erase_pos);

               it->token = value;
               it->type  = eTokenAlphaNum;
            }
            else {
               //TODO: error...  variable not found


            }
         }
      }
      else {
         ++it;
      }

   } while (!(it->type == eTokenSymbol && it->token == ")"));

   return 0;
}

int DcScriptEngine::parse_line_tokens(std::vector<TokenPair>& tokens, std::string& fn_name, std::string& result_name, std::vector<std::string>& params)
{
   fn_name.clear();
   result_name.clear();
   params.clear();

   std::vector<TokenPair>::const_iterator it = tokens.begin();

   if (it->type == eTokenSymbol && it->token == "$") {
      ++it;
      if (it->type == eTokenAlphaNum) {
         result_name = it->token;
         ++it;
         if (it->type == eTokenSymbol && it->token == "=") {
            ++it;
         }
         else {
            //error
         }
      }
      else {
         //error
      }
   }


   std::string tmp_fn_name;
   if (it->type == eTokenAlphaNum) {
      tmp_fn_name = it->token;
      ++it;
      if (it->type == eTokenSymbol && it->token == "(") {
         ++it;

         //while params...
         do
         {
            if (it->type == eTokenAlphaNum || it->type == eTokenText) {
               params.push_back(it->token);
            }

            ++it;
         } while(!(it->type == eTokenSymbol && it->token == ")"));

         ++it;
         fn_name = tmp_fn_name;

      }
      else {
         //error..
      }
   }

   return 0;
}


std::string test_func(const std::vector<std::string>& params) {

   return std::string();
}

int DcScriptEngine::execute_line(const std::string& fn_name, const std::vector<std::string>& params, std::string& result)
{
   if (m_script_cli->check_function(fn_name)) {
      m_script_cli->execute(fn_name, params, result);
   }
   else {
      //TODO: some other functions...

      if (fn_name == "printf") {
         result = scr_fn_print(params);
      }
   }


   //serach for function

#if 0
   //call function with params..
   result = test_func(params);

   const char* test_string = "{\n"
         "    'user': {\n"
         "        'name': 'abc',\n"
         "        'fx': {\n"
         "            'message': {\n"
         "                'color': 'red'\n"
         "            },\n"
         "            'user': {\n"
         "                'color': 'blue'\n"
         "            }\n"
         "        }\n"
         "    },\n"
         "    'timestamp': '2013-10-04T08: 10: 41+0100',\n"
         "    'message': 'I'mABC..',\n"
         "    'nanotime': '19993363098581330'\n"
         "}    ";

   result = std::string(test_string);
#endif

   return 0;
}

int DcScriptEngine::interpret()
{
   if (!m_cmd_file.is_open()) {
      return -1;
   }

   std::map<std::string, std::string> variables;

   std::vector<TokenPair> tokens;

   int ret;
   std::string line;
   while(!m_cmd_file.eof()) {
      std::getline(m_cmd_file, line);

      if (line.empty())
         continue;

      ret = parse_line(line, tokens);
      if (tokens.empty())
         continue;

      ret = convert_variables(tokens, variables);

      std::string fn_name, result_name;
      std::vector<std::string> params;

      //parse tokens into function with params and result
      ret = parse_line_tokens(tokens, fn_name, result_name, params);
      if (ret < 0)
         continue;

      std::string fn_result;
      //execute function..
      ret = execute_line(fn_name, params, fn_result);

      //put result into variables
      if(!result_name.empty()) {
         variables.insert(std::make_pair(result_name, fn_result));
      }

   }

   return 0;
}

std::string DcScriptEngine::encode_param(const std::string& param)
{
   return std::string();
}

std::string DcScriptEngine::decode_param(const std::string& param)
{
   return std::string();
}

///////////////////////////////////////////////////////////////////


int interpret_commands(const std::string& filename, std::shared_ptr<script_cli> client)
{

   return 0;
}



