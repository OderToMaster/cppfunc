#include<funcext.hpp>
#include<iostream>
#include<string>
#include<cctype>
#include<vector>
#include<iterator>
int main(int argc,char*argv[]){
	std::string str = "hello,world!";
	auto upper = stle::to_stream(str).map(std::toupper).collect<std::string>();
	auto vec = stle::to_stream(str).map(std::toupper).collect<std::vector<char>>();
	std::cout << upper << std::endl;
	std::cout.write(vec.data(), vec.size()) << std::endl;
	return 0;
}