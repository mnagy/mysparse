#include <stdio.h>

#include <sparse/lib.h>
#include <sparse/token.h>
#include <sparse/symbol.h>

int main(int argc, char **argv)
{
	struct symbol_list *list;
	struct symbol *sym;
	struct string_list *files = NULL;
	char *file;

	list = sparse_initialize(argc, argv, &files);
	FOR_EACH_PTR_NOTAG(files, file) {
		list = sparse(file);
	} END_FOR_EACH_PTR_NOTAG(file);

	return 0;
}
