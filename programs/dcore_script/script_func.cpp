
#include "script_func.h"
#include <iostream>

static std::string scr_fn_print(const std::vector<std::string>& args)
{
   std::cout << args.at(0) << std::endl;

   return std::string();
}

///////////////////////////////////////////////////////////

script_int_func::script_int_func()
{
   _methods["printf"] = scr_fn_print;

}

bool script_int_func::check_function(const std::string& fn_name)
{
   return _methods.find(fn_name) != _methods.end();
}

void script_int_func::execute(const std::string& fn_name, const std::vector<std::string>& args, std::string& result)
{
   auto find = _methods.find(fn_name);
   if (find == _methods.end()) {
      return;
   }

   try
   {
      result = find->second(args);


   }
   catch(const std::exception& ex) {
      std::cout << ex.what() << std::endl;
   }
}


