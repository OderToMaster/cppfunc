#include<funcext.hpp>
#include<iostream>
#include<string>
#include<cctype>
#include<vector>
#include<iterator>
#include<list>
#include<cctype>
int main(int argc,char*argv[]){
	std::string str = "{12,34,56,78,89,67,45,34}";
	auto upper = stle::to_stream(str).map(std::toupper).collect<std::string>();
	auto vec = stle::to_stream(str).map(std::toupper).collect<std::vector<char>>();
	std::cout << upper << std::endl;
	auto mapper = stle::to_stream(str).map(std::toupper);
	using list_string = std::list<std::string>;
	auto lst = mapper
		.split<std::string>([](char ch)->bool { return !std::isdigit(ch);  })
		.map([](const std::string&v)->int{return std::stoi(v);})
		.collect<std::list<int>>();
	std::cout.write(vec.data(), vec.size()) << std::endl;
	std::ostream_iterator<int> out(std::cout," ");
	std::copy(std::begin(lst),std::end(lst),out);
	return 0;
}