#include <iostream>
#include <locale>
#include <codecvt>
#include "properties.hpp"
#include "one_note.hpp"
#include "one_note_json.hpp"
#include "one_note_json_jansson.hpp"

extern "C" {
#include <b64/cencode.h>
}

std::string get_json(const OneNote*document,
		     const RoleAndContext *revision_role,
		     bool only_latest) {
  json_t *representation = get_json_representation(document,
						   revision_role,
						   only_latest);
  char *data = json_dumps(representation, JSON_ENSURE_ASCII | JSON_INDENT(2));
  json_decref(representation);
  std::string retval(data);
  free(data);
  return retval;
}
json_t* property_set_to_json(const OneNote*thus, const PropertySet&fn);
json_t *file_node_to_json(const OneNote*thus, const FileNode&fn);
extern const char * get_property_id_name(uint64_t id);
json_t* property_to_json(const OneNote*thus, const PropertyValue&data) {
  json_t *retval = json_object();
  json_t *file_nodes = NULL;
  json_object_set_new(retval, "id", json_string(get_property_id_name(data.propertyID.id)));
  if (data.propertyID.type && data.propertyID.type <= 6) {
    json_object_set_new(retval, "scalar", json_integer(data.scalar));
  } else if (data.propertyID.type == 7) {
    OneNotePtr content(thus);
    error err = content.reposition(data.rawData);
    assert(err == Ok && "Already parsed, already filtered for errors");
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> conversion;
    std::string mbs;
    json_t *raw = NULL;
    bool is_binary = (data.propertyID.id == (RgOutlineIndentDistance&0xffff)) ||
      (data.propertyID.id == (NotebookManagementEntityGuid&0xffff));
    if ((content.size() & 1) == 0 &&
	data.propertyID.id != (TextExtendedAscii&0xffff) &&
	is_binary == false) {
      mbs = conversion.to_bytes(std::u16string((const char16_t*)content.data(), content.size() / 2));
      raw = json_stringn((const char*)mbs.data(), mbs.size());
    } else if (is_binary == false) {
      raw = json_stringn((const char*)content.data(), content.size());
    }
    if (raw) {
      json_object_set_new(retval, "data", raw);
    } else {
      base64_encodestate state;
      base64_init_encodestate(&state);
      std::vector<char> b64data((content.size() + 3) * 4 / 3);
      char *cur = &b64data[0];
      for (size_t i = 0; i < content.size(); i += 16384) {
	cur += base64_encode_block((const char*)content.data() + i,
				   std::min((size_t)(content.size() - i), (size_t)16384),
				   cur,
				   &state);
      }
      cur += base64_encode_blockend(cur, &state);
      json_object_set_new(retval, "base64", json_stringn(&b64data[0], cur - &b64data[0]));
    }    
  } else if (data.propertyID.type == 0x9 || data.propertyID.type == 0x8 ||
	     data.propertyID.type == 0xb || data.propertyID.type == 0xc ||
	     data.propertyID.type == 0xa || data.propertyID.type == 0xd) {
    /*
      const char * xtype = "contextID";
      if (data.propertyID.type == 0x8 || data.propertyID.type == 0x9) {
	xtype = "OIDs";
      }
      if (data.propertyID.type == 0xa || data.propertyID.type == 0xb) {
	xtype = "OSIDS";
      }
    */
      for (const auto &compactID : data.compactIDs) {
	auto where = thus->guid_to_object.find(compactID.guid);
	if (where != thus->guid_to_object.end()) {
	  const FileNode *fn = where->second.dereference(thus);
	  if (fn) {
	    json_t *fn_json = file_node_to_json(thus, *fn);
	    if (fn_json) {
	      if (!file_nodes) {
		file_nodes = json_array();
		json_object_set_new(retval, "sub_nodes", file_nodes);
	      }
	      json_array_append_new(file_nodes, fn_json);
	    }
	  }
	}
      }
  } else if (data.propertyID.type == 0x10 || data.propertyID.type == 0x11){
      json_t *sub_property = property_set_to_json(thus,
						 data.propertySet);
      if (sub_property) {
	json_object_set_new(retval, "property_sub_set", sub_property);
      }
  }
  return retval;
}
json_t* property_set_to_json(const OneNote*thus, const PropertySet&fn) {
  json_t *retval = NULL;
  for (const auto &child : fn.rgPridsData) {
    json_t *child_json = property_to_json(thus, child);
    if (child_json) {
      if (!retval) {
	retval = json_array();
      }
      json_array_append_new(retval, child_json);
    }
  }
  return retval;
}
json_t* property_set_to_json(const OneNote*thus, const ObjectSpaceObjectPropSet&fn) {
  return property_set_to_json(thus, fn.body);
}

json_t *file_node_to_json(const OneNote*thus, const FileNode&fn) {
  json_t *retval = NULL;
  json_t *children = NULL;
  for (const auto&child : fn.children.children) {
    json_t *child_json = file_node_to_json(thus, child);
    if (child_json) {
      if (!children) {
	children = json_array();
      }
      json_array_append_new(children, child_json);
    }
  }
  if (children) {
    if (!retval) {
      retval = json_object();
    }
    json_object_set_new(retval, "children", children);
  }
  json_t *properties = property_set_to_json(thus, fn.property_set);
  if (properties) {
    if (!retval) {
      retval = json_object();
    }
    json_object_set_new(retval, "property_set", properties);
  }
  return retval;
}

class JsonWalker : public Walker {
public:
  json_t *json;
  JsonWalker() {
    json = json_array();
  }
  void operator() (const OneNote*thus, const FileNodePtr&ptr) {
    auto fn = ptr.dereference(thus);
    if (fn) {
      (*this)(thus, *fn);
    }
  }
  void operator() (const OneNote*thus, const FileNode&fn) {
    json_t *data = file_node_to_json(thus, fn);
    json_array_append_new(json, data);
  }
};
json_t *get_json_representation(const OneNote*document,
			       const RoleAndContext*revision_role,
			       bool only_latest) {
  JsonWalker walker;
  document->walk_revisions(walker, revision_role, only_latest);
  json_t *retval = walker.json;
  walker.json = NULL;
  return retval;
}

