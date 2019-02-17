#include<funcext.hpp>
#include<iostream>
#include<string>
#include<cctype>
#include<vector>
#include<iterator>
#include<list>
#include<fstream>
#include<set>
#include<functional>
auto lambda_stoi = [](const std::string&v)->int{
	if (v.empty()) {
		std::cout << v << std::endl;
	}
	return std::stoi(v);
};
auto lambda_is_not_digit = [](char ch)->bool {
	
	return !std::isdigit(ch);
};
int main(int argc,char*argv[]){
	std::string str = "{12,34,56,78,89,67,45,34}";
	//auto upper = stle::to_stream(str).map(std::toupper).collect<std::string>();
	//auto vec = stle::to_stream(str).map(std::toupper).collect<std::vector<char>>();
	//std::cout << upper << std::endl;
	//auto mapper = stle::to_stream(str).map(std::toupper);
	std::function<void(int)> fnc;
	using list_string = std::list<std::string>;
	auto lst = stle::to_stream(str)
				   .map(std::toupper)
				   .split<std::string>(lambda_is_not_digit)
				   //.filter([](std::string &v) -> bool { return !v.empty(); })
				   .map(lambda_stoi)
				   .collect<std::list<int>>();
	auto sstr = "hello,world{12}";
	auto nums = stle::to_range(sstr, sstr + strlen(sstr))
					.split<std::string>(lambda_is_not_digit)
					//.filter([](std::string&v)->bool {return !v.empty(); })
					.map(lambda_stoi)
					.filter([](int v)->bool {return v < 50; })
					.collect<std::list<int>>();
	//auto spliter = mapper.split<std::string>(lambda_is_not_digit);
	//auto spmapper = spliter.map(lambda_stoi);
	//std::cout.write(vec.data(), vec.size())<< std::endl;
	std::ostream_iterator<int>
		out(std::cout, " ");
	std::copy(std::begin(lst),std::end(lst),out);
	std::cout<<std::endl<<std::flush;
	std::ifstream istream;
	if(!(istream.open("input.data"),istream.is_open())){
		std::cout<<"read file input.data fail!"<<std::endl;
		return 2;
	}
	std::istream_iterator<char> in=istream,inend;
	std::multiset<int> sets;
	stle::to_range(in, inend)
		.split<std::string>(lambda_is_not_digit)
		.map(lambda_stoi)
	.filter([](int v)->bool{return v>40;})
		//.collect<std::list<int>>();
		.for_each([&sets](int v){
			sets.insert(v);
			std::cout<<v<<",";
		});
	istream.close();
	return 0;
}