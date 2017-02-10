#ifndef _ONE_NOTE_PTR_HPP_
#define _ONE_NOTE_PTR_HPP_
#include <vector>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <map>
#include "guid.hpp"

#define htole64(x) (x)
#define htole32(x) (x)
struct GUID;
struct CompactID;
struct ExtendedGUID;
struct FileChunkReference64;
typedef FileChunkReference64 FileChunkReference32;
typedef FileChunkReference64 FileChunkReference64x32;
struct FileNodeListHeader;
struct FileNodeList;
struct FileNode;
struct ObjectDeclarationWithRefCountBody;
struct ObjectInfoDependencyOverrideData;
struct FileDataStoreObject;
struct ObjectSpaceObjectPropSet;
struct ObjectSpaceObjectStreamOfOIDsOSIDsOrContextIDs;
struct PropertySet;
struct PropertyValue;
struct ObjectStreamCounters;
struct FileNodePtr;
struct Revision {
  std::map<uint32_t, GUID> global_id;
  std::vector<FileNodePtr> manifest_list;
  ExtendedGUID gosid;
  ExtendedGUID dependent;
};
std::string get_indent();
class IncIndentLevel {
public:
  IncIndentLevel();
  ~IncIndentLevel();
};

class DecIndentLevel {
public:
  DecIndentLevel();
  ~DecIndentLevel();
};

struct PropertyID {
    uint32_t id; // 26 bits
    uint8_t type; // 5 bits
  bool inline_bool; // 1 bit
  std::string toString() const;
};
struct OneNote;
const char *propertyID_to_string(PropertyID pid);


struct FileNodePtr {
  std::vector<size_t> offsets;
  FileNode*dereference(OneNote*document) const;
  const FileNode*dereference(const OneNote*document)const;
};
struct FileNodePtrBackPush {
  FileNodePtr *parent;
  FileNodePtrBackPush(const FileNodePtrBackPush&parent) = delete;
  FileNodePtrBackPush(const FileNodePtrBackPush&&parent) = delete;
  FileNodePtrBackPush& operator =(const FileNodePtrBackPush&parent) = delete;
  FileNodePtrBackPush(FileNodePtr&parent) {
    this->parent = &parent;
    this->parent->offsets.push_back(0);
  }
  void inc() {
    ++parent->offsets.back();
  }
  ~FileNodePtrBackPush() {
    parent->offsets.pop_back();
  }
};
struct OneNotePtr {
  const OneNote *document;
  uint64_t offset;
  uint64_t end;
  static error wrap(error err) {
    return ::wrap(err);
  }
  OneNotePtr(const OneNote * doc);
  uint64_t room_left() const {
    return end - offset;
  }
  void dump_hex() const;
  const uint8_t *data()const;
  const uint64_t size()const {return end - offset;}
  error deserialize_bytes(uint8_t *data, uint64_t num_bytes);
  error deserialize_uint8_t(uint8_t *data);
  error deserialize_uint16_t(uint16_t *data);
  error deserialize_int32_t(int32_t *data) {
    return deserialize_uint32_t((uint32_t*)data);
  }
  error deserialize_int64_t(int64_t *data) {
    return deserialize_uint64_t((uint64_t*)data);
  }
  error deserialize_uint32_t(uint32_t *data);
  error deserialize_uint64_t(uint64_t *data);
  error deserialize_GUID(GUID *data);
  error deserialize_ExtendedGUID(ExtendedGUID *data);
    error deserialize_PropertyID(PropertyID *data);
  error deserialize_PropertySet(const ObjectSpaceObjectPropSet&object_ref,
				ObjectStreamCounters*counters,
				PropertySet *data);
  error deserialize_PropertyValueFromPropertyID(PropertyID propertyID,
						const ObjectSpaceObjectPropSet&object_ref,
						ObjectStreamCounters*counters,
						PropertyValue *data);

  error deserialize_ObjectInfoDependencyOverrideData(ObjectInfoDependencyOverrideData *data);
  error deserialize_ObjectDeclarationWithRefCountBody(ObjectDeclarationWithRefCountBody *data);
  error deserialize_ObjectDeclaration2Body(ObjectDeclarationWithRefCountBody *data);
  error deserialize_CompactID(CompactID *data);
  error deserialize_VarFileChunkReference(FileChunkReference64 *data,
					  uint32_t stp_fmt,
					  uint32_t cb_fmt);
  error deserialize_FileChunkReference32(FileChunkReference32 *data);
  error deserialize_FileChunkReference64(FileChunkReference64 *data);
  error deserialize_FileChunkReference64x32(FileChunkReference64x32 *data);
  error deserialize_FileDataStoreObject(FileDataStoreObject*data);
  error deserialize_ObjectSpaceObjectPropSet(ObjectSpaceObjectPropSet*data);
  error deserialize_ObjectSpaceObjectStreamOfOIDsOSIDsOrContextIDs(ObjectSpaceObjectStreamOfOIDsOSIDsOrContextIDs *data);
  error deserialize_FileNodeListHeader(FileNodeListHeader *list);
    // you may pass a partially filled FileNodeList--this will add more items
  error deserialize_FileNodeListFragment(FileNodeList *list, FileChunkReference64x32 *next,
					 FileNodePtr *curPath);
  // this chases pointers to fulfil the full fragment
  error deserialize_FileNodeList(FileNodeList*list, FileNodePtr *curPath);
  error deserialize_FileNode(FileNode*list, FileNodePtr *curPath);
  error postprocess_object_declaration_contents(FileNode*data,
						const FileNodePtr &curPtr);
  const uint8_t& operator[](uint64_t offset) const;
  error reposition(const FileChunkReference64x32&loc);
};

#endif
