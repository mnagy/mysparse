#include <stdio.h>

#include <sparse/lib.h>
#include <sparse/token.h>
#include <sparse/symbol.h>

void execute_tree_check(const char *file, const char *string, struct symbol_list *list);

int main(int argc, char **argv)
{
	struct symbol_list *list;
	struct symbol *sym;
	struct string_list *files = NULL;
	char *file;

	list = sparse_initialize(argc, argv, &files);
	FOR_EACH_PTR_NOTAG(files, file) {
		list = sparse(file);
		execute_tree_check(NULL, "from %_", list);
	} END_FOR_EACH_PTR_NOTAG(file);

	return 0;
}
