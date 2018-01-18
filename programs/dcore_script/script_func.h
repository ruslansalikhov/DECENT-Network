
#ifndef DECENT_SCRIPT_FUNC_H
#define DECENT_SCRIPT_FUNC_H

#include <vector>
#include <string>
#include <map>

class script_int_func
{
public:
   script_int_func();

   bool check_function(const std::string& fn_name);

   void execute(const std::string& fn_name, const std::vector<std::string>& args, std::string& result);

private:
   std::map<std::string, std::function<std::string(const std::vector<std::string>& )> > _methods;
   std::vector<std::string> _func_names;

};



#endif //DECENT_SCRIPT_FUNC_H
