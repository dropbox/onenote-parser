#include "one_note_ptr.hpp"
#include "enums.hpp"
#include "one_note.hpp"
#include "one_note_json.hpp"
#include "one_note_json_html.hpp"
error Header::deserialize(OneNotePtr *ptr) {
    error err = Ok;
    FOREACH_HEADER_MEMBER(DESERIALIZE)
    err = ptr->deserialize_bytes(reserved, sizeof(reserved));
    return err;
}


int main(int argc, char **argv) {
  OneNote retval(argv[1]);
  Header header;
  OneNotePtr ptr(&retval);
error err = header.deserialize(&ptr);
if (err != Ok) {
assert(err == Ok && false);
}
 ptr.reposition(header.fcrFileNodeListRoot);
 FileNodePtr curPath;
 ptr.deserialize_FileNodeList(&retval.root, &curPath);
 RoleAndContext rc(1, ExtendedGUID::nil());
 //Printer printer;
 //retval.walk_revisions(printer, &rc, true);
 
 std::string json_val;
 if (argc > 2 && strstr(argv[2], "-json")) {
   json_val = get_json(&retval,
		       &rc,
		       true);
 } else {
   json_val = get_json_html(&retval);
 }
 printf("%s\n", json_val.c_str());
 return 0;
}
