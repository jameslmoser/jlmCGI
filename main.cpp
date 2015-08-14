#include <iostream>
#include <stdexcept>

#include "CGI.h"

int main (int argc, char * const argv[]) {

	try {
		jlm::CGI cgi;
		
		printf("%s%c%c\n","Content-Type: text/html;charset=iso-8859-1",13,10);
		std::cout << cgi;			

	} catch(std::exception & e){
		printf("%s%c%c\n","Content-Type: text/html;charset=iso-8859-1",13,10);
		
		std::cout << "An exception has occurred: " << e.what();
	} catch(std::string str){
		printf("%s%c%c\n","Content-Type: text/html;charset=iso-8859-1",13,10);
		
		std::cout << "An error occured: " << str;
	} catch (...){
		printf("%s%c%c\n","Content-Type: text/html;charset=iso-8859-1",13,10);
		
		std::cout << "An unknown error occured!";
	}
	
	return 0;
}
