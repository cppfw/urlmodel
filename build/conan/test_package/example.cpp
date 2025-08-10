#include <urlmodel/url.hpp>

#include <iostream>

int main(int argc, const char** argv){
	urlmodel::url url;

	url.host = "hello.com";

	std::cout << "url = " << url.to_string() << std::endl;
}
