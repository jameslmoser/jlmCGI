#include "CGI.h"
	
namespace jlm {

	std::ostream & operator << (std::ostream & outs, const jlm::CGI & cgi)
	{
		std::multimap<std::string, std::string>::const_iterator iter;		 
		outs << "<b>POST Values</b><br/>\n";
		for(iter = cgi.Post.begin(); iter != cgi.Post.end(); iter++)
			outs << iter->first << " = " << iter->second << "<br/>";
		
		outs << "&nbsp;<br/>\n";
		
		outs << "<b>GET Values</b><br/>\n";
		for(iter = cgi.Get.begin(); iter != cgi.Get.end(); iter++)
			outs << iter->first << " = " << iter->second << "<br>";				

		outs << "&nbsp;<br/>\n";
		
		outs << "<b>Cookie Values</b><br/>" << std::endl;
		for(iter = cgi.Cookie.begin(); iter != cgi.Cookie.end(); iter++)
			outs << iter->first << " = " << iter->second << "<br/>";
		
		outs << "&nbsp;<br/>\n";
		
		outs << "<b>Environment Variables</b><br/>\n"
			<< "Method: " << (cgi.Method ? cgi.Method : "<font color='red'>NULL</font>") << "<br/>\n"
			<< "QueryString: " << (cgi.QueryString ? cgi.QueryString : "<font color='red'>NULL</font>") << "<br/>\n"
			<< "ContentLength: " << (cgi.ContentLength ? cgi.ContentLength : "<font color='red'>NULL</font>") << "<br/>\n"
			<< "ContentType: " << (cgi.ContentType ? cgi.ContentType : "<font color='red'>NULL</font>") << "<br/>\n"
			<< "HttpCookie: " << (cgi.HttpCookie ? cgi.HttpCookie : "<font color='red'>NULL</font>") << "<br/>\n";
				
		outs << "&nbsp;<br>\n";

		outs << "<b>POST stdin</b>\n<pre>";
		if(cgi.PostBuffer)
			outs.write(cgi.PostBuffer, atoi(cgi.ContentLength));
		else
			outs << "<font color='red'>NULL</font>";
		outs << "</pre>\n";
		
		return outs;
	}

	CGI::CGI() :
		Method(getenv("REQUEST_METHOD")),
		QueryString(getenv("QUERY_STRING")),
		ContentType(getenv("CONTENT_TYPE")),
		ContentLength(getenv("CONTENT_LENGTH")),
		HttpCookie(getenv("HTTP_COOKIE"))
	{
		if(QueryString)
			initializeMap(Get, QueryString);
		if(ContentType && ContentLength)
			initializePost();
		if(HttpCookie)
			initializeMap(Cookie, HttpCookie);
	}

	CGI::~CGI()
	{
		if(PostBuffer)
			delete [] PostBuffer;
	}
	
	void CGI::initializePost()
	{
		int content_length = atoi(ContentLength);
		PostBuffer = new char[content_length * sizeof(char)];
		fread(PostBuffer, sizeof(char), content_length, stdin);
		*(PostBuffer + content_length) = '\0';
		
		if(strcmp(ContentType, "application/x-www-form-urlencoded") == 0)
			/* Simple key/value pairs urlencoded */
			initializeMap(Post, PostBuffer); 
		else if(strncmp(ContentType, "multipart/form-data", 19) == 0)
		{	/* multipart post data for file uploads */
			const char * boundary = strstr(ContentType, "boundary=") + 9;
			int boundary_length = strlen(boundary), clmbl = content_length-boundary_length;
			char *pStart, *pEnd, *hStart, *hEnd, *bStart, *bEnd, *name, *filename;
			
			/* Loop through POST data and find individual parts between boundary */
			pStart = strstr(PostBuffer, boundary);
			pEnd = pStart + boundary_length;
			while(memcmp(pEnd, boundary, boundary_length) != 0)
				if((pEnd = (char *)memchr(pEnd+1, boundary[0], clmbl - int(pEnd - PostBuffer))) == NULL)
					break;
		
			while(pEnd != '\0'){
				/* find Start/End of header */
				hStart = (char *)(pStart + boundary_length + 2); // \r\n
				hEnd = strstr(hStart, "\r\n\r\n");

				/* find Start/End of body */
				bStart = hEnd + 4; // \r\n\r\n
				bEnd = pEnd - 4; //  \r\n--
				
				/* terminate header and body */
				*hEnd = '\0';
                *bEnd = '\0';
				
				/* fine part name , then skip first quote and terminate on last */
				name = strstr(hStart, "form-data; name=") + 16;
				filename=strstr(hStart, "; filename=");
				memset(strchr(name + 1, name++[0]), '\0' , 1) ;
				
				/* is this a file? */
				if(filename){
					filename += 11;
					/* skip first quote and terminate on last */
					memset(strchr(filename + 1, filename++[0]), '\0', 1) ;
					/* add filename as value for Post map */
					Post.insert(StringPair(name, filename));
					/* location of file data and length in the PostBuffer */
					FileData file= {hStart, (hEnd - hStart), bStart, int(bEnd - bStart)};
					/* add file info to file map so it can be retrieved */
					File.insert(std::pair<std::string,FileData>(filename, file));
					/* restore POST data */
					memset(filename + strlen(filename), (filename-1)[0], 1);
                } else {
					Post.insert( StringPair(name,bStart));
                }

				/* restore POST data */
				memset(name + strlen(name), (name - 1)[0], 1);
				*hEnd = '\r';
                *bEnd = '\r';
				
				/* set pointers to next part */
				pStart = pEnd;
				pEnd = pStart + boundary_length;
				
				/* check if last part */
				if(strncmp(pEnd, "--", 2) ==0)
					break;

				while(memcmp(pEnd, boundary, boundary_length) != 0)
					if((pEnd = (char*)memchr(pEnd + 1, boundary[0], clmbl - int(pEnd - PostBuffer))) == NULL )
						break;
			}
		}
	}
	
	void CGI::initializeMap(std::multimap<std::string, std::string> & Map, const char * EnvData)
	{
		std::string tmpkey, tmpvalue;
		std::string *tmpstr = &tmpkey;
		register char* raw_data = (char *)EnvData;
		while (*raw_data != '\0') {
			if (*raw_data == '&') {
                if (tmpkey != "") {
					Map.insert(StringPair(UriDecode(tmpkey), UriDecode(tmpvalue)));
                }
                tmpkey.clear();
				tmpvalue.clear();
				tmpstr = &tmpkey;
			} else if (*raw_data == '=') {
				tmpstr = &tmpvalue;
			} else {
				(*tmpstr) += (*raw_data);
			}
			raw_data++;
		}
		//enter the last pair to the map
		if (tmpkey != "")
			Map.insert(StringPair(UriDecode(tmpkey), UriDecode(tmpvalue)));
	}
	
	std::string CGI::UriDecode(const std::string & sSrc) const
	{
		// Note from RFC1630:  "Sequences which start with a percent sign
		// but are not followed by two hexadecimal characters (0-9, A-F) are reserved
		// for future extension"
		
		const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
		const int SRC_LEN = sSrc.length();
		const unsigned char * const SRC_END = pSrc + SRC_LEN;
		const unsigned char * const SRC_LAST_DEC = SRC_END - 2;   // last decodable '%' 

		char * const pStart = new char[SRC_LEN];
		char * pEnd = pStart;

		while (pSrc < SRC_LAST_DEC)
		{
			if (*pSrc == '%')
			{
				char dec1, dec2;
				if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)]) && -1 != (dec2 = HEX2DEC[*(pSrc + 2)]))
				{
					*pEnd++ = (dec1 << 4) + dec2;
					pSrc += 3;
					continue;
				}
			}
			if(*pSrc == '+')
				*pEnd++ = ' ';
			else
				*pEnd++ = *pSrc;
			*pSrc++;
		}

		// the last 2- chars
		while (pSrc < SRC_END)
			*pEnd++ = *pSrc++;

		std::string sResult(pStart, pEnd);
		delete [] pStart;
		return sResult;
	}

	std::string CGI::UriEncode(const std::string & sSrc) const
	{
		const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
		const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
		const int SRC_LEN = sSrc.length();
		unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
		unsigned char * pEnd = pStart;
		const unsigned char * const SRC_END = pSrc + SRC_LEN;

		for (; pSrc < SRC_END; ++pSrc)
		{
			if (SAFE[*pSrc]) 
				*pEnd++ = *pSrc;
			else
			{
				// escape this char
				*pEnd++ = '%';
				*pEnd++ = DEC2HEX[*pSrc >> 4];
				*pEnd++ = DEC2HEX[*pSrc & 0x0F];
			}
		}

		std::string sResult((char *)pStart, (char *)pEnd);
		delete [] pStart;
		return sResult;
	}
}
