#include "PCH.h"
#include "Xml.h"

#include "IO/File.h"
#include "rapidxml/Include/rapidxml.hpp"
#include "rapidxml/Include/rapidxml_print.hpp"

namespace XE
{
	using xml_document = rapidxml::xml_document<Char>();
	using xml_node = rapidxml::xml_node<Char>;
	using xml_attribute = rapidxml::xml_attribute<Char>;

	FXmlNode FXmlNode::GetParent() const
	{
		if (!Node)
			return {};

		return FXmlNode(Document, Node->parent());
	}

	FStringView FXmlNode::GetName() const
	{
		if (!Node)
			return {};

		return FStringView(Node->name(), Node->name_size());
	}

	FXmlNode FXmlNode::GetFirstChild() const
	{
		if (!Node)
			return {};
		return FXmlNode(Document, Node->first_node());
	}

	FXmlNode FXmlNode::GetFirstChild(FStringView Name) const
	{
		if (!Node)
			return {};
		return FXmlNode(Document, Node->first_node(Name.Data, Name.Size));
	}

	size_t FXmlNode::GetChildrenCount() const
	{
		if (!Node)
			return 0;

		size_t Count = 0;
		xml_node * CurrNode = Node->first_node();
		while (CurrNode)
		{
			++Count;
			CurrNode = CurrNode->next_sibling();
		}
		return Count;
	}
	
	TVector<FXmlNode> FXmlNode::GetChildren() const
	{
		if (!Node)
			return {};

		TVector<FXmlNode> Nodes;
		xml_node * CurrNode = Node->first_node();
		while (CurrNode)
		{
			Nodes.Add(FXmlNode(Document, CurrNode));
			CurrNode = CurrNode->next_sibling();
		}
		return Nodes;
	}

	TVector<FXmlNode> FXmlNode::GetChildren(FStringView Name) const
	{
		if (!Node)
			return {};

		TVector<FXmlNode> Nodes;
		xml_node * CurrNode = Node->first_node(Name.Data, Name.Size);
		while (CurrNode)
		{
			Nodes.Add(FXmlNode(Document, CurrNode));
			CurrNode = CurrNode->next_sibling(Name.Data, Name.Size);
		}
		return Nodes;
	}


	TVector<FXmlAttribute> FXmlNode::GetAttributes() const
	{
		if (!Node)
			return {};

		TVector<FXmlAttribute> Attributes;
		xml_attribute * CurrAttribute = Node->first_attribute();
		while (CurrAttribute)
		{
			Attributes.Add(FXmlAttribute(Document, CurrAttribute));
			CurrAttribute = CurrAttribute->next_attribute();
		}
		return Attributes;
	}
	
	TVector<FXmlAttribute> FXmlNode::GetAttributes(FStringView Name) const
	{
		if (!Node)
			return {};

		TVector<FXmlAttribute> Attributes;
		xml_attribute * CurrAttribute = Node->first_attribute(Name.Data, Name.Size);
		while (CurrAttribute)
		{
			Attributes.Add(FXmlAttribute(Document, CurrAttribute));
			CurrAttribute = CurrAttribute->next_attribute(Name.Data, Name.Size);
		}
		return Attributes;
	}
	
	FStringView FXmlNode::GetAttribute(FStringView Name, FStringView DefaultValue) const
	{
		if (!Node)
			return {};

		xml_attribute * Attribute = Node->first_attribute(Name.Data, Name.Size);
		if (!Attribute)
			return DefaultValue;

		return FStringView(Attribute->value(), Attribute->value_size());
	}

	bool FXmlNode::SetAttribute(FStringView Name, FStringView Value) const
	{
		if (!Node)
			return false;

		xml_attribute * Attribute = Node->first_attribute(Name.Data, Name.Size);
		if (Attribute)
		{
			Attribute->value(Value.Data, Value.Size);
			return true;
		}
		else
		{
			Attribute = Document->allocate_attribute(Document->allocate_string(Name.Data, Name.Size), Document->allocate_string(Value.Data, Value.Size), Name.Size, Value.Size);
			Node->append_attribute(Attribute);
			return false;
		}
	}

	void FXmlNode::AppendChild(FXmlNode & Child)
	{
		if (!Node || !Child.Node)
			return;

		Node->append_node(Child.Node);
	}

	FXmlNode FXmlNode::CreateChild(FStringView Name)
	{
		if (!Node)
			return {};
		xml_node * NewNode = nullptr;
		if (Name.IsEmpty())
			NewNode = Document->allocate_node(rapidxml::node_element, nullptr, nullptr, 0, 0);
		else
			NewNode = Document->allocate_node(rapidxml::node_element, Document->allocate_string(Name.Data, Name.Size), nullptr, Name.Size, 0);
		Node->append_node(NewNode);
		return FXmlNode(Document, NewNode);
	}

	void FXmlNode::RemoveChild(FXmlNode & Child)
	{
		if (!Node || !Child.Node)
			return;
		Node->remove_node(Child.Node);
	}

	void FXmlNode::RemoveAllChildren()
	{
		if (!Node)
			return;
		Node->remove_all_nodes();
	}

	FXmlNode FXmlAttribute::GetOwner() const
	{
		if (!Attribute)
			return {};

		return FXmlNode(Document, Attribute->parent());
	}
	FStringView FXmlAttribute::GetName() const
	{
		if (!Attribute)
			return {};

		return FStringView(Attribute->name(), Attribute->name_size());
	}
	
	FStringView FXmlAttribute::GetValue() const
	{
		if (!Attribute)
			return {};

		return FStringView(Attribute->value(), Attribute->value_size());
	}
	
	FXmlDocument::FXmlDocument()
	{
	}

	FXmlDocument::~FXmlDocument()
	{
		if (RootNode.Document)
		{
			delete RootNode.Document;
			RootNode.Document = nullptr;
		}
	}

	FXmlNode FXmlDocument::FindRootNode(FStringView Name) const
	{
		if (!RootNode)
			return {};

		return FXmlNode(RootNode.Document, RootNode.Document->first_node(Name.Data, Name.Size));
	}

	FXmlNode FXmlDocument::CreateRootNode(FStringView Name)
	{
		if (!RootNode.Document)
			RootNode.Document = new rapidxml::xml_document<Char>();
		else
			RootNode.Document->clear();
		RootNode.Node = nullptr;

		xml_node * HeadNode = RootNode.Document->allocate_node(rapidxml::node_pi, RootNode.Document->allocate_string(Str("xml version='1.0' encoding='utf-8'")));
		RootNode.Document->append_node(HeadNode);

		RootNode.Node = RootNode.Document->allocate_node(rapidxml::node_element, RootNode.Document->allocate_string(Name.Data, Name.Size), nullptr, Name.Size, 0);
		RootNode.Document->append_node(RootNode.Node);
		return RootNode;
	}

	bool FXmlDocument::LoadFromFile(FStringView FilePath_)
	{
		if (!RootNode.Document)
			RootNode.Document = new rapidxml::xml_document<Char>();
		else
			RootNode.Document->clear();
		RootNode.Node = nullptr;

		EFileBOM FileBOM = EFileBOM::None;
		FString FileTextContent = FFile::ReadToString(FilePath_, &FileBOM);
		if (FileTextContent.IsEmpty())
			return false;

		if (FileTextContent.StartsWith(Str("<?xml")))
		{
			const Char ENCODING_STR[] = Str("encoding=\"");
			size_t EncodingStringIndex = FileTextContent.FindFirst(ENCODING_STR);
			if (EncodingStringIndex != NullIndex)
			{
				EncodingStringIndex += Size(ENCODING_STR) - 1;
				size_t EncodingStringEndIndex = FileTextContent.Slice(EncodingStringIndex).FindFirst(Str('\"'));
				if (EncodingStringEndIndex != NullIndex)
				{
					FStringView XmlEncoding = FileTextContent.Slice(EncodingStringIndex, EncodingStringEndIndex);
#ifdef XE_ANSI
					if (Strings::ICCompare(XmlEncoding, FStringView(Str("utf-8"))) == 0)
					{
						FileTextContent = Strings::Utf8ToAnsi(FU8StringView(reinterpret_cast<const char8_t *>(FileTextContent.Data), FileTextContent.Size));
					}
#else
					if (Strings::ICCompare(XmlEncoding, FStringView(Str("ansi"))) == 0)
					{
						FileTextContent = Strings::AnsiToUtf8(FAStringView(reinterpret_cast<const char *>(FileTextContent.Data), FileTextContent.Size));
					}
#endif
				}
			}
		}
		Char * DocumentText = RootNode.Document->allocate_string(FileTextContent.GetData(), FileTextContent.GetSize() + 1);
		DocumentText[FileTextContent.GetSize()] = 0;
		RootNode.Document->parse<0>(DocumentText);
		RootNode.Node = RootNode.Document->first_node();
#ifdef _DEBUG
		RootNode.DebugName = RootNode.GetName();
		FilePath = FilePath_;
#endif
		return true;
	}

	bool FXmlDocument::SaveToFile(FStringView FilePath_)
	{
		std::wstring text;
		rapidxml::print(std::back_inserter(text), *(RootNode.Document), 0);
		FStringView FileTextContent = Strings::FromWide(text.c_str(), int(text.length()));
		FFile::SaveToFile(FilePath_, FileTextContent);
		return true;
	}

	FString FXmlAttributes::GetString(FStringView XmlPath, FStringView NodePath, FStringView AttributeName)
	{
		FXmlDocument Xml(XmlPath);
		if (!Xml.GetRootNode().IsValid())
			return {};

		FXmlNode Node = Xml.GetRootNode();
		TVector<FStringView> Names = Strings::Split(NodePath, { Str("/\\") });
		for (size_t Index = 0; Index < Names.GetSize(); ++Index)
		{
			if (Names[Index].IsEmpty())
				continue;

			Node = Node.GetFirstChild(Names[Index]);
		}
		return Node.GetAttribute(AttributeName);
	}

	FXmlNodeDataProvider::FXmlNodeDataProvider(FXmlNode XmlNode_)
	{
		LoadFromXmlNode(XmlNode_);
	}

	void FXmlNodeDataProvider::LoadFromXmlNode(FXmlNode XmlNode_)
	{
		FXmlNodeDataProvider * LastChild = nullptr;
		for (FXmlNode & Child : XmlNode_.GetChildren())
		{
			TReferPtr<FXmlNodeDataProvider> XmlNodeDataProvider = MakeRefer<FXmlNodeDataProvider>(Child);
			Children.Add(XmlNodeDataProvider);
			if (LastChild)
				LastChild->Silbing = XmlNodeDataProvider.Get();
			LastChild = XmlNodeDataProvider.Get();
		}

		FXmlAttributePropertyProvider * LastAttribute = nullptr;
		for (FXmlAttribute & XmlAttribute : XmlNode_.GetAttributes())
		{
			TReferPtr<FXmlAttributePropertyProvider> XmlAttributePropertyProvider = MakeRefer<FXmlAttributePropertyProvider>(XmlAttribute);
			Attributes.Add(XmlAttributePropertyProvider);
			if (LastAttribute)
				LastAttribute->Silbing = XmlAttributePropertyProvider.Get();
			LastAttribute = XmlAttributePropertyProvider.Get();
		}

		XmlNode = XmlNode_;
	}

	FXmlDataProvider::FXmlDataProvider(FStringView FilePath)
	{
		LoadFromFile(FilePath);
	}

	void FXmlDataProvider::LoadFromFile(FStringView FilePath)
	{
		XmlDocument = MakeUnique<FXmlDocument>(FilePath);
		if (!XmlDocument->RootNode)
			return;
		LoadFromXmlNode(XmlDocument->RootNode);
	}
}
