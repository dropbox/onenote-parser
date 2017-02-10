#include <stdlib.h>
#include <stdio.h>
#include <set>
#include "one_note.hpp"
#include "enums.hpp"
#include "properties.hpp"
OneNote::OneNote (const char*filename) {
  FILE * fp = fopen(filename, "rb");
  fseek(fp,0,SEEK_END);
  size_t size = ftell(fp);
  fseek(fp,0,SEEK_SET);
  this->data.resize(size);
  fread(&data[0], 1, size, fp);
  fclose(fp);
}

error OneNote::get_assoc_guid_to_ref(ExtendedGUID guid, FileChunkReference64*ref) const {
  auto where = guid_to_ref.find(guid);
  if (where == guid_to_ref.end()) {
    return wrap(error::UNKNOWN_GUID);
  }
  *ref = where->second;
  return Ok;
}
void OneNote::set_assoc_guid_to_ref(ExtendedGUID guid, FileChunkReference64 ref) const {
  guid_to_ref[guid] = ref;
}
void OneNote::registerRevisionManifestList(const ExtendedGUID &guid,
					   const FileNodePtr&ptr) const{
  revision_manifest_lists[guid] = ptr;
  revision_list_order.push_back(guid);
}
void OneNote::registerRevisionManifest(const FileNode&fn) const{
  Revision&to_modify = revision_map[fn.gosid];
  to_modify.gosid = fn.gosid;
  to_modify.dependent = fn.sub_type.RevisionManifest.ridDependent;
  current_revision = fn.gosid;
}
void OneNote::registerAdditionalRevisionRole(const ExtendedGUID&rid,
					     uint32_t revision_role,
					     const ExtendedGUID&gctxid) const {
  revision_role_map.insert(RevisionRoleMap::value_type(rid,
						       RoleAndContext(revision_role,
								      gctxid)));
}

bool OneNote::has_revision_role(const ExtendedGUID&rid,
				const RoleAndContext &revision_role) const {
  auto where = revision_role_map.equal_range(rid);
  for (auto iter = where.first; iter != where.second; ++iter) {
    if (iter->second.first == revision_role.first && iter->second.second == revision_role.second) {
      return true;
    }
  }
  return false;
}

void OneNote::walk_revision(const ExtendedGUID&rid,
			    Walker&walker,
			    const FileNodePtr& revision_manifest,
			    const RoleAndContext *revision_role,
			    bool only_latest) const {
  const FileNode*ptr = revision_manifest.dereference(this);
  assert(ptr && ptr->id == RevisionManifestListReferenceFND);
  /*  if (ptr == NULL ||
      (ptr->id != RevisionManifestListReferenceFND &&
       ptr->id != RevisionManifestListStartFND)) {
    return;
    }*/
  std::set<ExtendedGUID> valid_revisions;
  for (auto iter = ptr->children.children.rbegin(); iter != ptr->children.children.rend(); ++iter) {
    if (revision_role == NULL ||
	has_revision_role(iter->gosid, *revision_role)) {
      valid_revisions.insert(iter->gosid);
      if (only_latest) {
	break;
      }
    }
  }
  bool ok_group = false;
  for (const auto &child : ptr->children.children) {
    if (child.id == RevisionManifestStart4FND||
	child.id == RevisionManifestStart6FND||
	child.id == RevisionManifestStart7FND) {
      ok_group = (valid_revisions.find(child.gosid) != valid_revisions.end());
    }
    if (ok_group) {
	if ((child.id == RootObjectReference2FNDX ||
	     child.id == RootObjectReference3FND) &&
	    child.sub_type.RootObjectReference.RootObjectReferenceBase.RootRole == 1) {
	  auto where = this->guid_to_object.find(child.gosid);
	  if (where != this->guid_to_object.end()) {
	    auto rootObject = where->second.dereference(this);
	    walker(this, *rootObject);
	}
      }
    }
    /*
      if (child.gosid == ExtendedGUID::nil()) {
      printf("Node : %s\n", child.fnd_name());
	} else {
	printf("Node : %s [%s]\n", child.fnd_name(), child.gosid.toString().c_str());
	}
	if (child.id == ObjectGroupListReferenceFND) {
	child.print(this);
	}*/
  }
}
void OneNote::walk_revisions(Walker&walker,
			     const RoleAndContext *revisionRole,
			     bool only_latest) const {
  for (const auto &key : revision_list_order) {
    const auto &val = revision_manifest_lists[key];
    walk_revision(key, walker, val, revisionRole, only_latest);
  }
}
void Printer::operator()(const OneNote*thus, const FileNodePtr &fp) {
  auto val = fp.dereference(thus);
  if (val) {
    (*this)(thus, *val);
  }
}
void Printer::operator()(const OneNote*thus, const FileNode &fp) {
  fp.print(thus);
}
