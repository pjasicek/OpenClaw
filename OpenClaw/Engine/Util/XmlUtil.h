#ifndef __XML_UTIL_H__
#define __XML_UTIL_H__

#include <tinyxml.h>
#include "Util.h"
#include "StringUtil.h"

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

template<typename T>
void AddXmlTextElement(const char* elemName, T val, TiXmlElement* pParent)
{
    XML_ADD_TEXT_ELEMENT(elemName, ToStr(val).c_str(), pParent);
}

inline bool SetTiXmlElementText(const std::string& text, TiXmlElement* pElem)
{
    TiXmlNode* child = pElem->FirstChild();
    if (child)
    {
        TiXmlText* childText = child->ToText();
        if (childText)
        {
            childText->SetValue(text.c_str());
            return true;
        }
    }

    return false;
}

// pathToNode in format: elem1.elem2.elem3
TiXmlElement* GetTiXmlElementFromPath(TiXmlElement* pRootElem, const std::string& pathToNode);

template <typename T>
inline bool SetTiXmlNodeValue(TiXmlElement* pRootElem, const std::string& pathToNode, const T& val)
{
    if (TiXmlElement* pDestElem = GetTiXmlElementFromPath(pRootElem, pathToNode))
    {
        std::string valStr = ToStr(val);
        return SetTiXmlElementText(valStr, pDestElem);
    }

    return false;
}

template <typename T>
inline bool SetTiXmlNode1Attribute(TiXmlElement* pRootElem,
    const std::string& pathToNode,
    const std::string& attr1Name,
    const T& attr1Val)
{
    if (TiXmlElement* pDestElem = GetTiXmlElementFromPath(pRootElem, pathToNode))
    {
        std::string val1Str = ToStr(attr1Val);
        pDestElem->SetAttribute(attr1Name.c_str(), val1Str.c_str());

        return true;
    }

    return false;
}

template <typename T>
inline bool SetTiXmlNode2Attribute(TiXmlElement* pRootElem,
    const std::string& pathToNode,
    const std::string& attr1Name,
    const T& attr1Val,
    const std::string& attr2Name,
    const T& attr2Val)
{
    if (TiXmlElement* pDestElem = GetTiXmlElementFromPath(pRootElem, pathToNode))
    {
        std::string val1Str = ToStr(attr1Val);
        std::string val2Str = ToStr(attr2Val);
        pDestElem->SetAttribute(attr1Name.c_str(), val1Str.c_str());
        pDestElem->SetAttribute(attr2Name.c_str(), val2Str.c_str());

        return true;
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------------
// Convinience functions for getting values from XMl elements
//---------------------------------------------------------------------------------------------------------------------

inline void SetBoolIfDefined(bool *dest, TiXmlElement* elem)
{
    if (elem && elem->GetText() == NULL)
        return;

    std::string opt = elem->GetText();
    if (opt == "true")
    {
        *dest = true;
    }
    else if (opt == "false")
    {
        *dest = false;
    }
}

inline void SetUintIfDefined(unsigned* dest, TiXmlElement* elem)
{
    if (elem && elem->GetText())
    {
        *dest = std::stoi(elem->GetText());
    }
}

inline void SetIntIfDefined(int* dest, TiXmlElement* elem)
{
    if (elem && elem->GetText())
    {
        *dest = std::stoi(elem->GetText());
    }
}

inline void SetStringIfDefined(std::string* dest, TiXmlElement* elem)
{
    if (elem && elem->GetText())
    {
        *dest = elem->GetText();
    }
}

inline void SetDoubleIfDefined(double* dest, TiXmlElement* elem)
{
    if (elem && elem->GetText())
    {
        *dest = std::stod(elem->GetText());
    }
}

inline void SetFloatIfDefined(float* dest, TiXmlElement* elem)
{
    if (elem && elem->GetText())
    {
        *dest = std::stof(elem->GetText());
    }
}

// Rather overload it than 10 different functions ?

template <typename T>
inline bool ParseValueFromXmlElem(T* val, TiXmlElement* pRootElem, const std::string& pathToNode)
{
    if (TiXmlElement* pDestElem = GetTiXmlElementFromPath(pRootElem, pathToNode))
    {
        ParseValueFromXmlElem(val, pDestElem);
        return true;
    }

    return false;
}

inline bool ParseValueFromXmlElem(bool* pDest, TiXmlElement* pElemSource)
{
    if (pElemSource && pElemSource->GetText())
    {
        std::string opt = pElemSource->GetText();
        if (opt == "true")
        {
            *pDest = true;
        }
        else if (opt == "false")
        {
            *pDest = false;
        }
        return true;
    }

    return false;
}

inline bool ParseValueFromXmlElem(unsigned* pDest, TiXmlElement* pElemSource)
{
    if (pElemSource && pElemSource->GetText())
    {
        *pDest = std::stoi(pElemSource->GetText());
        return true;
    }

    return false;
}

inline bool ParseValueFromXmlElem(unsigned long* pDest, TiXmlElement* pElemSource)
{
    if (pElemSource && pElemSource->GetText())
    {
        *pDest = std::stoul(pElemSource->GetText());
        return true;
    }

    return false;
}

inline bool ParseValueFromXmlElem(long* pDest, TiXmlElement* pElemSource)
{
    if (pElemSource && pElemSource->GetText())
    {
        *pDest = std::stol(pElemSource->GetText());
        return true;
    }

    return false;
}

inline bool ParseValueFromXmlElem(int* pDest, TiXmlElement* pElemSource)
{
    if (pElemSource && pElemSource->GetText())
    {
        *pDest = std::stoi(pElemSource->GetText());
        return true;
    }

    return false;
}

inline bool ParseValueFromXmlElem(float* pDest, TiXmlElement* pElemSource)
{
    if (pElemSource && pElemSource->GetText())
    {
        *pDest = std::stof(pElemSource->GetText());
        return true;
    }

    return false;
}

inline bool ParseValueFromXmlElem(double* pDest, TiXmlElement* pElemSource)
{
    if (pElemSource && pElemSource->GetText())
    {
        *pDest = std::stof(pElemSource->GetText());
        return true;
    }

    return false;
}

inline bool ParseValueFromXmlElem(std::string* pDest, TiXmlElement* pElemSource)
{
    if (pElemSource && pElemSource->GetText())
    {
        *pDest = std::string(pElemSource->GetText());
        return true;
    }

    return false;
}

class Point;
bool ParseValueFromXmlElem(Point* pDest, TiXmlElement* pElem, const char* elemAttrNameX, const char* elemAttrNameY);

inline bool ParseAttributeFromXmlElem(std::string* pDest, const char* attrName, TiXmlElement* pElemSource)
{
    if (pElemSource && pElemSource->Attribute(attrName))
    {
        *pDest = pElemSource->Attribute(attrName);
        return true;
    }

    return false;
}

class Point;
bool SetPointIfDefined(Point* pDest, TiXmlElement* pElem, const char* elemAttrNameX, const char* elemAttrNameY);

#define SetEnumIfDefined(pDest, pElem, enumType) \
if (pElem && pElem->GetText()) \
{ \
    *pDest = enumType(std::stoi(pElem->GetText())); \
} \

#endif