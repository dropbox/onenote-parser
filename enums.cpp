#include "enums.hpp"
#include "properties.hpp"
#include "one_note.hpp"
void ObjectSpaceObjectPropSet::print(const OneNote *document) const {
  return body.print(document);
}
void PropertySet::print(const OneNote *document)const {
  IncIndentLevel inc;
  for (const auto &child : rgPridsData) {
    child.print(document);
  }
}
extern const char * get_property_id_name(uint64_t id);
void PropertyValue::print(const OneNote*document)const {
  bool isRawText = true;//std::string(get_property_id_name(propertyID.id)).find("TextE")!=-1;

  if (isRawText) {
    printf("%s<%s", get_indent().c_str(),get_property_id_name(propertyID.id));
  }
  if (propertyID.type && propertyID.type <= 6) {
    if (isRawText) {
      printf("(%ld)", scalar);
    }
  } else if (propertyID.type == 7) {
    OneNotePtr content(document);
    error err = content.reposition(rawData);
    assert(err == Ok && "Already parsed, already filtered for errors");
    if (isRawText) {
      printf(" [");
      content.dump_hex();
      printf("]");
    }
  } else if (propertyID.type == 0x9 || propertyID.type == 0x8 ||
	     propertyID.type == 0xb || propertyID.type == 0xc ||
	     propertyID.type == 0xa || propertyID.type == 0xd) {
      const char * xtype = "contextID";
      if (propertyID.type == 0x8 || propertyID.type == 0x9) {
	xtype = "OIDs";
      }
      if (propertyID.type == 0xa || propertyID.type == 0xb) {
	xtype = "OSIDS";
      }
      if (isRawText) {
	if (!compactIDs.empty()) {
	  printf("\n");
	}
	for (const auto &compactID : compactIDs) {
	  IncIndentLevel sub;
	  printf("%s%s[%s]\n", get_indent().c_str(),xtype, compactID.toString().c_str());
	  auto where = document->guid_to_object.find(compactID.guid);
	  if (where != document->guid_to_object.end()) {
	    where->second.dereference(document)->print(document);
	  }
	}
      }
  } else if (propertyID.type == 0x10 || propertyID.type == 0x11){
    if (isRawText) {
    printf("SubProperty\n");
    }
    propertySet.print(document);
  }
  if (isRawText) {
  printf(">\n");
  }
}


void FileNode::print(const OneNote*document) const {
  IncIndentLevel inc;
  bool shouldPrintHeader = std::string(fnd_name()).find("ObjectDec") == std::string::npos;
  if (gosid != ExtendedGUID::nil() && shouldPrintHeader) {
    printf("%s[beg %s]:%s\n", get_indent().c_str(), fnd_name(), gosid.toString().c_str());
  }
  property_set.print(document);
  if (!children.children.empty()) {
    if (shouldPrintHeader) {
      printf("%schildren\n",get_indent().c_str());
    }
    for (const auto&child : children.children) {
      child.print(document);
    }
  }
  if (id == RevisionRoleDeclarationFND || id == RevisionRoleAndContextDeclarationFND) {
    printf("%s[Revision Role %d]\n", get_indent().c_str(), sub_type.RevisionRoleDeclaration.RevisionRole);

  }
  if (id == RevisionManifestStart4FND ||
      id == RevisionManifestStart6FND ||
      id == RevisionManifestStart7FND) {
    printf("%s[revisionRole %d]\n", get_indent().c_str(), sub_type.RevisionManifest.revisionRole);

  }
  if ((gctxid != ExtendedGUID::nil()||id == RevisionManifestStart7FND) && shouldPrintHeader) {
    printf("%s[gctxid %s]\n", get_indent().c_str(), gctxid.toString().c_str());
  }
  if (gosid != ExtendedGUID::nil() && shouldPrintHeader) {
    printf("%s[end %s]:%s\n", get_indent().c_str(), fnd_name(), gosid.toString().c_str());

  }
}
bool FileNode::is_object_declaration() const {
    using namespace CanRevise;
return id == ObjectDeclarationWithRefCountFNDX || id==ObjectDeclaration2RefCountFND  || id == ReadOnlyObjectDeclaration2RefCountFND
    || id == ObjectDeclarationFileData3RefCountFND ||
    id == ObjectDeclarationFileData3LargeRefCountFND;
}
JCID FileNode::object_declaration_jcid() const {
    assert(is_object_declaration());
    return sub_type.ObjectDeclarationWithRefCount.body.jcid;
}
