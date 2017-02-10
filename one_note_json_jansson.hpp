#include <jansson.h>

json_t* get_json_representation(const OneNote*document,
				const RoleAndContext *revision_role,
				bool only_latest);
