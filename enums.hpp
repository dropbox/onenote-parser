#ifndef _ENUMS_HPP_
#define _ENUMS_HPP_

#include <cstdint>
#include <string>
#include "one_note_ptr.hpp"

enum class PropertyIDtype {
  ObjectID = 0x8,
    ArrayOfObjectIDs = 0x9,
    ObjectSpaceID = 0xa,
    ArrayOfObjectSpaceIDs = 0xb,
    ContextID = 0xc,
    ArrayofContextIDs = 0xd,
    };
const uint8_t ObjectSpaceManifestRootFND = 0x04;
const uint8_t ObjectSpaceManifestListReferenceFND = 0x08;
const uint8_t ObjectSpaceManifestListStartFND = 0x0c;
const uint8_t RevisionManifestListReferenceFND = 0x10;
const uint8_t RevisionManifestListStartFND = 0x14;
const uint8_t RevisionManifestStart4FND = 0x1b;
const uint8_t RevisionManifestEndFND = 0x1c;
const uint8_t RevisionManifestStart6FND = 0x1e;
const uint8_t RevisionManifestStart7FND = 0x1f;
const uint8_t GlobalIdTableStartFNDX = 0x21;
const uint8_t GlobalIdTableStart2FND = 0x22;
const uint8_t GlobalIdTableEntryFNDX = 0x24;
const uint8_t GlobalIdTableEntry2FNDX = 0x25;
const uint8_t GlobalIdTableEntry3FNDX = 0x26;
const uint8_t GlobalIdTableEndFNDX = 0x28;
namespace CanRevise {
  const uint8_t ObjectDeclarationWithRefCountFNDX = 0x2d;
  const uint8_t ObjectDeclarationWithRefCount2FNDX = 0x2e;
  const uint8_t ObjectRevisionWithRefCountFNDX = 0x041;
  const uint8_t ObjectRevisionWithRefCount2FNDX = 0x42;
  const uint8_t ObjectDeclaration2RefCountFND = 0x0A4;
  const uint8_t ObjectDeclaration2LargeRefCountFND = 0xA5;
  const uint8_t ReadOnlyObjectDeclaration2RefCountFND = 0xc4;
  const uint8_t ReadOnlyObjectDeclaration2LargeRefCountFND = 0xc5;
  const uint8_t ObjectDeclarationFileData3RefCountFND = 0x72;
  const uint8_t ObjectDeclarationFileData3LargeRefCountFND = 0x73;
}
const uint8_t RootObjectReference2FNDX = 0x59;
const uint8_t RootObjectReference3FND = 0x5a; // each root object must have a differe
const uint8_t RevisionRoleDeclarationFND = 0x5c;
const uint8_t RevisionRoleAndContextDeclarationFND = 0x5d;
const uint8_t ObjectDataEncryptionKeyV2FNDX = 0x7c;
const uint8_t ObjectInfoDependencyOverridesFND = 0x84;
const uint8_t DataSignatureGroupDefinitionFND = 0x8c;
const uint8_t FileDataStoreListReferenceFND = 0x90;
const uint8_t FileDataStoreObjectReferenceFND = 0x94;
const uint8_t ObjectGroupListReferenceFND = 0xb0;
const uint8_t ObjectGroupStartFND = 0xb4;
const uint8_t ObjectGroupEndFND = 0xb8;
const uint8_t HashedChunkDescriptor2FND = 0xc2;

const uint8_t ChunkTerminatorFND = 0xff;


struct uint24 {
  uint8_t val[3];
  uint8_t value() const {
    uint32_t le = val[2];
    le <<= 8;
    le |= val[1];
    le <<= 8;
    le |= val[0];
    return le;
  }
};

struct CompactID {
  uint8_t n;
  uint24 guidIndex; //only occupies 24 bits
  ExtendedGUID guid;
  operator ExtendedGUID() const {
    return guid;
  }
  std::string toString()const {
    return guid.toString();
  }
};
struct FileChunkReference64 {
  uint64_t stp;
  uint64_t cb;
  static constexpr FileChunkReference64 nil() {return {
      (uint64_t)-1, 0};}
  bool operator ==(const FileChunkReference64&other)const {
    return stp == other.stp && cb == other.cb;
  }
  bool operator !=(const FileChunkReference64&other)const {
    return stp != other.stp || cb != other.cb;
  }
};
/*
struct FileChunkReference64x32 {
  uint64_t stp;
  uint32_t cb;
  static constexpr FileChunkReference64x32 nil() {return {
      (uint64_t)-1, 0};}
};
*/
typedef FileChunkReference64 FileChunkReference32;
typedef FileChunkReference64 FileChunkReference64x32;
struct ObjectInfoDependencyOverrideData {
  uint32_t c8bitOverrides;
    uint32_t c32bitOverrides;
  uint32_t crc;
  std::vector<uint8_t> overrides1;
  std::vector<uint32_t> overrides2;
};
struct FileNode;
struct FileNodeList {
  std::vector<FileNode> children;
};
struct ObjectSpaceObjectStreamOfOIDsOSIDsOrContextIDs {
  uint32_t count; // 24 bits
  bool ExtendedStreamsPresent;
  bool OsidsStreamNotPresent;
  std::vector<CompactID> data;
};

struct PropertyID;
struct PropertyValue;

struct PropertySet {
  std::vector<PropertyValue> rgPridsData;
  void print(const OneNote*document)const;
};
struct ObjectSpaceObjectPropSet {
  ObjectSpaceObjectStreamOfOIDsOSIDsOrContextIDs oids;
  ObjectSpaceObjectStreamOfOIDsOSIDsOrContextIDs osids;
  ObjectSpaceObjectStreamOfOIDsOSIDsOrContextIDs contextIDs;
  PropertySet body;
  void print(const OneNote*document)const;
};

struct PropertyValue {
  PropertyID propertyID;
  // union of one of these things based on the type of the corresponding PropertyID
  uint64_t scalar; // holds a boolean value if type = 0x2, retrieved from header
  // either ObjectID or ObjectSpaceID or ContextID (single value in array)
  // either ArrayOfObjectIDs or ArrayOfObjectSpaceIDs or ArrayOfContextID
  std::vector<CompactID> compactIDs;
  PropertySet propertySet; // or used to house a single value
  FileChunkReference64 rawData; // FourBytesOfLengthFollowedByData
  void print(const OneNote*document) const;
};
struct FileDataStoreObject {
  GUID header;
  // uint64_t cbLength;implicit in the fileData FileChunkReference
  uint32_t reserved0;
  uint64_t reserved;
  FileChunkReference64 fileData; //points to raw data
  GUID footer; //MUST be {71FBA722-0F79-4A0B-BB13-899256426B24}.
};
struct JCID {
  uint16_t index;
  bool is_binary;
  bool is_property_set;
  bool is_graph_node;
  bool is_file_data;
  bool is_read_only;
  JCID() {
      memset(this, 0, sizeof(*this));
  }
  std::string toString()const;
  error read_uint32_t(uint32_t full_index) {
    index = full_index & 0xffff;
    is_binary = ((full_index >> 16) & 1) == 1;
    is_property_set = ((full_index >> 17) & 1) == 1;
    is_graph_node = ((full_index >> 18) & 1) == 1;
    is_file_data = ((full_index >> 19) & 1) == 1;
    is_read_only = ((full_index >> 20) & 1) == 1;
    if ((full_index >> 21) != 0) {
      return wrap(error::RESERVED_NONZERO);
    }
    return Ok;
  }
  error read_uint10_t(uint16_t jci) {
    memset(this, 0, sizeof(*this));
    index = jci;
    is_property_set = true;
    return Ok;
  }
};
struct ObjectDeclarationWithRefCountBody {
  CompactID oid;
  JCID jcid; // if this is a ObjectDeclarationWithRefCountBody, jci = 0x01
  bool fHasOidReferences;
  bool hasOsidReferences;
  // the obj is a GUID in the file_data_store_reference
  // for a ObjectDeclarationFileData3RefCountFND
  bool file_data_store_reference; 
};
struct FileNodeUnion {
  struct {
    uint32_t nInstanceIgnored;//ignored
  }RevisionManifestListStart;
  struct {
      ExtendedGUID ridDependent;
      uint64_t timeCreation; //ignored
      uint32_t revisionRole;
      uint16_t odcsDefault;// must be zero or 0x2 for RevisionManifestStart6
  }RevisionManifest;

  struct {
    uint8_t reserved; // must be zero
  } GlobalIdTableStartFNDX;
  struct {
    uint32_t index;
    GUID guid;
  }GlobalIdTableEntryFNDX;
  struct {
    uint32_t indexMapFrom;
    uint32_t indexMapTo;
  }GlobalIdTableEntry2FNDX;
  struct {
    uint32_t indexCopyFromStart;
    uint32_t entriesToCopy;
    uint32_t indexCopyToStart;
  } GlobalIdTableEntry3FNDX;
  struct {
    ObjectSpaceObjectPropSet ref;
    CompactID oid;
    uint8_t hasOidReferences;
    uint8_t hasOsidReferences;
    uint32_t cRef;
  } ObjectRevisionWithRefCountFNDX;
  struct {
    struct {
      uint32_t RootRole;
    } RootObjectReferenceBase;
    CompactID oidRoot; // only if theis is a RootObjectReference2FNDX
  } RootObjectReference;
  struct {
    uint32_t RevisionRole;
  }RevisionRoleDeclaration;
  struct { 
    // FIXME: populate this by parsing the FileNodeChunkReference?
    ObjectInfoDependencyOverrideData data;
  }ObjectInfoDependencyOverrides;
  struct {
    FileDataStoreObject ref;
  } FileDataStoreObjectReference;
  struct {
    ObjectSpaceObjectPropSet ObjectRef;
    ObjectDeclarationWithRefCountBody body;
    uint32_t cRef;
    struct {
      uint8_t md5[16];
    } read_only;
  } ObjectDeclarationWithRefCount;
 };
struct FileNode {
  uint16_t id; //10 bits
  uint16_t size; // 13 bits
  //base_type 0, ignore stp&cb 1, reference to data 2) reference to FileNodeList
  uint8_t base_type;
  // for ObjectSpaceManifestRoot
  // for ObjectSpaceManifestStart
  // for ObjectSpaceManifestList
  // for RevisionManifestListStart
  // ObjectGroupStartFND
  // ObjectGroupID for ObjectGroupListReferenceFND
  // RID for RevisionManifestStart4FND
  // DataSignatureGroup for RevisionManifestEndFND
  ExtendedGUID gosid; 
  
  // only present for RevisionManfiest7FND and RevisionRoleAndContextDeclaration
  ExtendedGUID gctxid; 
  GUID file_data_store_reference;
  FileChunkReference64 ref;
  PropertySet property_set;
  bool is_file_data;
  FileNodeList children; // for ObjectGroupListReference
  const char *fnd_name() const;
  void print(const OneNote*document)const;
  bool has_gctxid() const {
    return id == RevisionRoleAndContextDeclarationFND
      || id == RevisionManifestStart7FND;
  }
  ExtendedGUID get_gctxid() const {
    return gctxid;
  }
  bool hasChildren()const {
    return !children.children.empty();
  }
  const FileNodeList& Children()const {
    return children;
  }
  bool hasData()const;
  ObjectSpaceObjectPropSet data();
  GUID fileDataStoreReference(); // for future reference of this object
  bool isFile()const;
  FileChunkReference64 file()const;
  bool hasFileName()const;
  std::string fileName()const;
  GUID ifndf();
  FileNodeUnion sub_type;
  bool is_object_declaration() const;
  JCID object_declaration_jcid() const;
};

#define FOREACH_HEADER_MEMBER(CB)			   \
  CB(GUID, guidFileType)                           \
  CB(GUID, guidFile)                                   \
  CB(GUID, guidLegacyFileVersion)                   \
  CB(GUID, guidFileFormat)                           \
  CB(uint32_t, ffvLastCode)                           \
  CB(uint32_t, ffvNewestCode)                           \
  CB(uint32_t, ffvOldestCode)                           \
  CB(uint32_t, ffvOldestReader)                           \
  CB(FileChunkReference32, fcrLegacyFreeChunkList)  \
  CB(FileChunkReference32, fcrLegacyTransactionLog) \
  CB(uint32_t, cTransactionsInLog)                    \
  CB(uint32_t, cbLegacyExpectedFileLength)          \
  CB(uint64_t, rgbPlaceholder)				      \
  CB(FileChunkReference32, fcrLegacyFileNodeListRoot)	      \
  CB(uint32_t, cbLegacyFreeSpaceInFreeChunkList)		      \
  CB(uint8_t, ignoredZeroA)				      \
  CB(uint8_t, ignoredZeroB)				      \
  CB(uint8_t, ignoredZeroC)				      \
  CB(uint8_t, ignoredZeroD)				      \
  CB(GUID, guidAncestor)					      \
  CB(uint32_t, crcName)							\
  CB(FileChunkReference64x32, fcrHashedChunkList)          \
  CB(FileChunkReference64x32, fcrTransactionLog)		   \
  CB(FileChunkReference64x32, fcrFileNodeListRoot)	   \
  CB(FileChunkReference64x32, fcrFreeChunkList)		   \
  CB(uint64_t, cbExpectedFileLength)			   \
  CB(uint64_t, cbFreeSpaceInFreeChunkList)		   \
  CB(GUID, guidFileVersion)				   \
  CB(uint64_t, nFileVersionGeneration)			   \
  CB(GUID, guidDenyReadFileVersion)			   \
  CB(uint32_t, grfDebugLogFlags)				   \
  CB(FileChunkReference64x32, fcrDebugLogA)		   \
  CB(FileChunkReference64x32, fcrDebugLogB)		   \
  CB(uint32_t, buildInfoA)                               \
  CB(uint32_t, buildInfoB)				\
  CB(uint32_t, buildInfoC)				\
  CB(uint32_t, buildInfoD)

#define MAKE_STRUCT(TYP, VAR) TYP VAR;
struct OneNotePtr;
struct FileNodeListHeader{
  uint64_t uintMagic;
  uint32_t FileNodeListId;
  uint32_t nFragmentSequence;
};
struct Header {
  // first FileNodeListFragment .. if nil, hash chunk does not list
  FOREACH_HEADER_MEMBER(MAKE_STRUCT)
  uint8_t reserved[728];
  error deserialize(OneNotePtr* ptr);
};

#define DESERIALIZE(TYP, VAR) if ((err = ptr->deserialize_##TYP(&this->VAR)) != Ok) {return err;}


#endif
