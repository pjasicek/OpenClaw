#ifndef __XMLMACROS_H__
#define __XMLMACROS_H__

#include <Tinyxml/tinyxml.h>

#ifndef XML_ADD_TEXT_ELEMENT
#define XML_ADD_TEXT_ELEMENT(elementName, elementValue, parentElement) \
{ \
    TiXmlElement* element = new TiXmlElement(elementName); \
    element->LinkEndChild(new TiXmlText(elementValue)); \
    parentElement->LinkEndChild(element); \
} \

#endif

#ifndef XML_CREATE_NEW_NODE
#define XML_CREATE_NEW_NODE(elementName, parentElement) \
{ \
    TiXmlElement* element = new TiXmlElement(elementName); \
    parentElement->LinkEndChild(element); \
} \

#endif

#ifndef XML_ADD_1_PARAM_ELEMENT
#define XML_ADD_1_PARAM_ELEMENT(elementName, param1Name, param1Value, parentElement) \
{ \
    TiXmlElement* element = new TiXmlElement(elementName); \
    element->SetAttribute(param1Name, param1Value); \
    parentElement->LinkEndChild(element); \
} \

#endif

#ifndef XML_ADD_2_PARAM_ELEMENT
#define XML_ADD_2_PARAM_ELEMENT(elementName, param1Name, param1Value, param2Name, param2Value, parentElement) \
{ \
    TiXmlElement* element = new TiXmlElement(elementName); \
    element->SetAttribute(param1Name, param1Value); \
    element->SetAttribute(param2Name, param2Value); \
    parentElement->LinkEndChild(element); \
} \

#endif

#endif