#include "XmlUtil.h"
#include "../SharedDefines.h"
#include "Util.h"

bool SetPointIfDefined(Point* pDest, TiXmlElement* pElem, const char* elemAttrNameX, const char* elemAttrNameY)
{
    if (pElem)
    {
        pElem->Attribute(elemAttrNameX, &pDest->x);
        pElem->Attribute(elemAttrNameY, &pDest->y);
        return true;
    }

    return false;
}

// pathToNode in format: elem1.elem2.elem3
TiXmlElement* GetTiXmlElementFromPath(TiXmlElement* pRootElem, const std::string& pathToNode)
{
    assert(pRootElem != NULL);

    std::vector<std::string> nodes;
    std::string tmpPath = pathToNode;
    std::replace(tmpPath.begin(), tmpPath.end(), '.', ' ');

    Util::SplitStringIntoVector(tmpPath, nodes);

    if (nodes.empty())
    {
        return NULL;
    }

    // Check for root element here
    if (std::string(pRootElem->Value()) != nodes[0])
    {
        return NULL;
    }
    else
    {
        nodes.erase(nodes.begin());
    }

    TiXmlHandle rootHandle(pRootElem);
    for (const std::string& node : nodes)
    {
        rootHandle = rootHandle.FirstChild(node.c_str());
    }
    return rootHandle.ToElement();
}

bool ParseValueFromXmlElem(Point* pDest, TiXmlElement* pElem, const char* elemAttrNameX, const char* elemAttrNameY)
{
    if (pElem)
    {
        pElem->Attribute(elemAttrNameX, &pDest->x);
        pElem->Attribute(elemAttrNameY, &pDest->y);
        return true;
    }

    return false;
}