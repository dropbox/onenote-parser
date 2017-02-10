#ifndef _ONE_NOTE_HPP_
#define _ONE_NOTE_HPP_
#include <vector>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <map>

#include "guid.hpp"
#include "one_note_ptr.hpp"
#include "enums.hpp"
class Walker;
typedef std::pair<uint32_t, ExtendedGUID> RoleAndContext;
typedef std::multimap<ExtendedGUID, RoleAndContext> RevisionRoleMap;
struct OneNote {
  mutable std::vector<ExtendedGUID > revision_list_order;
  mutable std::map<ExtendedGUID, Revision > revision_map;
  mutable std::map<ExtendedGUID, FileNodePtr> revision_manifest_lists;
  mutable std::map<ExtendedGUID, FileChunkReference64> guid_to_ref;
  mutable std::map<ExtendedGUID, FileNodePtr> guid_to_object;
  mutable std::multimap<ExtendedGUID,
			std::pair<uint32_t, ExtendedGUID> >revision_role_map;
  mutable ExtendedGUID current_revision;
  FileNodeList root;
  std::vector<uint8_t> data;
  OneNote(const char *filename);
  OneNote(const OneNote&)=delete;
  OneNote(OneNote&&)=delete;
  OneNote& operator=(const OneNote&)=delete;
  void registerRevisionManifest(const FileNode&fn) const;
  void registerRevisionManifestList(const ExtendedGUID&revision_list_id, const FileNodePtr&ptr) const;
  void registerAdditionalRevisionRole(const ExtendedGUID&rid,
				      uint32_t revision_role,
				      const ExtendedGUID&gctxid) const;
  error get_assoc_guid_to_ref(ExtendedGUID guid, FileChunkReference64*ref)const;
  void set_assoc_guid_to_ref(ExtendedGUID guid, FileChunkReference64 ref)const;
  void walk_revisions(Walker&walker,
		      const RoleAndContext *revisionRole, // if missing, any are accepted
		      bool only_latest=true)const;
  void walk_revision(const ExtendedGUID&rid,
		     Walker &walker,
		     const FileNodePtr& revision_manifest,
		     const RoleAndContext *revisionRole,
		     bool only_latest)const;
  bool has_revision_role(const ExtendedGUID&rid,
			 const RoleAndContext&revision_role)const;
};
class Walker {
public:
  virtual void operator()(const OneNote*thus, const FileNodePtr&) = 0;
  virtual void operator()(const OneNote*thus, const FileNode&) = 0;
  virtual ~Walker(){}
};
class Printer : public Walker {
public:
  Printer(){}
  void operator()(const OneNote*thus, const FileNodePtr&fp);
  void operator()(const OneNote*thus, const FileNode&);
};
#endif
