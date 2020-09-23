#pragma once

#include "CoreInc.h"
#include "IO/File.h"

namespace rapidxml
{
	template<class CharT>
	class xml_node;

	template<class CharT>
	class xml_attribute;

	template<class CharT>
	class xml_document;
}

namespace XE
{
	struct FXmlAttribute;
	
	struct CORE_API FXmlNode
	{
		friend class FXmlDocument;

		FXmlNode() = default;
		FXmlNode(rapidxml::xml_document<Char> * DocumentIn, rapidxml::xml_node<Char> * NodeIn) : Document(DocumentIn), Node(NodeIn)
		{

#ifdef _DEBUG
			DebugName = GetName();
#endif
		}
		bool IsValid() const { return !!Node; }
		explicit operator bool() const { return !!Node; }
		
		FXmlNode GetParent() const;
		FStringView GetName() const;

		FXmlNode GetFirstChild() const;
		FXmlNode GetFirstChild(FStringView Name) const;

		size_t GetChildrenCount() const;
		TVector<FXmlNode> GetChildren() const;
		TVector<FXmlNode> GetChildren(FStringView Name) const;

		TVector<FXmlAttribute> GetAttributes() const;
		TVector<FXmlAttribute> GetAttributes(FStringView Name) const;

		FStringView GetAttribute(FStringView Name, FStringView DefaultValue = FStringView()) const;
		bool SetAttribute(FStringView Name, FStringView Value) const;

		void AppendChild(FXmlNode & Child);
		FXmlNode CreateChild(FStringView Name);

		void RemoveChild(FXmlNode & Child);
		void RemoveAllChildren();

	private:
#ifdef _DEBUG
		FStringView DebugName;
#endif
		rapidxml::xml_document<Char> * Document = nullptr;
		rapidxml::xml_node<Char> * Node = nullptr;
	};

	struct CORE_API FXmlAttribute
	{
		friend class FXmlDocument;

		FXmlAttribute() = default;
		FXmlAttribute(rapidxml::xml_document<Char> * DocumentIn, rapidxml::xml_attribute<Char> * AttributeIn) : Document(DocumentIn), Attribute(AttributeIn)
		{

#ifdef _DEBUG
			DebugName = GetName();
			DebugValue = GetValue();
#endif
		}
		bool IsValid() const { return !!Attribute; }
		explicit operator bool() const { return !!Attribute; }

		FXmlNode GetOwner() const;
		FStringView GetName() const;
		FStringView GetValue() const;

	private:
#ifdef _DEBUG
		FStringView DebugName;
		FStringView DebugValue;
#endif
		rapidxml::xml_document<Char> * Document = nullptr;
		rapidxml::xml_attribute<Char> * Attribute = nullptr;
	};


	class CORE_API FXmlDocument
	{
	public:
		FXmlDocument();
		FXmlDocument(FStringView FilePath_) { LoadFromFile(FilePath_); }
		~FXmlDocument();

	public:
		FXmlNode GetRootNode() const { return RootNode; }
		FXmlNode FindRootNode(FStringView Name) const;

		FXmlNode CreateRootNode(FStringView Name);

	public:
		bool LoadFromFile(FStringView FilePath_);
		bool SaveToFile(FStringView FilePath_);

	public:
#ifdef _DEBUG
		FString FilePath;
#endif
		FXmlNode RootNode;
	};

	class CORE_API FXmlAttributes
	{
	public:
		FXmlAttributes() = delete;

	public:
		static FString GetString(FStringView XmlPath, FStringView NodePath, FStringView AttributeName);
		static bool GetBool(FStringView XmlPath, FStringView NodePath, FStringView AttributeName)
		{
			return Strings::ToBool(GetString(XmlPath, NodePath, AttributeName));
		}
	};


	class CORE_API FXmlAttributePropertyProvider : public TRefer<IDataProvider>
	{
	public:
		FXmlAttributePropertyProvider() = default;
		FXmlAttributePropertyProvider(FXmlAttribute XmlAttribute_) : XmlAttribute(XmlAttribute_) {}

		FStringView GetName() const override { return XmlAttribute.GetName(); }
		FStringView GetValue() const override { return XmlAttribute.GetValue(); }
		const IDataProvider * GetSibling() const override { return Silbing; }

		const IDataProvider * GetFirstAttribute() const override { return nullptr; }
		const IDataProvider * GetFirstChild() const override { return nullptr; }

	private:
		FXmlAttribute XmlAttribute;

	public:
		const FXmlAttributePropertyProvider * Silbing = nullptr;
	};

	class CORE_API FXmlNodeDataProvider : public TRefer<IDataProvider>
	{
	public:
		FXmlNodeDataProvider() = default;
		FXmlNodeDataProvider(FXmlNode XmlNode_);

		void LoadFromXmlNode(FXmlNode XmlNode_);
		
	public:
		FStringView GetName() const override { return XmlNode.GetName(); }
		FStringView GetValue() const override { return FStringView::Empty; }
		const IDataProvider * GetSibling() const override { return Silbing; }

		const IDataProvider * GetFirstAttribute() const override { return Attributes.IsEmpty() ? nullptr : Attributes.Front().Get(); }
		const IDataProvider * GetFirstChild() const override { return Children.IsEmpty() ? nullptr : Children.Front().Get(); }
		
	private:
		FXmlNode XmlNode;
		TVector<TReferPtr<FXmlAttributePropertyProvider>> Attributes;
		TVector<TReferPtr<IDataProvider>> Children;
	public:
		const FXmlNodeDataProvider * Silbing = nullptr;
	};

	class CORE_API FXmlDataProvider : public FXmlNodeDataProvider
	{
	public:
		FXmlDataProvider() = default;
		FXmlDataProvider(FStringView FilePath);

		void LoadFromFile(FStringView FilePath);
	private:
		TUniquePtr<FXmlDocument> XmlDocument;
	};
}
