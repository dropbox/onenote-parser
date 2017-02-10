#include <iostream>
#include <locale>
#include <codecvt>
#include "properties.hpp"
#include "one_note.hpp"
#include "one_note_json.hpp"
#include "one_note_json_jansson.hpp"
#include <set>

#define INTERNAL_ID_NAME "#pname"
#define INTERNAL_SCALAR "#scalar"
#define INTERNAL_DATA "#data"
#define INTERNAL_BASE64 "#base64"
#define INTERNAL_JCID "#jcid"

const std::set<std::string> _internal_gen_id_blacklist() {
    std::set<std::string> ret;
    ret.insert("AuthorMostRecent");
    ret.insert("AuthorOriginal");
    ret.insert("Author");
    ret.insert("CreationTimeStamp");
    ret.insert("LastModifiedTime");
    ret.insert("TopologyCreationTimeStamp");
    ret.insert("NotebookManagementEntityGUid");
    ret.insert("unknown");
    return ret;
}
const std::set<std::string>& id_blacklist() {
    static const std::set<std::string> retval = _internal_gen_id_blacklist();
    return retval;
}
extern "C" {
#include <b64/cencode.h>
}
json_t* property_set_to_json_html(const OneNote*thus, const PropertySet&fn, const JCID &jcid);
json_t *file_node_to_json_html(const OneNote*thus, const FileNode&fn);

class JsonHtmlWalker : public Walker {
public:
  json_t *json;
  JsonHtmlWalker() {
    json = json_array();
  }
  void operator() (const OneNote*thus, const FileNodePtr&ptr) {
    auto fn = ptr.dereference(thus);
    if (fn) {
      (*this)(thus, *fn);
    }
  }
  void operator() (const OneNote*thus, const FileNode&fn) {
    json_t *data = file_node_to_json_html(thus, fn);
    json_array_append_new(json, data);
  }
};
json_t *get_json_html_representation(const OneNote*document,
                               const RoleAndContext*revision_role,
                               bool only_latest) {
  JsonHtmlWalker walker;
  document->walk_revisions(walker, revision_role, only_latest);
  json_t *retval = walker.json;
  walker.json = NULL;
  return retval;
}

json_t *flatten_arrays(json_t *obj) {
    if (json_is_array(obj)) {
        size_t index = 0;
        json_t *value = NULL;
        json_t *first_array = NULL;
        json_array_foreach(obj, index, value) {
            if (!json_is_array(value)) {
                return obj; // no modification allowed
            } else {
                if (first_array == NULL) {
                    first_array = value;
                }
            }
        }
        if (!first_array) {
            return obj; // no children
        }
        json_array_foreach(obj, index, value) {
            if (value != first_array) {
                int code = json_array_extend(first_array, value);
                assert(code == 0 && "Array extend must succeed");
            }
        }
        json_incref(first_array);
        json_decref(obj); // this should free all children
        return first_array;
    } else {
        return obj;
    }
}

std::string one_note_to_css_style_name(const std::string&input) {
    std::string retval;
    for (auto s : input) {
        if (retval.length() == 0) {
            retval.push_back(tolower(s));
        }else {
            if (tolower(s) != s) {
                retval.push_back('-');
                retval.push_back(tolower(s));
            } else {
                retval.push_back(s);
            }
        }
    }
    return retval;
}

json_t* styleRelevantData(const std::string &name, json_t *obj) {
    json_t * data= json_object_get(obj, INTERNAL_DATA);
    if (data) {
        return data;
    }
    json_t * b64data= json_object_get(obj, INTERNAL_BASE64);
    if (b64data) {
        return b64data;
    }
    json_t * scalar= json_object_get(obj, INTERNAL_SCALAR);
    return scalar;
}

void convert_style_array_to_css(json_t *style, json_t *output) {
    json_t *children_array = json_object_get(style, "children");
    if (!children_array) {
        return;
    }
    assert(json_is_array(children_array));
    size_t index;
    json_t *value;
    json_array_foreach(children_array, index, value) {
        const char * styleName = json_string_value(json_object_get(value, INTERNAL_ID_NAME));
        std::string cssStyleName = one_note_to_css_style_name(styleName);
        json_t *relevant_data = styleRelevantData(cssStyleName, value);
        if (relevant_data) {
            int retval = json_object_set(output, cssStyleName.c_str(), relevant_data);
            assert(retval == 0);
        }
    }
    json_object_del(style, "children");
    json_object_del(style, "id");
}

void flatten_out_style(json_t *object) {
    json_t *children_array = json_object_get(object, "children");
    if (!children_array) {
        return;
    }
    assert(json_is_array(children_array));
    size_t index;
    json_t *value;
    std::vector<size_t> mfd;
    std::vector<json_t*> to_copy;
    std::vector<std::pair<json_t*, json_t*> > current_run;
    json_array_foreach(children_array, index, value) {
        if (json_object_get(value, INTERNAL_ID_NAME)) {
            const char * id = json_string_value(json_object_get(value, INTERNAL_ID_NAME));
            if (strcmp(id, "ListNodes") == 0) {
                json_t *style = json_object();
                convert_style_array_to_css(value, style);
                json_object_set(value, "style", style);
            }else if (strcmp(id, "ParagraphStyle") == 0) {
                mfd.push_back(index);
                convert_style_array_to_css(value, value);
                if (current_run.empty()) {
                    to_copy.push_back(value);
                } else {
                    json_object_set(current_run.back().first, "style", value);
                }
            } else if (!current_run.empty()) {
                json_array_append(current_run.back().second,
                                  value);
                mfd.push_back(index);
            }
            if (strstr(id, "TextRunFormatting")) {
                json_t *span_style = json_object();
                convert_style_array_to_css(value, span_style);
                json_object_set_new(value, "id", json_string("span"));
                json_object_set(value, "style", span_style); // should incref
                json_t *current_run_array = json_array();
                json_object_set_new(value, "children", current_run_array);
                current_run.push_back(std::pair<json_t*, json_t*>(value, current_run_array));
            }
        }
    }
    if (to_copy.size() > 1) {
        for (size_t i = 1 ; i < to_copy.size(); ++i) {
            json_object_update(to_copy[0], to_copy[i]);
        }
    }
    if (!to_copy.empty()) {
        json_object_set(object, "style", to_copy[0]); // should incref
    }
    for (auto i = mfd.rbegin(),ie=mfd.rend();i != ie; ++i) {
        json_array_remove(children_array, *i);
    }
}
bool invisible_node(json_t *children) {
  size_t index = 0;
  json_t *value = NULL;
  json_array_foreach(children, index, value) {
    json_t *prop = json_object_get(value, "#pname");
    if (strcmp(json_string_value(prop), "IsBoilerText") == 0) {
      return true;
    }
  }
  return false;
}
extern const char * get_property_id_name(uint64_t id);
json_t* property_to_json_html(const OneNote*thus,
                              const PropertyValue&data,
                              const JCID &jcid) {
  const char* id_str = get_property_id_name(data.propertyID.id);
  if (id_blacklist().find(id_str) != id_blacklist().end()) {
      return NULL;
  }
  if (data.propertyID.type && data.propertyID.type <= 6 && data.scalar == 0) {
      return NULL; // lets pretend we're in golang and 0 means nop
  }
  if (data.propertyID.type && data.propertyID.type <= 6 && data.scalar == 1033 &&
      (strstr(id_str, "LanguageID") || strstr(id_str, "LangID") )) {
      return NULL; // this is english--fine to ignore as default.
  }
  json_t *retval = json_object();
  json_t *file_nodes = NULL;
  json_object_set_new(retval, INTERNAL_ID_NAME, json_string(id_str));
  json_object_set_new(retval, INTERNAL_JCID, json_string(jcid.toString().c_str()));
  bool any_content = false;
  if (data.propertyID.type && data.propertyID.type <= 6) {
    json_object_set_new(retval, INTERNAL_SCALAR, json_integer(data.scalar));
    any_content = true;
  } else if (data.propertyID.type == 7) {
    OneNotePtr content(thus);
    error err = content.reposition(data.rawData);
    assert(err == Ok && "Already parsed, already filtered for errors");
    if (content.size()) {
        any_content = true;
    }
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
      json_object_set_new(retval, INTERNAL_DATA, raw);
    } else {
      base64_encodestate state;
      base64_init_encodestate(&state);
      std::vector<char> b64data((content.size() + 3) * 4 / 3);
      char *cur = &b64data[0];
      for (size_t i = 0; i < content.size(); i += 16384) {
        cur += base64_encode_block((const char*)content.data() + i,
                                   std::min(content.size() - i, (size_t)16384),
                                   cur,
                                   &state);
      }
      cur += base64_encode_blockend(cur, &state);
      json_object_set_new(retval, INTERNAL_BASE64, json_stringn(&b64data[0], cur - &b64data[0]));
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
            json_t *fn_json = file_node_to_json_html(thus, *fn);
            if (fn_json) {
              if (!file_nodes) {
                file_nodes = json_array();
              }
              json_array_append_new(file_nodes, fn_json);
            }
          }
          
        }
      }
      if (file_nodes) {
          json_object_set_new(retval, "children", flatten_arrays(file_nodes));
          any_content = true;
      }
  } else if (data.propertyID.type == 0x10 || data.propertyID.type == 0x11){
      json_t *sub_property = property_set_to_json_html(thus,
                                                       data.propertySet,
                                                       JCID());
      if (sub_property) {
        json_object_set_new(retval, "children", sub_property);
        any_content = true;
      }
  }
  flatten_out_style(retval);
  if (strcmp(id_str, "ContentChildNodesOfOutlineElement") == 0) {
    assert((data.propertyID.id&0xffff) == (ContentChildNodesOfOutlineElement&0xffff));
  }
  json_t *children = json_object_get(retval, "children");
  if (invisible_node(children)) {
    json_decref(retval);
    return NULL;
  }
  switch(data.propertyID.id&0xffff) {
  case TextExtendedAscii&0xffff:
  case RichEditTextUnicode&0xffff:
    json_object_set_new(retval, "id", json_string("#text"));
    break;
  case ContentChildNodesOfOutlineElement&0xffff:
    if (children) {
      if (jcid.index == (jcidOutlineElementNode & 0xffff)) {
	json_object_set_new(retval, "id", json_string("li"));
      }else {
	json_object_set_new(retval, "id", json_string("div"));
      }
    }
    break;    
  case ElementChildNodesOfSection&0xffff:
    if (children) {
      if (false && jcid.index == (jcidOutlineNode & 0xffff)) {
	json_object_set_new(retval, "id", json_string("ul"));
      } else if (jcid.index == (jcidOutlineElementNode & 0xffff)) {
	json_object_set_new(retval, "id", json_string("ul"));
      }else {
	json_object_set_new(retval, "id", json_string("div"));
      }
    }
    break;
  default:
    if (json_object_get(retval, "id")) {
      break;
    }
    if (children) {
      json_object_set_new(retval, "id", json_string("div"));
    }
  }
  if (!any_content) {
      json_decref(retval);
      return NULL;
  }
  return retval;
}
json_t* property_set_to_json_html(
    const OneNote*thus,
    const PropertySet&fn,
    const JCID &jcid) {
  json_t *retval = NULL;
  for (const auto &child : fn.rgPridsData) {
    json_t *child_json = property_to_json_html(thus, child, jcid);
    if (child_json) {
      if (!retval) {
        retval = json_array();
      }
      json_array_append_new(retval, child_json);
    }
  }
  return retval;
}
json_t* property_set_to_json_html(const OneNote*thus,
                                  const ObjectSpaceObjectPropSet&fn,
                                  const JCID&jcid) {
    return property_set_to_json_html(thus, fn.body, jcid);
}

json_t *file_node_to_json_html(const OneNote*thus, const FileNode&fn) {
    using namespace CanRevise;
    JCID jcid;
    if (fn.is_object_declaration()) {
        jcid = fn.object_declaration_jcid();
    }
  json_t *children = NULL;
  for (const auto&child : fn.children.children) {
    json_t *child_json = file_node_to_json_html(thus, child);
    if (child_json) {
      if (!children) {
        children = json_array();
      }
      json_array_append_new(children, child_json);
    }
  }
  json_t *properties = property_set_to_json_html(thus, fn.property_set, jcid);
  if (properties) {
    assert(json_is_array(properties));
    if (children) {
        json_array_extend(children, properties);
    } else {
        children = properties;
    }
  }
  if (children) {
      children = flatten_arrays(children);
  }
  return children;
}


std::string get_json_html(const OneNote*document) {

  RoleAndContext rc(1,
                    ExtendedGUID::nil());
  json_t *representation = get_json_html_representation(document,
                                                        &rc,
                                                        true);
  representation = flatten_arrays(representation);
  char *data = json_dumps(representation, JSON_ENSURE_ASCII | JSON_INDENT(2));
  json_decref(representation);
  std::string retval(data);
  free(data);
  return retval;
}

