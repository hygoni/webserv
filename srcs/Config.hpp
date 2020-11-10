/* Copyright 2020 hyeyoo, hyekim */

#ifndef SRCS_CONFIG_HPP_ 
#define SRCS_CONFIG_HPP_ 

#include <string>
#include <map>

struct Config {
		std::string name;
		std::map<std::string, std::string> attr;
		std::vector<std::string, Config> child;
		virtual ~Config;
}


#endif  // SRCS_CONFIG_HPP_ 
