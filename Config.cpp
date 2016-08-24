#include <stdexcept>

#include "Config.h"

namespace bedrock{
	
void Config::trim(std::string& s){
	static const char whitespace[] = " \n\t\v\r\f";
	s.erase(0,s.find_first_not_of(whitespace));
	s.erase(s.find_last_not_of(whitespace) + 1U);
}

Config::Config(const std::string& filename):name_(filename){
	init();
}
void Config::init(){

	file_.open(name_,std::ios::in);
	if(!file_.is_open()){
		throw std::invalid_argument(name_+" may not exist");
	}
	std::string line;
	while(getline(file_,line)){
		trim(line);
		if(line.length()>0){
			auto index = line.find(':');
			if(index!=std::string::npos){
				auto key = line.substr(0,index);
				auto value = line.substr(index+1);
				trim(key);
				trim(value);
				m_.insert({key,value});
			}
		}
	}
}

Config::~Config(){
	if(file_.is_open()){
		file_.close();
	}
}

std::string Config::getValueByKey(const std::string& key) const {
	auto re = m_.find(key);
	if(re!=m_.end()){
		return re->second;
	}
	return "";
}

void Config::reload_config(){
	if(file_.is_open()){
		file_.close();
		m_.clear();
	}
	init();
}

}
/*
int main(){
	bedrock::Config config("config.conf");
	config.reload_config();
	std::cout << config.getValueByKey("key1") << std::endl;
	std::cout << config.getValueByKey("key1") << std::endl;
	std::cout << config.getValueByKey("key2") << std::endl;
	std::cout << config.getValueByKey("key4") << std::endl;

	return 0;
}
*/
