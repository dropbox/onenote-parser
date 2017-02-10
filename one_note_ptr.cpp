#include <stdlib.h>
#include <stdio.h>
#include "one_note.hpp"
#include "enums.hpp"
#include "properties.hpp"
int indent_level = 0;
bool quiet_parse = true;
std::string get_indent() {
  std::string retval;
  for (int i= 0; i < indent_level; ++i) {
    retval+= "  ";
  }
  return retval;
}
IncIndentLevel::IncIndentLevel(){
  ++indent_level;
}
IncIndentLevel::~IncIndentLevel(){
  --indent_level;
}
DecIndentLevel::DecIndentLevel(){
  --indent_level;
}
DecIndentLevel::~DecIndentLevel(){
  ++indent_level;
}
#define PROPERTY_ID_SWITCH(NAME, VAL) case VAL: return #NAME;
const char * propertyID_to_string(PropertyID pid) {
  switch (pid.id) {
    FOREACH_PROPERTY(PROPERTY_ID_SWITCH)
  default:
      return "Unknown Property ID";
  }
}
std::string PropertyID::toString() const{
  const char * retval = "unknown";
  const char * unk = retval;
  FOREACH_PROPERTY(PROPERTYSTRING)
  char data[256];
  if (retval == unk) {
    sprintf(data, "id: 0x%x type 0x%x -- bit %d", id, (int)type, (int)inline_bool);
  } else {
    sprintf(data, "id: %s type 0x%x -- bit %d", retval, (int)type, (int)inline_bool);
  }
  return data;
}

std::string JCID::toString()const {
  JCID nil;
  if (memcmp(this, &nil, sizeof(nil)) == 0) {
      return "null";
  }
  std::string retval;
  switch(index) {
    FOR_EACH_JCID(SWITCH_RETVAL_APPEND)
  default:
      retval += "unknown";
  }
  if (is_file_data) retval += " file_data";
  if (is_property_set) retval += " property_set";
  if (is_read_only) retval += " read_only";
  return retval;
}
static const char * fnd_name(uint8_t fnd) {
  using namespace CanRevise;
  switch (fnd) {
  case ObjectSpaceManifestRootFND: return "ObjectSpaceManifestRootFND";
  case ObjectSpaceManifestListReferenceFND: return "ObjectSpaceManifestListReferenceFND";
  case ObjectSpaceManifestListStartFND: return "ObjectSpaceManifestListStartFND";
  case RevisionManifestListReferenceFND: return "RevisionManifestListReferenceFND";
  case RevisionManifestListStartFND: return "RevisionManifestListStartFND";
case RevisionManifestStart4FND: return "RevisionManifestStart4FND";
case RevisionManifestEndFND: return "RevisionManifestEndFND";
case RevisionManifestStart6FND: return "RevisionManifestStart6FND";
case RevisionManifestStart7FND: return "RevisionManifestStart7FND";
case GlobalIdTableStartFNDX: return "GlobalIdTableStartFNDX";
case GlobalIdTableStart2FND: return "GlobalIdTableStart2FND";
case GlobalIdTableEntryFNDX: return "GlobalIdTableEntryFNDX";
case GlobalIdTableEntry2FNDX: return "GlobalIdTableEntry2FNDX";
case GlobalIdTableEntry3FNDX: return "GlobalIdTableEntry3FNDX";
case GlobalIdTableEndFNDX: return "GlobalIdTableEndFNDX";
  case ObjectDeclarationWithRefCountFNDX: return "ObjectDeclarationWithRefCountFNDX";
  case ObjectDeclarationWithRefCount2FNDX: return "ObjectDeclarationWithRefCount2FNDX";
  case ObjectRevisionWithRefCountFNDX: return "ObjectRevisionWithRefCountFNDX";
  case ObjectRevisionWithRefCount2FNDX : return "ObjectRevisionWithRefCount2FNDX";
  case ObjectDeclaration2RefCountFND: return "ObjectDeclaration2RefCountFND";
 case ObjectDeclaration2LargeRefCountFND: return "ObjectDeclaration2LargeRefCountFND";
  case ReadOnlyObjectDeclaration2RefCountFND: return "ReadOnlyObjectDeclaration2RefCountFND";
  case ReadOnlyObjectDeclaration2LargeRefCountFND: return "ReadOnlyObjectDeclaration2LargeRefCountFND";
  case ObjectDeclarationFileData3RefCountFND: return "ObjectDeclarationFileData3RefCountFND";
  case ObjectDeclarationFileData3LargeRefCountFND: return "ObjectDeclarationFileData3LargeRefCountFND";
case RootObjectReference2FNDX: return "RootObjectReference2FNDX";
  case RootObjectReference3FND: return  "RootObjectReference3FND";
  case RevisionRoleDeclarationFND: return "RevisionRoleDeclarationFND";
case RevisionRoleAndContextDeclarationFND: return "RevisionRoleAndContextDeclarationFND";
case ObjectDataEncryptionKeyV2FNDX: return "ObjectDataEncryptionKeyV2FNDX";
case ObjectInfoDependencyOverridesFND: return "ObjectInfoDependencyOverridesFND";
case DataSignatureGroupDefinitionFND: return "DataSignatureGroupDefinitionFND";
case FileDataStoreListReferenceFND: return "FileDataStoreListReferenceFND";
case FileDataStoreObjectReferenceFND: return "FileDataStoreObjectReferenceFND";
case ObjectGroupListReferenceFND: return "ObjectGroupListReferenceFND";
case ObjectGroupStartFND: return "ObjectGroupStartFND";
case ObjectGroupEndFND: return "ObjectGroupEndFND";
case HashedChunkDescriptor2FND: return "HashedChunkDescriptor2FND";

case ChunkTerminatorFND: return "ChunkTerminatorFND";
 default: return "UnknownFND";
  }
}

const char * FileNode::fnd_name() const {
  return ::fnd_name(id);
}

OneNotePtr::OneNotePtr(const OneNote*doc) {
  document = doc;
  offset = 0;
  end = document->data.size();
}				       
error OneNotePtr::deserialize_bytes(uint8_t *data, uint64_t num_bytes) {
    if (offset+num_bytes > end) {
      return wrap(error::SEGV);
    }
    if (num_bytes != 0){
    std::memcpy(data, &document->data[offset], num_bytes);
    offset += num_bytes;
    }
    return Ok;
}

error OneNotePtr::deserialize_uint8_t(uint8_t *data) {
    if (offset >= end) {
      return wrap(error::SEGV);
    }
    *data = document->data[offset];
    offset += 1;
    return Ok;
  }

error OneNotePtr::deserialize_uint16_t(uint16_t *data) {
    if (offset + sizeof(*data) > end) {
      return wrap(error::SEGV);
    }
    std::memcpy(data, &document->data[offset], sizeof(*data));
    *data = htole16(*data);
    offset += sizeof(*data);
    return Ok;
  }

error OneNotePtr::deserialize_uint32_t(uint32_t *data) {
    if (offset + sizeof(*data) > end) {
      return wrap(error::SEGV);
    }
    std::memcpy(data, &document->data[offset], sizeof(*data));
    *data = htole32(*data);
    offset += sizeof(*data);
    return Ok;
  }

error OneNotePtr::deserialize_uint64_t(uint64_t *data) {
    if (offset + sizeof(*data) > end) {
      return wrap(error::SEGV);
    }
    std::memcpy(data, &document->data[offset], sizeof(*data));
    *data = htole64(*data);
    offset += sizeof(*data);
    return Ok;
  }

error OneNotePtr::deserialize_GUID(GUID *data) {
    return deserialize_bytes(data->guid, 16);
  }

error OneNotePtr::deserialize_ExtendedGUID(ExtendedGUID *data) {
    error errorA = deserialize_GUID(&data->guid);
    error errorB = deserialize_uint32_t(&data->n);
    if (errorA == Ok) {
      return errorB;
    }
    return errorA;
  }

error OneNotePtr::deserialize_CompactID(CompactID *data) {
    error errorA = deserialize_uint8_t(&data->n);
    error errorB = deserialize_bytes(&data->guidIndex.val[0], 3);
    if (errorA != Ok) {
      return errorA;
    }
    if (errorB != Ok) {
      return errorB;
    }
    data->guid = ExtendedGUID::nil();
    data->guid.n = data->n;
    uint32_t index = data->guidIndex.value();
    auto &global_id = document->revision_map[document->current_revision].global_id;
    std::map<uint32_t, GUID>::iterator where = global_id.find(index);
    if (where != global_id.end()) {
      data->guid.guid = where->second;
    } else {
      printf("%sFailed Mapping %d/0x%x -> ????\n", get_indent().c_str(),
	     data->guidIndex.value(), data->n);
      return wrap(error::COMPACT_ID_MISSING);
    }
    //    printf("%sMapping %d/0x%x -> %s\n", get_indent().c_str(),
    //	   data->guidIndex.value(), data->n, data->guid.toString().c_str());
    return Ok;
  }

error OneNotePtr::deserialize_FileChunkReference32(FileChunkReference32 *data) {
    uint32_t stp;
    error errorA = deserialize_uint32_t(&stp);
    uint32_t cb;
    error errorB = deserialize_uint32_t(&cb);
    data->stp = stp;
    data->cb = cb;
    if (errorA == Ok) {
      return errorB;
    }
    return errorA;
  }

error OneNotePtr::deserialize_FileChunkReference64(FileChunkReference64 *data) {
    error errorA = deserialize_uint64_t(&data->stp);
    error errorB = deserialize_uint64_t(&data->cb);
    if (errorA == Ok) {
      return errorB;
    }
    return errorA;
  }

error OneNotePtr::deserialize_FileChunkReference64x32(FileChunkReference64x32 *data) {
    error errorA = deserialize_uint64_t(&data->stp);
    uint32_t cb;
    error errorB = deserialize_uint32_t(&cb);
    data->cb = cb;
    if (errorA == Ok) {
      return errorB;
    }
    return errorA;
}

error OneNotePtr::deserialize_ObjectDeclarationWithRefCountBody(ObjectDeclarationWithRefCountBody *data) {
  error errorA = deserialize_CompactID(&data->oid);
  uint32_t jci_odcs_etc = 0;
  error errorB = deserialize_uint32_t(&jci_odcs_etc);
  uint16_t reserved = 0;
  error errorC = deserialize_uint16_t(&reserved);
  if (errorA != Ok) {
    return errorA;
  }
  if (errorB != Ok) {
    return errorB;
  }
  if (errorC != Ok) {
    return errorC;
  }
  error err = data->jcid.read_uint10_t((jci_odcs_etc & 0x3ff));
  if (err != Ok) {
    return err;
  }
  uint8_t must_be_zero = (jci_odcs_etc >> 10) & 0xf;
  uint8_t must_be_zeroA = ((jci_odcs_etc >> 14) &0x3);
  data->fHasOidReferences = ((jci_odcs_etc >> 16) &0x1) ? true : false;
  data->hasOsidReferences = ((jci_odcs_etc >> 17) &0x1) ? true : false;
  if (jci_odcs_etc >> 18) {
    return wrap(error::RESERVED_NONZERO);
  }
  if (reserved != 0 || must_be_zeroA != 0 || must_be_zero != 0) {
    return wrap(error::RESERVED_NONZERO);
  }
  return Ok;
}

error OneNotePtr::deserialize_ObjectDeclaration2Body(ObjectDeclarationWithRefCountBody *data) {
  error errorA = deserialize_CompactID(&data->oid);
  uint32_t jcid = 0;  
  error errorB = deserialize_uint32_t(&jcid);
  error err = data->jcid.read_uint32_t(jcid);
  if (err != Ok) {
    return err;
  }
  uint8_t has_refs = 0;
  error errorC = deserialize_uint8_t(&has_refs);
  if (errorA != Ok) {
    return errorA;
  }
  if (errorB != Ok) {
    return errorB;
  }
  if (errorC != Ok) {
    return errorC;
  }
  data->fHasOidReferences = (has_refs&0x1) ? true : false;
  data->hasOsidReferences = (has_refs&0x2) ? true : false;
  return Ok;
}

error OneNotePtr::deserialize_PropertyID(PropertyID *data) {
  uint32_t pid = 0;
  error err = deserialize_uint32_t(&pid);
  if (err != Ok) {
    return err;
  }
  data->id = (pid & 0x3ffffff);
  data->type = pid >> 26 & 0x1f;
  if (data->type == 0x2) {
    data->inline_bool = ((pid >> 31) & 0x1) ? true : false; // set the bool value from header
  } else {
    if ((pid >> 31) & 0x1) {
      return wrap(error::RESERVED_NONZERO);
    }
  }
  return Ok;
}

struct ObjectStreamCounters {
  size_t oids_count;
  size_t osids_count;
  size_t context_ids_count;
  ObjectStreamCounters() {
    oids_count = 0;
    osids_count = 0;
    context_ids_count = 0;
  }
};
const char * get_property_id_name(uint64_t id) {
    const char * retval = "unknown";
    FOREACH_PROPERTY(PROPERTYSTRING);
    return retval;
}

error OneNotePtr::deserialize_PropertyValueFromPropertyID(PropertyID propertyID,
							  const ObjectSpaceObjectPropSet &streams,
							  ObjectStreamCounters *counters,
							  PropertyValue *data) {
  data->propertyID = propertyID;
  uint8_t val8=0;
  uint16_t val16=0;
  uint32_t val32=0;
  uint64_t val64=0;
  error err =  Ok;
  if (!quiet_parse) {
    uint64_t id = propertyID.id;
    const char *retval = get_property_id_name(id);
    printf("\n%s<%s", get_indent().c_str(), retval);
  }
  IncIndentLevel inc;
  switch(propertyID.type) {
  case 0x1:
    if (!quiet_parse){printf(" [] ");}
    return Ok;
  case 0x2:
    if (!quiet_parse){
      printf(" PropertyID bool(%d)", (int)propertyID.inline_bool);
    }
    data->scalar = propertyID.inline_bool ? 1 : 0;
    return Ok;
  case 0x3:
      err = deserialize_uint8_t(&val8);
      data->scalar = val8;
      if (!quiet_parse) {
	printf(" PropertyID byte(%ld)", data->scalar);
      }
    break;
  case 0x4:
    err = deserialize_uint16_t(&val16);
    data->scalar = val16;
    if (!quiet_parse) {
      printf(" uint16 PropertyID short(%ld)", data->scalar);
    }
    break;
  case 0x5:
    err = deserialize_uint32_t(&val32);
    data->scalar = val32;
    if (!quiet_parse) {
      printf(" PropertyID int(%ld)", data->scalar);
    }
    break;
  case 0x6:
    err = deserialize_uint64_t(&val64);
    data->scalar = val64;
    if (!quiet_parse) {
      printf(" PropertyID long(%ld)", data->scalar);
    }
    break;
  case 0x7:
    err = deserialize_uint32_t(&val32);
    if (err != Ok) {
      return err;
    }
    if (!quiet_parse) {
    printf(" raw data: (%d)[", val32);
    }
    {
      data->rawData.stp = offset;
      data->rawData.cb = 0;
      if (offset + val32 > end) {
	data->rawData.cb = end - offset;
	offset = end;
	return wrap(error::SEGV);
      }
      data->rawData.cb = val32;
      offset += val32;
      {
	OneNotePtr content(*this);
	error err = content.reposition(data->rawData);
	if (err != Ok) {
	  return err;
	}
	if (!quiet_parse) {
	  content.dump_hex();
	}
      }
    }
    if (!quiet_parse) {
      printf("]");
    }
    break;
  case 0x9:
  case 0xb:
  case 0xd:
    err = deserialize_uint32_t(&val32);
    if (err != Ok) {
      return err;
    }
    // fallthrough
  case 0x8:
  case 0xa:
  case 0xc:
    if (propertyID.type == 0x8 || propertyID.type == 0xa || propertyID.type == 0xc) {
      val32 = 1;
    }
    {
      const std::vector<CompactID> *stream = &streams.contextIDs.data;
      const char * xtype = "contextID";
      size_t *s_count = &counters->context_ids_count;
      if (propertyID.type == 0x8 || propertyID.type == 0x9) {
	stream = &streams.oids.data;
	s_count = &counters->oids_count;
	xtype = "OIDs";
      }
      if (propertyID.type == 0xa || propertyID.type == 0xb) {
	stream = &streams.osids.data;
	s_count = &counters->osids_count;
	xtype = "OSIDS";
      }
      for (uint32_t i = 0; i < val32;++i) {
	uint32_t index = (*s_count)++;
	if (index < stream->size()) {
	  data->compactIDs.push_back((*stream)[index]);
	  if (!quiet_parse) {
	    printf(" %s[%s]",
		   xtype,
		   data->compactIDs.back().toString().c_str());
	  }
	} else {
	  return wrap(error::SEGV);
	}
      }
    }
    break;
  case 0x10:
    err = deserialize_uint32_t(&val32);
    if (err != Ok) {
      return err;
    }
    {
      PropertyID prop_id;
      err = deserialize_PropertyID(&prop_id);
      if (err != Ok) {
	return err;
      }
      if (!quiet_parse) {
	printf(" UnifiedSubPropertySet %d %s", val32, prop_id.toString().c_str());
      }
      data->propertySet.rgPridsData.resize(val32);
      for (uint32_t i = 0;i < val32; ++i) {
	err = deserialize_PropertyValueFromPropertyID(prop_id,
						      streams,
						      counters,
						      &data->propertySet.rgPridsData[i]);
	if (err != Ok) {
	  data->propertySet.rgPridsData.resize(i);
	  return err;
	}
      }
    }
    break;
  case 0x11:
    if (!quiet_parse) {
      printf(" SubPropertySet");
    }
    err = deserialize_PropertySet(streams, counters, &data->propertySet);
    break;
  default:
    return wrap(error::INVALID_CONSTANT);
  }
  if (!quiet_parse) {
    printf(">");
  }
  return err;
}
						       
error OneNotePtr::deserialize_PropertySet(const ObjectSpaceObjectPropSet &streams,
					  ObjectStreamCounters *counters,
					  PropertySet *data) {
  uint16_t count = 0;
  error err = deserialize_uint16_t(&count);
  if (err != Ok) {
    return err;
  }
  data->rgPridsData.resize(count);
  for (uint16_t i = 0; i < count; ++i) {
    err = deserialize_PropertyID(&data->rgPridsData[i].propertyID);
    if (err != Ok) {
      return err;
    }
    if (!quiet_parse) {
      printf("%sProperty %s\n", get_indent().c_str(), data->rgPridsData[i].propertyID.toString().c_str());
    }
  }
  if (!quiet_parse) {
    printf("%s%d elements in property set:", get_indent().c_str(), count);
  }
  for (uint16_t i = 0; i < count; ++i) {
    err = deserialize_PropertyValueFromPropertyID(data->rgPridsData[i].propertyID,
						  streams,
						  counters,
						  &data->rgPridsData[i]);
  }
  if (!quiet_parse) {
    printf("\n");
  }
  return Ok;
}
error OneNotePtr::postprocess_object_declaration_contents(FileNode *data,
							  const FileNodePtr& curPtr) {
    data->gosid = data->sub_type.ObjectDeclarationWithRefCount.body.oid.guid;
    document->guid_to_object[data->gosid] = curPtr;
    if (data->sub_type.ObjectDeclarationWithRefCount.body.jcid.is_property_set) {
      OneNotePtr objectSpacePropSetPtr(*this);
    error err = objectSpacePropSetPtr.reposition(data->ref);
    if (err != Ok) {
      return err;
    }
    err = objectSpacePropSetPtr.deserialize_ObjectSpaceObjectPropSet(&data->sub_type.ObjectDeclarationWithRefCount.ObjectRef);
    if (err != Ok) {
      return err;
    }
    ObjectStreamCounters stream_counters;
    err = objectSpacePropSetPtr.deserialize_PropertySet(data->sub_type.ObjectDeclarationWithRefCount.ObjectRef,
							&stream_counters,
							&data->property_set);
      if (err != Ok) {
	return err;
      }
    } else {
      if (!data->sub_type.ObjectDeclarationWithRefCount.body.jcid.is_file_data) {
	return wrap(error::INVALID_CONSTANT);
      }
      // this is FileData
      data->is_file_data = true;
      OneNotePtr content(*this);
      error err = content.reposition(data->ref);
      if (err != Ok) {
	return err;
      }
      if (!quiet_parse) {
	printf("%sRaw:", get_indent().c_str());
	content.dump_hex();
	printf("\n");
      }
    }
    return Ok;
}

error OneNotePtr::deserialize_ObjectInfoDependencyOverrideData(ObjectInfoDependencyOverrideData *data) {
  uint32_t num_8bit_overrides = 0;
  uint32_t num_32bit_overrides = 0;
  uint32_t crc = 0;
  error errorA = deserialize_uint32_t(&num_8bit_overrides);
  error errorB = deserialize_uint32_t(&num_32bit_overrides);
  error errorC = deserialize_uint32_t(&crc);
  if (errorA != Ok) {
    return errorA;
  }
  if (errorB != Ok) {
    return errorB;
  }
  if (errorC != Ok) {
    return errorC;
  }
  for (uint32_t i = 0; i < num_8bit_overrides; ++i) {
    uint8_t local = 0;
    errorA = deserialize_uint8_t(&local);
    if (errorA != Ok) {
      return errorA;
    }
    data->overrides1.push_back(local);
  }
  for (uint32_t i = 0; i < num_32bit_overrides; ++i) {
    uint32_t local = 0;
    errorA = deserialize_uint32_t(&local);
    if (errorA != Ok) {
      return errorA;
    }
    data->overrides2.push_back(local);
  }
  return Ok;
}
  error OneNotePtr::deserialize_FileNodeListHeader(FileNodeListHeader *data) {
    error errorA = deserialize_uint64_t(&data->uintMagic);
    error errorB = deserialize_uint32_t(&data->FileNodeListId);
    error errorC = deserialize_uint32_t(&data->nFragmentSequence);
    if (errorA == Ok) {
      if (errorB == Ok) {
	return errorC;
      }
      return errorB;
    }
    return errorA;
  }
error OneNotePtr::deserialize_FileNode(FileNode *data,
				       FileNodePtr *curPath) {
  const char * id_desc = "unknown";

  data->is_file_data = false;
  using namespace CanRevise;
  OneNotePtr backup(*this);
  uint32_t file_node_header;
  data->gosid = ExtendedGUID::nil();
  error err = deserialize_uint32_t(&file_node_header);
  if (err != Ok) {
    return err;
  }
  data->id = file_node_header & 0x3ff;
  if (data->id == 0) {
    return Ok;
  }
  if (!quiet_parse) {
    printf("%sStart Node %s (%d)\n", get_indent().c_str(), fnd_name(data->id), (int)data->id);
  }
  IncIndentLevel inc;
  data->size = (file_node_header >> 10) & 0x1fff;
  if (this->end < backup.offset + data->size) {
    // this node claims to have more space than the underlying chunk
    return wrap(error::SEGV);
  }
  // reset the size to only be in scope of this FileNode
  this->end = backup.offset + data->size;
  uint8_t stp_format = (file_node_header >> 23) & 0x3;
  uint8_t cb_format = (file_node_header >> 25) & 0x3;
  data->base_type = (file_node_header >> 27) & 0xf;
  uint8_t reserved = (file_node_header >> 31);
  data->ref = FileChunkReference64::nil();
  if (data->base_type == 1 || data->base_type == 2) {
    deserialize_VarFileChunkReference(&data->ref, stp_format, cb_format);
  } // otherwise ignore the data ref, since we're a type 0
  if (data->base_type == 1 && !(data->ref == FileChunkReference64::nil())) {
    OneNotePtr content(*this);
    err = content.reposition(data->ref);
    if (err != Ok) {
      return err;
    }
  }
  if (data->id == ObjectGroupStartFND) {
    id_desc = "oid(group)";
    err = deserialize_ExtendedGUID(&data->gosid);
    if (err != Ok) {
      return err;
    }
    //printf("%sgosid %s\n", get_indent().c_str(), data->gosid.toString().c_str());
  } else if (data->id == ObjectGroupEndFND) {
    // no data
  } else if (data->id == ObjectSpaceManifestRootFND
	     || data->id == ObjectSpaceManifestListStartFND) {
    if (data->id == ObjectSpaceManifestRootFND) {
      id_desc = "gosidRoot";
    }else {
      id_desc = "gosid";
    }
    err = deserialize_ExtendedGUID(&data->gosid);

    if (err != Ok) {
      return err;
    }
    //printf("%sgosid %s\n", get_indent().c_str(), data->gosid.toString().c_str());
  } else if (data->id == ObjectSpaceManifestListReferenceFND) {
    err = deserialize_ExtendedGUID(&data->gosid);
    id_desc = "gosid";
    if (err != Ok) {
      return err;
    }
    //printf("%sgosid %s\n", get_indent().c_str(),data->gosid.toString().c_str());
    //children parsed in generic base_type 2 parser
  }else if (data->id == RevisionManifestListStartFND) {
    err = deserialize_ExtendedGUID(&data->gosid);
    id_desc = "gosid";
    if (err != Ok) {
      return err;
    }
    FileNodePtr parentPath(*curPath);
    parentPath.offsets.pop_back();
    document->registerRevisionManifestList(data->gosid, parentPath);
					   
    //printf("%sgosid %s\n", get_indent().c_str(),data->gosid.toString().c_str());
    err = deserialize_uint32_t(&data->sub_type.
			       RevisionManifestListStart.nInstanceIgnored);
    if (err != Ok) {
      return err;
    }
  } else if (data->id == RevisionManifestStart4FND) {
    err = deserialize_ExtendedGUID(&data->gosid); // the rid
    id_desc = "rid";
    //printf("%sgosid %s\n", get_indent().c_str(), data->gosid.toString().c_str());
    if (err != Ok) {
      return err;
    }
    err = deserialize_ExtendedGUID(&data->sub_type.RevisionManifest.ridDependent); // the rid
    if (err != Ok) {
      return err;
    }
    if (!quiet_parse) {
      printf("%sdependent gosid %s\n", get_indent().c_str(), data->sub_type.RevisionManifest.ridDependent.toString().c_str());
    }
    err = deserialize_uint64_t(&data->sub_type.RevisionManifest.timeCreation);
    if (err != Ok) {
      return err;
    }
    err = deserialize_uint32_t(&data->sub_type.RevisionManifest.revisionRole);
    if (err != Ok) {
      return err;
    }
    err = deserialize_uint16_t(&data->sub_type.RevisionManifest.odcsDefault);
    if (err != Ok) {
      return err;
    }
    data->gctxid = ExtendedGUID::nil();
    document->registerRevisionManifest(*data);
  } else if (data->id == RevisionManifestStart6FND
	     || data->id == RevisionManifestStart7FND) {
    err = deserialize_ExtendedGUID(&data->gosid); // the rid
    id_desc = "rid";
    //printf("%sgosid %s\n", get_indent().c_str(), data->gosid.toString().c_str());
   if (err != Ok) {
      return err;
    }
    err = deserialize_ExtendedGUID(&data->sub_type.RevisionManifest.ridDependent); // the rid
    if (err != Ok) {
      return err;
    }
    if (!quiet_parse) {
      printf("%sdependent gosid %s\n", get_indent().c_str(), data->sub_type.RevisionManifest.ridDependent.toString().c_str());
    }
    err = deserialize_uint32_t(&data->sub_type.RevisionManifest.revisionRole);
    if (err != Ok) {
      return err;
    }
    err = deserialize_uint16_t(&data->sub_type.RevisionManifest.odcsDefault);
    if (err != Ok) {
      return err;
    }
    data->gctxid = ExtendedGUID::nil();
    if (data->id == RevisionManifestStart7FND) {
      err = deserialize_ExtendedGUID(&data->gctxid); // the rid
      if (err != Ok) {
	return err;
      }
    }
    document->registerAdditionalRevisionRole(data->gosid,
					     data->sub_type.RevisionManifest.revisionRole,
					     data->gctxid);
    document->registerRevisionManifest(*data);
  }  else if (data->id == GlobalIdTableStartFNDX) {
    err = deserialize_uint8_t(&data->sub_type.GlobalIdTableStartFNDX.reserved);
    if (err != Ok) {
      return err;
    }
  }else if (data->id == GlobalIdTableEntryFNDX) {
    err = deserialize_uint32_t(&data->sub_type.GlobalIdTableEntryFNDX.index);
    if (err != Ok) {
      return err;
    }
    err = deserialize_GUID(&data->sub_type.GlobalIdTableEntryFNDX.guid);
    if (err != Ok) {
      return err;
    }
    document->revision_map[document->current_revision].global_id[data->sub_type.GlobalIdTableEntryFNDX.index] =
      data->sub_type.GlobalIdTableEntryFNDX.guid;
  }else if (data->id == GlobalIdTableEntry2FNDX) {
    err = deserialize_uint32_t(&data->sub_type.GlobalIdTableEntry2FNDX.indexMapFrom);
    if (err != Ok) {
      return err;
    }
    err = deserialize_uint32_t(&data->sub_type.GlobalIdTableEntry2FNDX.indexMapTo);
    if (err != Ok) {
      return err;
    }
    ExtendedGUID dependent_revision =
      document->revision_map[document->current_revision].dependent;
    auto where = document->revision_map[dependent_revision].global_id.find(data->sub_type.GlobalIdTableEntry2FNDX.indexMapFrom);
    if (where == document->revision_map[dependent_revision].global_id.end()) {
      return wrap(error::COMPACT_ID_MISSING);
    }
    document->revision_map[document->current_revision].global_id[data->sub_type.GlobalIdTableEntry2FNDX.indexMapTo] = where->second;
  }else if (data->id == GlobalIdTableEntry3FNDX) {
    err = deserialize_uint32_t(&data->sub_type.GlobalIdTableEntry3FNDX.indexCopyFromStart);
    if (err != Ok) {
      return err;
    }
    err = deserialize_uint32_t(&data->sub_type.GlobalIdTableEntry3FNDX.entriesToCopy);
    if (err != Ok) {
      return err;
    }
    err = deserialize_uint32_t(&data->sub_type.GlobalIdTableEntry3FNDX.indexCopyToStart);
    if (err != Ok) {
      return err;
    }
    ExtendedGUID dependent_revision =
      document->revision_map[document->current_revision].dependent;
    for (uint32_t i = 0;i < data->sub_type.GlobalIdTableEntry3FNDX.entriesToCopy;
	 ++i) {
      auto &global_id = document->revision_map[dependent_revision].global_id;
      auto where = global_id.find(data->sub_type.GlobalIdTableEntry3FNDX.indexCopyFromStart + i);
      if (where == global_id.end()) {
	return wrap(error::COMPACT_ID_MISSING);
      }
      document->revision_map[document->current_revision].global_id[data->sub_type.GlobalIdTableEntry3FNDX.indexCopyToStart + i] = where->second;
    }
  } else if (data->id == ObjectRevisionWithRefCountFNDX || data->id == ObjectRevisionWithRefCount2FNDX ) {
    err = deserialize_CompactID(&data->sub_type.ObjectRevisionWithRefCountFNDX.oid); // the oid    
    if (err != Ok) {
      return err;
    }
    if (data->id == ObjectRevisionWithRefCountFNDX) {
      uint8_t ref = 0;
      err = deserialize_uint8_t(&ref);
      if (err != Ok) {
	return err;
      }
      data->sub_type.ObjectRevisionWithRefCountFNDX.hasOidReferences = ref & 1;
      data->sub_type.ObjectRevisionWithRefCountFNDX.hasOsidReferences = ref & 2;
      data->sub_type.ObjectRevisionWithRefCountFNDX.cRef = (ref >> 2);
    } else {
      uint32_t ref = 0;
      err = deserialize_uint32_t(&ref);
      if (err != Ok) {
	return err;
      }
      data->sub_type.ObjectRevisionWithRefCountFNDX.hasOidReferences = ref & 1;
      data->sub_type.ObjectRevisionWithRefCountFNDX.hasOsidReferences = ref & 2;
      if ((ref >> 2) != 0) {
	return wrap(error::RESERVED_NONZERO);
      }
      err = deserialize_uint32_t(&data->sub_type.ObjectRevisionWithRefCountFNDX.cRef);
      if (err != Ok) {
	return err;
      }
    }
  }else if (data->id == RootObjectReference2FNDX) {
    err = deserialize_CompactID(&data->sub_type.RootObjectReference.oidRoot);
      if (err != Ok) {
	return err;
      }
    id_desc = "oidRoot";
    data->gosid = data->sub_type.RootObjectReference.oidRoot.guid;
    err = deserialize_uint32_t(&data->sub_type.RootObjectReference.RootObjectReferenceBase.RootRole);
      if (err != Ok) {
	return err;
      }
      if (!quiet_parse) {
	printf("%sRoot role %d\n",get_indent().c_str(),
	       data->sub_type.RootObjectReference.RootObjectReferenceBase.RootRole);
      }
  }else if (data->id == RootObjectReference3FND) {
    id_desc = "oidRoot";
    err = deserialize_ExtendedGUID(&data->gosid);
      if (err != Ok) {
	return err;
      }
    err = deserialize_uint32_t(&data->sub_type.RootObjectReference.RootObjectReferenceBase.RootRole);
      if (err != Ok) {
	return err;
      }
      if (!quiet_parse) {
	printf("%sRoot role %d\n",get_indent().c_str(), data->sub_type.RootObjectReference.RootObjectReferenceBase.RootRole);
      }
  }else if (data->id == RevisionRoleDeclarationFND ||
	    data->id == RevisionRoleAndContextDeclarationFND) {
    err = deserialize_ExtendedGUID(&data->gosid);
      if (err != Ok) {
	return err;
      }
      err = deserialize_uint32_t(&data->sub_type.RevisionRoleDeclaration.RevisionRole);
      if (err != Ok) {
	return err;
      }
      if (data->id == RevisionRoleAndContextDeclarationFND) {
	err = deserialize_ExtendedGUID(&data->gctxid);
	if (err != Ok) {
	  return err;
	}
      }
      document->registerAdditionalRevisionRole(data->gosid,
					       data->sub_type.RevisionRoleDeclaration.RevisionRole,
					       data->gctxid);
      // FIXME: deal with ObjectDataEncryptionKey
  } else if (data->id == ObjectInfoDependencyOverridesFND) {
    OneNotePtr content(*this);
    if (data->ref != FileChunkReference64::nil()) {
      err = content.reposition(data->ref); // otherwise it's positioned right at this node
      if (err != Ok) {
	return err;
      }
    }
    err = content.deserialize_ObjectInfoDependencyOverrideData(&data->sub_type.ObjectInfoDependencyOverrides.data);
      if (err != Ok) {
	return err;
      }
  } else if (data->id == FileDataStoreListReferenceFND) {
    // already processed this
  } else if (data->id == FileDataStoreObjectReferenceFND) {
    OneNotePtr fileDataStorePtr(*this);
    err = fileDataStorePtr.reposition(data->ref);
    if (err != Ok) {
      return err;
    }
    err = fileDataStorePtr.deserialize_FileDataStoreObject(&data->sub_type.FileDataStoreObjectReference.ref);
    if (err != Ok) {
      return err;
    }
  } else if (data->id == ObjectDeclarationWithRefCountFNDX ||
	     data->id == ObjectDeclarationWithRefCount2FNDX ||
	     data->id == ObjectDeclaration2RefCountFND ||
	     data->id == ObjectDeclaration2LargeRefCountFND ||
	     data->id == ReadOnlyObjectDeclaration2RefCountFND ||
	     data->id == ReadOnlyObjectDeclaration2LargeRefCountFND) {
    data->sub_type.ObjectDeclarationWithRefCount.body.file_data_store_reference=false;
    if (data->id == ObjectDeclarationWithRefCountFNDX ||
	data->id == ObjectDeclarationWithRefCount2FNDX) {
      err = deserialize_ObjectDeclarationWithRefCountBody(&data->sub_type.ObjectDeclarationWithRefCount.body);
    } else { // one of the other 4 that use the ObjectDeclaration2Body
      err = deserialize_ObjectDeclaration2Body(&data->sub_type.ObjectDeclarationWithRefCount.body);
    }
    if (err != Ok) {
      return err;
    }
    if (data->id == ObjectDeclarationWithRefCountFNDX || data->id==ObjectDeclaration2RefCountFND  || data->id == ReadOnlyObjectDeclaration2RefCountFND) {
      uint8_t ref_cnt = 0;
      err = deserialize_uint8_t(&ref_cnt);
      data->sub_type.ObjectDeclarationWithRefCount.cRef = ref_cnt;
    } else {
      err = deserialize_uint32_t(&data->sub_type.ObjectDeclarationWithRefCount.cRef);
    }
    if (err != Ok) {
      return err;
    }
    if (data->id == ReadOnlyObjectDeclaration2RefCountFND ||
	data->id == ReadOnlyObjectDeclaration2LargeRefCountFND) {
      err = deserialize_bytes(data->sub_type.ObjectDeclarationWithRefCount.read_only.md5, 16);
      if (err != Ok) {
	return err;
      }
    }
    id_desc = "oid";
    err = postprocess_object_declaration_contents(data, *curPath);
    if (err != Ok) {
      return err;
    }
    if (!quiet_parse) {
      printf("%sRef Count JCID %s\n",get_indent().c_str(), data->sub_type.ObjectDeclarationWithRefCount.body.jcid.toString().c_str());
    }
  } else if (data->id == ObjectDeclarationFileData3RefCountFND ||
	     data->id == ObjectDeclarationFileData3LargeRefCountFND) {
    err = deserialize_CompactID(&data->sub_type.ObjectDeclarationWithRefCount.body.oid);
      if (err != Ok) {
	return err;
      }
      uint32_t jcid;
      err = deserialize_uint32_t(&jcid);
      if (err != Ok) {
	return err;
      }
      err = data->sub_type.ObjectDeclarationWithRefCount.body.jcid.read_uint32_t(jcid);
      if (err != Ok) {
	return err;
      }
      if (data->id == ObjectDeclarationFileData3RefCountFND) {
	uint8_t ref_count;
	err = deserialize_uint8_t(&ref_count);
	data->sub_type.ObjectDeclarationWithRefCount.cRef = ref_count;
      } else {
	err = deserialize_uint32_t(&data->sub_type.ObjectDeclarationWithRefCount.cRef);
      }
      if (err != Ok) {
	return err;
      }
      uint32_t cch = 0;
      err = deserialize_uint32_t(&cch);
      if (err != Ok) {
	return err;
      }
      if (cch > room_left()) { // not a valid guid
	return wrap(error::SEGV);
      }
      std::vector<uint16_t> u16data(cch);
      uint16_t guidPrefix[] = {'<','i','f','n','d','f','>'};
      size_t prefix_len = sizeof(guidPrefix) / sizeof(guidPrefix[0]);
      if (cch > prefix_len &&
	  memcmp(guidPrefix, &u16data[0], sizeof(guidPrefix)) == 0) {
	err = GUID::parse(&u16data[prefix_len],
			  cch - prefix_len,
			  &data->file_data_store_reference);	   
	if (err != Ok) {
	  return err;
	}
	data->sub_type.ObjectDeclarationWithRefCount.body.file_data_store_reference=true;
	//FIXME: why reference 0
	err = document->get_assoc_guid_to_ref(ExtendedGUID{data->file_data_store_reference,0}, &data->ref);
	if (err != Ok) {
	  if (!quiet_parse) {
	  fprintf(stderr, "err %d: Have not encountered this guid before %s\n", (int)err,
		  data->file_data_store_reference.toString().c_str());
	  }
	} else {
	  id_desc = "oid";
	  err = postprocess_object_declaration_contents(data, *curPath);
	  if (err != Ok) {
	    return err;
	  }
	}
      } else {
	// external reference
	std::string fmt;
	for (size_t i=  0; i < u16data.size(); ++i) {
	  fmt.push_back(u16data[i]);
	}
	fprintf(stderr, "Do not support external references %s\n", fmt.c_str());
      }
  }else if (data->id == ObjectGroupListReferenceFND) {
    id_desc = "object_group_id";
    err = deserialize_ExtendedGUID(&data->gosid); // the object group id
    if (err != Ok) {
      return err;
    }
    // the ref populates the FileNodeList children
  } else if (data->id == ObjectGroupStartFND) {
    id_desc = "object_group_id";
    err = deserialize_ExtendedGUID(&data->gosid); // the oid    
    if (err != Ok) {
      return err;
    }
  }else if (data->id == ObjectGroupEndFND) {
    // nothing to see here
  }else if (data->id == DataSignatureGroupDefinitionFND) {
    id_desc = "data_sig";
    err = deserialize_ExtendedGUID(&data->gosid); // the DataSignatureGroup
    if (err != Ok) {
      return err;
    }
  }else if (data->id == RevisionManifestListReferenceFND) {
    document->revision_map[document->current_revision].manifest_list.emplace_back(*curPath);
  }
  if (data->base_type == 2) {
    OneNotePtr subList(*this);
    err = subList.reposition(data->ref);
    if (err != Ok) {
      return err;
    }
    err = subList.deserialize_FileNodeList(&data->children, curPath);
    if (err != Ok) {
      fprintf(stderr, "%sFailed %s (%d): %d\n", get_indent().c_str(), fnd_name(data->id), (int)data->id, (int) err);
      return err; // failed
    }
  }
  
  offset = backup.offset + data->size;
  end = backup.end;
  
  if (reserved != 1) {
    return wrap(error::RESERVED_NONZERO);
  }
  if (data->base_type == 1 && !(data->ref == FileChunkReference64::nil())) {
    document->set_assoc_guid_to_ref(data->gosid, data->ref);
    OneNotePtr content(*this);
    error err = content.reposition(data->ref);
    if (err != Ok) {
      return err;
    }
    /*
    printf("%sRaw Data for Revision %s GUID(%s), type %x\n", get_indent().c_str(), document->current_revision.toString().c_str(), data->gosid.toString().c_str(), data->id);
    content.dump_hex();
    printf("\n%sEnd Data for Revision %s\n", get_indent().c_str(), document->current_revision.toString().c_str());
    */
    if (data->has_gctxid()) {
      if (!quiet_parse) {
	printf("%sgctxid %s\n", get_indent().c_str(), data->gctxid.toString().c_str());
      }
    }
  }
  DecIndentLevel dec;
  if (data->gosid == ExtendedGUID::nil()) {
    if (!quiet_parse) {
      printf("%sEnd Node %s (%d)\n", get_indent().c_str(), fnd_name(data->id), (int)data->id);
    }
  }else {
    if (!quiet_parse) {
      printf("%sEnd Node %s (%d) %s:[%s]\n", get_indent().c_str(), fnd_name(data->id), (int)data->id, id_desc, data->gosid.toString().c_str());
    }
  }
  return Ok;
}
error OneNotePtr::deserialize_FileDataStoreObject(FileDataStoreObject*data) {
  error errA = deserialize_GUID(&data->header);
  uint64_t len = 0;
  error errB = deserialize_uint64_t(&len);
  error errC = deserialize_uint32_t(&data->reserved0);
  error errD = deserialize_uint64_t(&data->reserved);
  if (errA != Ok) {
    return errA;
  }
  if (errB != Ok) {
    return errB;
  }
  if (errC != Ok) {
    return errC;
  }
  if (errD != Ok) {
    return errD;
  }
  if (offset + len + 16 > end) {
    return wrap(error::SEGV);
  }
  if (data->reserved0 || data->reserved) {
    return wrap(error::RESERVED_NONZERO);
  }
  data->fileData.stp = offset;
  data->fileData.cb = len;
  offset += len;
  while (offset & 0x7) {
    ++offset;
  }
  error errE = deserialize_GUID(&data->footer);
  if (errE != Ok) {
    return errE;
  }
  GUID desired_footer;
  int i = 0;
  desired_footer.guid[i++] = 0x22;
  desired_footer.guid[i++] = 0xa7;
  desired_footer.guid[i++] = 0xfb;
  desired_footer.guid[i++] = 0x71;

  desired_footer.guid[i++] = 0x79;
  desired_footer.guid[i++] = 0x0f;

  desired_footer.guid[i++] = 0x0b;
  desired_footer.guid[i++] = 0x4a;
  
  desired_footer.guid[i++] = 0xbb;
  desired_footer.guid[i++] = 0x13;
  desired_footer.guid[i++] = 0x89;
  desired_footer.guid[i++] = 0x92;
  desired_footer.guid[i++] = 0x56;
  desired_footer.guid[i++] = 0x42;
  desired_footer.guid[i++] = 0x6b;
  desired_footer.guid[i++] = 0x24;
  if (data->footer != desired_footer) {
    return wrap(error::INVALID_CONSTANT);
  }
  return Ok;
}
error OneNotePtr::deserialize_ObjectSpaceObjectStreamOfOIDsOSIDsOrContextIDs(ObjectSpaceObjectStreamOfOIDsOSIDsOrContextIDs *data) {
  uint32_t header;
  error err = deserialize_uint32_t(&header);
  if (err != Ok) {
    return err;
  }
  data->count = header & 0xffffff;
  data->OsidsStreamNotPresent = ((header >> 31) & 0x1) ? true : false;
  data->ExtendedStreamsPresent = ((header >> 30) & 0x1) ? true : false;
  if (!quiet_parse) {
    printf("%sDeserialized Stream Header count: %d OsidsNotPresent %d Extended %d\n",
	   get_indent().c_str(),data->count, (int)data->OsidsStreamNotPresent,
	 (int)data->ExtendedStreamsPresent);
  }
  for (uint32_t i = 0; i < data->count; ++i) {
    CompactID cid;
    err = deserialize_CompactID(&cid);
    if (err != Ok) {
      return err;
    }
    data->data.push_back(cid);
  }
  return Ok;
}
error OneNotePtr::deserialize_ObjectSpaceObjectPropSet(ObjectSpaceObjectPropSet*data) {
  data->osids.ExtendedStreamsPresent = false;
  data->osids.OsidsStreamNotPresent = true;
  data->contextIDs.ExtendedStreamsPresent = false;
  data->contextIDs.OsidsStreamNotPresent = false;
  //uint64_t cur_offset = offset;
  //printf("starting deserialization %lx(%lx) / %lx\n", offset, offset - cur_offset, end);
  error err = deserialize_ObjectSpaceObjectStreamOfOIDsOSIDsOrContextIDs(&data->oids);
  if (err != Ok) {
    return err;
  }
  //printf("mid deserialization %lx(%lx) / %lx\n", offset, offset - cur_offset, end);
  if (!data->oids.OsidsStreamNotPresent) {
    err = deserialize_ObjectSpaceObjectStreamOfOIDsOSIDsOrContextIDs(&data->osids);
    if (err != Ok) {
      return err;
    }
  }
  //printf("lat deserialization %lx(%lx) / %lx\n", offset, offset - cur_offset, end);
  if (data->oids.ExtendedStreamsPresent) {
    err = deserialize_ObjectSpaceObjectStreamOfOIDsOSIDsOrContextIDs(&data->contextIDs);
    if (err != Ok) {
      return err;
    }
  }
  
  //  printf("body deserialization %lx(%lx) / %lx\n", offset, offset - cur_offset, end);
  //  dump_hex();
  return Ok;
}
error OneNotePtr::deserialize_VarFileChunkReference(FileChunkReference64 *data,
						    uint32_t stp_fmt,
						    uint32_t cb_fmt) {
  error err = Ok;
  uint8_t local8 = 0;
  uint16_t local16 = 0;
  uint32_t local32 = 0;
  switch(stp_fmt) {
  case 0: // 8 bytes, uncompressed
    err = deserialize_uint64_t(&data->stp);
      break;
  case 1:
    err = deserialize_uint32_t(&local32);
    data->stp = local32;
    break;
  case 2:
    err = deserialize_uint16_t(&local16);
    data->stp = local16;
    data->stp <<= 3;
    break;
  case 3:
    err = deserialize_uint32_t(&local32);
    data->stp = local32;
    data->stp <<= 3;
    break;
  default:
    return wrap(error::UNKNOWN_ENUM);
  }
  if (err != Ok) {
    return err;
  }
  switch(cb_fmt) {
  case 0: // 4 bytes, uncompressed
    err = deserialize_uint32_t(&local32);
    data->cb = local32;
    break;
  case 1: // 8 bytes, uncompressed;
    err = deserialize_uint64_t(&data->cb);
      break;
  case 2: // 1 byte, compressed
    err = deserialize_uint8_t(&local8);
    data->cb = local8;
    data->cb <<= 3;
    break;
  case 3: // 2 bytes, compressed
    err = deserialize_uint16_t(&local16);
    data->cb = local16;
    data->cb <<= 3;
    break;
  default:
    return wrap(error::UNKNOWN_ENUM);
  }
  return err;
  
}
error internal_deserialize_FileNodeList(OneNotePtr *ptr,
					FileNodeList *data,
					FileNodePtr *curPath) {
  OneNotePtr localPtr(ptr->document);
  FileNodePtrBackPush bp(*curPath); // this starts a complete list...
  // the curPath is valid for all the fragments
  while (true) {
    FileChunkReference64x32 next = FileChunkReference64x32::nil();
    error err = ptr->deserialize_FileNodeListFragment(data, &next, curPath);
    if (err != Ok) {
      return err;
    }
    if (next == FileChunkReference64x32::nil()) {
      break;
    }
    err = localPtr.reposition(next);
    if (err != Ok) {
      return err;
    }
    ptr = &localPtr;
  }
  return Ok;
}
error OneNotePtr::reposition(const FileChunkReference64x32&loc) {
  if (loc.stp >= document->data.size() && loc.cb != 0) {
    return wrap(error::SEGV);
  }
  if (loc.stp + loc.cb > document->data.size()) {
    return wrap(error::SEGV);
  }
  offset = loc.stp;
  end = offset + loc.cb;
  return Ok;
}
error OneNotePtr::deserialize_FileNodeList(FileNodeList *data,
					   FileNodePtr *curPath) {
  return internal_deserialize_FileNodeList(this, data, curPath);
}

const uint8_t* OneNotePtr::data()const {
  return &document->data[offset]; 
}
void OneNotePtr::dump_hex() const {
  uint64_t count = 0;
  for (uint64_t i = offset; i != end; ++i) {
    if (document->data[i] < 128) {
      count += 1;
    }
  }
  if (count != end - offset) {
    for (uint64_t i = offset; i != end; ++i) {
      printf("%02x ", document->data[i]);
    }
  }
  if (count > 3*(end - offset) / 4) {
    if (count != end-offset) {
      printf(" == ");
    }
    for (uint64_t i = offset; i != end; ++i) {
      if (document->data[i] >= 10 && document->data[i] < 128) {
	printf("%c", document->data[i]);
      }
    }
  }
  
}
class CheckedFileNodePushBack {
  bool committed;
  std::vector<FileNode>*vec;
public:
  
  CheckedFileNodePushBack(std::vector<FileNode>*v) {
    committed = true;
    vec = v;
    vec->emplace_back();
    committed = false;
  }
  void commit() {
    committed = true;
  }
  ~CheckedFileNodePushBack(){
    if (!committed) {
      vec->pop_back();
    }
  }
};
// FIXME: if transaction log is partial we need to stop partway
error OneNotePtr::deserialize_FileNodeListFragment(FileNodeList *data,
						   FileChunkReference64x32 *next,
						   FileNodePtr *curPath) {
  FileNodeListHeader hdr;
  error err = deserialize_FileNodeListHeader(&hdr);
  if (err != Ok) {
    return err;
  }

  bool terminated = false;
  while (offset + 24 <= end) { // while there are at least 24 bytes free
    // 24 = sizeof(nextFragment) [12 bytes] + sizeof(footer) [8 bytes]
    // + 4 bytes for the FileNode header
    CheckedFileNodePushBack push_back(&data->children);
    err = deserialize_FileNode(&data->children.back(), curPath);
    if (err != Ok) {
      return err;
    }
    if (data->children.back().id == ChunkTerminatorFND ||
	data->children.back().id == 0) {
      terminated = true;
      break;
    }
    push_back.commit();
    //printf("Pushing back %02x\n", node.id);
    assert(curPath->dereference(document) == &data->children.back());
    ++curPath->offsets.back();
    
  }
  if (offset > end - 20) {
    return wrap(error::SEGV);
  }
  offset = end - 20;
  err = deserialize_FileChunkReference64x32(next);
  if (err == Ok && terminated) {
    // make sure that next is "valid" (whatever that means)
  }
  if (err != Ok) {
    return err;
  }
  uint64_t footer = 0;
  err = deserialize_uint64_t(&footer);
  if (err != Ok) {
    return err;
  }
  if (footer != 0x8BC215C38233BA4B) {
    return wrap(error::INVALID_CONSTANT);
  }
  return err;
}


const uint8_t& OneNotePtr::operator[](uint64_t offset) const {
  //checked
  return document->data.at(this->offset + offset);
}
FileNode*FileNodePtr::dereference(OneNote*document) const {
  if (offsets.empty()) {
    return NULL;
  }
  FileNode *cur = &document->root.children.at(offsets[0]);
  for (size_t i = 1, ie = offsets.size(); i < ie; ++i) {
    cur = &cur->children.children.at(offsets[i]);
  }
  return cur;
}
const FileNode*FileNodePtr::dereference(const OneNote*document)const {
  return dereference(const_cast<OneNote*>(document));
}
