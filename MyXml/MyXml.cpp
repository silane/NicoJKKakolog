#include "MyXml.h"

namespace MyXml
{
XPathResult::XPathResult(xmlXPathObjectPtr xpathObj) noexcept: xpathObj(xpathObj)
{
}

XPathResult::XPathResult(const XPathResult &obj) noexcept: xpathObj(xmlXPathObjectCopy(obj.xpathObj))
{
}

XPathResult &XPathResult::operator=(const XPathResult &obj) noexcept
{
	if(this==&obj)
		return *this;

	this->xpathObj=xmlXPathObjectCopy(obj.xpathObj);
	return *this;
}
	
XPathResult::XPathResult(XPathResult &&obj) noexcept: xpathObj(obj.xpathObj)
{
	obj.xpathObj=nullptr;
}

	XPathResult &XPathResult::operator=(XPathResult &&obj) noexcept
{
	if(this==&obj)
		return *this;

	xmlXPathFreeObject(this->xpathObj);
	this->xpathObj=obj.xpathObj;
	obj.xpathObj=nullptr;

	return *this;
}

xmlXPathObjectType XPathResult::GetType() const noexcept
{
	return xpathObj->type;
}

xmlNodeSetPtr XPathResult::GetNodeSet() const noexcept
{
	return xpathObj->nodesetval;
}

int XPathResult::GetBool() const noexcept
{
	return xpathObj->boolval;
}

double XPathResult::GetFloat() const noexcept
{
	return xpathObj->floatval;
}

std::string XPathResult::GetString() const
{
	return std::string((char *)xpathObj->stringval);
}

XPathResult::~XPathResult() noexcept
{
	xmlXPathFreeObject(xpathObj);
	xpathObj=nullptr;
}


XPathExpr::XPathExpr(const std::string &xpath) :
	expr(xpath), compiled(xmlXPathCompile(BAD_CAST xpath.c_str()))
{
	if (!compiled)
		throw std::runtime_error("Error: unable to evaluate xpath expression \"" + xpath + "\"");
}

XPathExpr::XPathExpr(XPathExpr &&obj) noexcept :
	expr(std::move(obj.expr)), compiled(obj.compiled)
{
	obj.compiled = nullptr;
}

XPathExpr &XPathExpr::operator=(XPathExpr &&obj) noexcept
{
	if (this == &obj)
		return *this;

	xmlXPathFreeCompExpr(this->compiled);

	this->expr = std::move(obj.expr);
	this->compiled = obj.compiled;

	obj.compiled = nullptr;

	return *this;
}

std::string XPathExpr::GetExpr() const noexcept
{
	return this->expr;
}

xmlXPathCompExprPtr XPathExpr::GetCompiled() const noexcept
{
	return this->compiled;
}

XPathExpr::~XPathExpr() noexcept
{
	xmlXPathFreeCompExpr(this->compiled);
}


Doc::Doc(xmlDocPtr doc) noexcept: doc(doc), xpathCtx(nullptr)
{
}

Doc::Doc(Doc &&obj) noexcept: doc(obj.doc), xpathCtx(obj.xpathCtx)
{
	obj.doc=nullptr;
	obj.xpathCtx=nullptr;
}

Doc &Doc::operator=(Doc &&obj) noexcept
{
	if(this==&obj)
		return *this;

	xmlXPathFreeContext(this->xpathCtx);
	xmlFreeDoc(this->doc);

	this->doc=obj.doc;
	this->xpathCtx=obj.xpathCtx;
	obj.doc=nullptr;
	obj.xpathCtx=nullptr;

	return *this;
}

xmlDocPtr Doc::GetPtr() const noexcept
{
	return doc;
}

XPathResult Doc::XPath(const std::string &xpath,xmlNodePtr root) const
{
	if(!xpathCtx)
		xpathCtx=xmlXPathNewContext(doc);

	if(!xpathCtx)
	{
		throw std::runtime_error("Error: unable to create new XPath context");
	}

	if (root)
		xpathCtx->node = root;
	else
		xpathCtx->node = nullptr;
	
	xmlXPathObjectPtr xpathObj;
	xpathObj=xmlXPathEval(BAD_CAST xpath.c_str(),xpathCtx);
	if(!xpathObj)
	{
		throw std::runtime_error("Error: unable to evaluate xpath expression \"" + xpath + "\"");
	}

	return XPathResult(xpathObj);
}

XPathResult Doc::XPath(const XPathExpr &xpath, xmlNodePtr root) const
{
	if (!xpathCtx)
		xpathCtx = xmlXPathNewContext(doc);

	if (!xpathCtx)
	{
		throw std::runtime_error("Error: unable to create new XPath context");
	}

	if (root)
		xpathCtx->node = root;
	else
		xpathCtx->node = nullptr;

	xmlXPathObjectPtr xpathObj;
	xpathObj = xmlXPathCompiledEval(xpath.GetCompiled(), xpathCtx);
	if (!xpathObj)
	{
		throw std::runtime_error("Error: unable to evaluate xpath expression \"" + xpath.GetExpr() + "\"");
	}

	return XPathResult(xpathObj);
}

Doc::~Doc() noexcept
{
	xmlXPathFreeContext(xpathCtx);
	xmlFreeDoc(doc);
	
	xpathCtx=nullptr;
	doc=nullptr;
}
}
