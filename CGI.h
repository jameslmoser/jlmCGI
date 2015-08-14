#ifndef CGI_H
#define	CGI_H

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

#include <sys/types.h>
#include <unistd.h>

namespace jlm {

	struct FileData
	{
		char * Headers;
		size_t HeaderLength;
		char * Data;
		size_t DataLength;
	};

	const char HEX2DEC[256] = 
	{
		/*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
		/* 0 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* 1 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* 2 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* 3 */  0, 1, 2, 3,  4, 5, 6, 7,  8, 9,-1,-1, -1,-1,-1,-1,
		
		/* 4 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* 5 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* 6 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* 7 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		
		/* 8 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* 9 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* A */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* B */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		
		/* C */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* D */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* E */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* F */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
	};

	// Only alphanum is safe.
	const char SAFE[256] =
	{
		/*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
		/* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,
		
		/* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
		/* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
		/* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
		/* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
		
		/* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		
		/* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
	};
	
	class CGI
	{
		public:
			CGI();
			~CGI();
			/* Pointers to environment variables */
			const char * Method;
			const char * QueryString;
			const char * HttpCookie;
			const char * ContentType;
			const char * ContentLength;
			
			/* key,value pairs for GET, POST, and Cookies */
			std::multimap<std::string, std::string> Get;
			std::multimap<std::string, std::string> Post;
			std::multimap<std::string, std::string> Cookie; 
			typedef std::pair<std::string, std::string> StringPair;
						
			/* the First and Last character of the File in PostBuffer */
			std::multimap<std::string, FileData> File;
			
			/* URL Encoding/Decoding functions */
			std::string UriDecode(const std::string& sSrc) const;
			std::string UriEncode(const std::string& sSrc) const;

			/* keep POST data */
			char * PostBuffer;
		private:
			/* Downloads Content, then uses initializeMap() as well */
			void initializePost();
			/* Parses data and populates maps */
			void initializeMap(std::multimap<std::string, std::string> & Map, const char * EnvData);

	};
	
	std::ostream & operator << (std::ostream & outs, const jlm::CGI & cgi);
}


#endif