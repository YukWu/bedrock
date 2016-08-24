#ifndef _BEDROCK_CONFIG_H
#define _BEDROCK_CONFIG_H

#include <string>
#include <unordered_map>
#include <fstream>

namespace bedrock{
	/*读配置文件类
	  配置文件格式为
	  key1:value1
	  key2:value2
	  ...
	  ...
	 */
	class Config{
		public:
			explicit Config(const std::string& filename);
			~Config();
			std::string getValueByKey(const std::string& key) const;
			void reload_config();
		private:
			void trim(std::string& s);
			void init();

			std::string  name_;
			std::fstream file_;
			std::unordered_map<std::string,std::string> m_;
	};
}
#endif
