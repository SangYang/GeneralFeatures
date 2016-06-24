#include <stdio.h>
#include "memorycheck.h"
#include "tinyxml.h"


#if defined(TIXML_USE_STL) && defined(_DEBUG)
#pragma comment(lib, "tinyxmld_stl.lib")
#elif defined(TIXML_USE_STL)
#pragma comment(lib, "tinyxml_stl.lib")
#elif defined(_DEBUG)
#pragma comment(lib, "tinyxmld.lib")
#else
#pragma comment(lib, "tinyxml.lib")
#endif


bool CreateXml(const char *file_path) {
	TiXmlDocument *p_doc = new TiXmlDocument();
	TiXmlDeclaration *p_declar = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	TiXmlElement *p_root = new TiXmlElement("deviceline");
	TiXmlComment *p_comment = new TiXmlComment("comment for root");
	TiXmlElement *p_first = new TiXmlElement("first");
	TiXmlElement *p_second = new TiXmlElement("second");
	TiXmlElement *p_one = new TiXmlElement("one");
	TiXmlElement *p_two = new TiXmlElement("two");
	TiXmlElement *p_three = new TiXmlElement("three");
	TiXmlText *p_one_text = new TiXmlText("test1");
	TiXmlText *p_two_text = new TiXmlText("test2");
	TiXmlText *p_three_text = new TiXmlText("1");
	p_doc->LinkEndChild(p_declar);
	p_doc->LinkEndChild(p_root);
	p_root->LinkEndChild(p_comment); 
	p_root->LinkEndChild(p_first);
	p_root->LinkEndChild(p_second);
	p_first->SetAttribute("ID", "001");
	p_first->LinkEndChild(p_one);
	p_first->LinkEndChild(p_two);
	p_second->SetAttribute("ID", "002");
	p_second->LinkEndChild(p_three);
	p_one->LinkEndChild(p_one_text);
	p_two->LinkEndChild(p_two_text);
	p_three->SetAttribute("key", "123");
	p_three->SetAttribute("value", "456");
	p_three->LinkEndChild(p_three_text);
	p_doc->SaveFile(file_path);
	delete p_doc;
	return true;
}

bool ReadXml(const char *file_path) {
	TiXmlDocument doc(file_path);
	doc.LoadFile();
	TiXmlHandle doc_handle(&doc);  
	TiXmlElement *p_root2 = doc_handle.FirstChildElement().Element();  
	TiXmlHandle hRoot = TiXmlHandle(p_root2);
	TiXmlElement *pElem=hRoot.FirstChild( "Messages" ).FirstChild().Element();

	TiXmlElement *p_root = doc.RootElement();
	TiXmlElement *p_first = p_root->FirstChildElement();
	TiXmlElement *p_one = p_first->FirstChildElement();
	TiXmlElement *p_two = p_one->NextSiblingElement();
	TiXmlElement *p_three = p_two->NextSiblingElement();
	TiXmlAttribute *p_first_id = p_first->FirstAttribute();

	printf("root:     %s\n", p_root->Value());
	printf("first_id: %s\n", p_first_id->Value());
	printf("one:      %s\n", p_one->FirstChild()->Value());
	printf("two:      %s\n", p_two->FirstChild()->Value());
	const char *aaa = p_first->Attribute("key");

	if (!p_root2)
		printf("error\n");
	else
		printf("three:    %s\n", p_root2->Value());
	printf("pElem:%s\n", pElem->Value());
	return true;
}



#if 1
int main() {
	MEMCHECK_BEGIN();	

	char file_path[256] = "info.xml";
	CreateXml(file_path);
	//ReadXml(file_path);
	
	MEMCHECK_END();
	return 0;
}
#endif