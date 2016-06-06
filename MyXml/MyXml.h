#pragma once

#include <string>
#include <stdexcept>

extern "C"
{
#include <libxml/tree.h>
#include <libxml/xpath.h>
}

//libxml2ÇÃí¥ç≈í·å¿ÇÃÉâÉbÉpÅ[
namespace MyXml
{
	class Doc;

	class XPathResult
	{
		friend Doc;

	private:
		xmlXPathObjectPtr xpathObj;

		explicit XPathResult(xmlXPathObjectPtr xpathObj) noexcept;
			
	public:
		XPathResult() = delete;

		XPathResult(const XPathResult &obj) noexcept;
		XPathResult &operator=(const XPathResult &obj) noexcept;
		XPathResult(XPathResult &&obj) noexcept;
		XPathResult &operator=(XPathResult &&obj) noexcept;

		xmlXPathObjectType GetType() const noexcept;
		xmlNodeSetPtr GetNodeSet() const noexcept;
		int GetBool() const noexcept;
		double GetFloat() const noexcept;
		std::string GetString() const; 

		~XPathResult() noexcept;
	};

	class XPathExpr
	{
	private:
		std::string expr;
		xmlXPathCompExprPtr compiled;
	public:
		XPathExpr() = delete;
		XPathExpr(const XPathExpr &obj) = delete;
		XPathExpr &operator=(const XPathExpr &obj) = delete;

		XPathExpr(XPathExpr &&obj) noexcept;
		XPathExpr &operator=(XPathExpr &&obj) noexcept;

		explicit XPathExpr(const std::string &xpath);
		std::string GetExpr() const noexcept;
		xmlXPathCompExprPtr GetCompiled() const noexcept;

		~XPathExpr() noexcept;
	};

	class Doc
	{
	private:
		xmlDocPtr doc;
		mutable xmlXPathContextPtr  xpathCtx;

	public:
		Doc() = delete;
		Doc(const Doc &obj) = delete;
		Doc &operator=(const Doc &obj) = delete;
		
		explicit Doc(xmlDocPtr doc) noexcept;
		Doc(Doc &&obj) noexcept;
		Doc &operator=(Doc &&obj) noexcept;
		
		xmlDocPtr GetPtr() const noexcept;
		XPathResult XPath(const std::string &xpath, xmlNodePtr root = nullptr) const;
		XPathResult XPath(const XPathExpr &xpath, xmlNodePtr root = nullptr) const;

		~Doc() noexcept;
	};
}
