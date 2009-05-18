/* Tree/CFG checking pass.
  Copyright (C) 2006, 2007 Free Software Foundation, Inc.
  Contributed by Nic Volanschi <nic.volanschi@free.fr>

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.  */

#include <stdio.h>

#include "tree-match.h"

/* Raise a warning upon detecting a satisfied condate.  The concept of
   condate (control & data property to be checked) is described in
   tree-match.h.  */

static void tree_check_warning(condate cond, struct statement *stmt, int check_option)
{
	/* FIXME: Use sparse_warning() and struct position here */
	fprintf(stderr, "I have found something\n");
#if 0
	location_t saved_location = input_location;

	if (EXPR_HAS_LOCATION(stmt))
		input_location = EXPR_LOCATION(stmt);

	warning(check_option, "user-defined warning %s: %s.",
		cond->name, cond->msg ? cond->msg : "");

	if (flag_tree_check_verbose) {
		fprintf(stderr, "%s:%d: instance = ", input_location.file,
			input_location.line);
		/*   print_local_holes (); */
		print_global_holes();
		fprintf(stderr, ",\n");
		fprintf(stderr, "%s:%d: reached: ", input_location.file,
			input_location.line);
		print_generic_expr(stderr, stmt, 0);
		fprintf(stderr, ".\n");
	}

	input_location = saved_location;
#endif
}

/* Initialization function for the tree-check pass.  */

static void tree_check_init(void)
{
	/* XXX: This is not needed for now.. */
#if 0
	basic_block bb;
	block_stmt_iterator bsi;

	reset_global_holes();

	FOR_EACH_BB(bb)
	    for (bsi = bsi_start(bb); !bsi_end_p(bsi); bsi_next(&bsi))
		TREE_VISITED(bsi_stmt(bsi)) = 0;
#endif
}

/* FIXME: For now, let's just tree_scan() */
#if 0
/* Visit a CFG node.  Used in tree_check_instance.  */

static bool check_node(cfg_node node, condate cond)
{
	tree stmt = cfg_node_stmt(node);

	if (!stmt || TREE_VISITED(stmt))
		return 0;

	TREE_VISITED(stmt) = 1;

	if (tree_match_disj(stmt, cond->to, node)) {
		tree_check_warning(cond, stmt, OPT_ftree_checks_);
		return 0;	/* follow_none */
	}

	/* Inspect successors? */
	if (cond->avoid && tree_match_disj(stmt, cond->avoid, node)) {
		return 0; /* follow_none */ ;
	} else
		return 1;	/* follow_all */
}
#endif

/* FIXME: For now, let's just tree_scan() */
#if 0
/* Check a condate instance over the CFG of the current function.  */

static void tree_check_instance(condate cond)
{
	VEC(cfg_node, heap) * stack = VEC_alloc(cfg_node, heap, 100);
	basic_block bb;
	cfg_node node;
	tree stmt;

	/* Push from nodes on the stack.  */

	FOR_EACH_BB(bb) {
		block_stmt_iterator bsi;
		tree stmt;

		for (bsi = bsi_start(bb); !bsi_end_p(bsi); bsi_next(&bsi)) {
			pattern patt;
			stmt = bsi_stmt(bsi);
			patt = cond->from;

			if (!patt
			    || tree_match_disj(stmt, patt, bsi_cfg_node(bsi))) {
				node = bsi_cfg_node(bsi);
				stmt = cfg_node_stmt(node);

				VEC_safe_push(cfg_node, heap, stack, node);

				if (stmt)
					TREE_VISITED(stmt) = 1;

			}
		}
	}

	/* Perform depth-first search.  */
	while (VEC_length(cfg_node, stack) != 0) {
		cfg_node succ_node;
		bool push_it;

		node = VEC_pop(cfg_node, stack);
		stmt = cfg_node_stmt(node);

		if (node->next == NULL) {
			edge e;
			edge_iterator ei;

			bb = bb_for_stmt(stmt);

			FOR_EACH_EDGE(e, ei, bb->succs) {
				if (e->dest == EXIT_BLOCK_PTR)
					continue;

				if (TREE_CODE(stmt) == COND_EXPR
				    && (e->flags & EDGE_TRUE_VALUE
					&& cond->avoid_then
					&& tree_match_disj(COND_EXPR_COND(stmt),
							   cond->avoid_then,
							   node))) {
					continue;
				}

				if (TREE_CODE(stmt) == COND_EXPR
				    && (e->flags & EDGE_FALSE_VALUE
					&& cond->avoid_else
					&& tree_match_disj(COND_EXPR_COND(stmt),
							   cond->avoid_else,
							   node))) {
					continue;
				}

				succ_node = bb_1st_cfg_node(e->dest);
				push_it = check_node(succ_node, cond);

				if (push_it)
					VEC_safe_push(cfg_node, heap, stack,
						      succ_node);
			}
		} else {
			succ_node = node->next;
			push_it = check_node(succ_node, cond);

			if (push_it)
				VEC_safe_push(cfg_node, heap, stack, succ_node);
		}
	}

	VEC_free(cfg_node, heap, stack);
}
#endif

/* FIXME: For now, let's just tree_scan() */
#if 0
/* Collect new condate instances.  An instance is new if the
   combination of global hole values has not been seen yet.  */

static void push_global_holes_if_new(VEC(hole_p, heap) * stack)
{
	unsigned int i;
	hole_p h;

	/* Check if these global holes were already seen.  */
	for (i = 0; VEC_iterate(hole_p, stack, i, h); i++)
		if (eq_global_holes(global_holes, h)) {
			reset_global_holes();
			return;
		}

	VEC_safe_push(hole_p, heap, stack, save_global_holes());
	reset_global_holes();
}
#endif

/* Check a trivial condate consisting only in a (FROM) pattern. 
   This comes to reporting every match of the pattern in a function.  */

static void tree_scan(struct symbol_list *list, condate cond)
{
	struct symbol *sym;

	FOR_EACH_PTR(list, sym) {
		struct statement *stmt;

		if (!sym->ctype.base_type
		    && sym->ctype.base_type->type != SYM_FN)
			continue;

		/* FIXME: This is butt-ugly.. */
		FOR_EACH_PTR(sym->ctype.base_type->stmt->stmts, stmt) {
			if (!cond->from
			    || tree_match_disj(stmt, cond->from, NULL)) {
				// XXX: 0 is for "not verbose"
				tree_check_warning(cond, stmt, 0);
				reset_global_holes();
			}
		} END_FOR_EACH_PTR(stmt);
	} END_FOR_EACH_PTR(sym);
}

/* This is the old function.. */
#if 0
static void tree_scan(condate cond)
{
	basic_block bb;

	FOR_EACH_BB(bb) {
		block_stmt_iterator bsi;
		tree stmt;

		for (bsi = bsi_start(bb); !bsi_end_p(bsi); bsi_next(&bsi)) {
			stmt = bsi_stmt(bsi);

			if (!cond->from
			    || tree_match_disj(stmt, cond->from,
					       bsi_cfg_node(bsi))) {
				tree_check_warning(cond,
						   cfg_node_stmt(bsi_cfg_node
								 (bsi)),
						   OPT_ftree_check_);
				reset_global_holes();
			}
		}
	}
}
#endif

/* Check a condate on a function.  */

static void tree_check(struct symbol_list *list, condate cond)
{
/* FIXME: For now, let's just tree_scan() */
#if 0
	VEC(hole_p, heap) * stack;
	pattern patt = cond->from;
	basic_block bb;
#endif

	/* Check for trivial condates.  */
	if (!cond->to) {
		tree_scan(list, cond);
		return;
	}

/* FIXME: For now, let's just tree_scan() */
#if 0
	/* Allocate stack for collecting condate instances.  */
	stack = VEC_alloc(hole_p, heap, 10);
	patt = cond->from;

	FOR_EACH_BB(bb) {
		block_stmt_iterator bsi;
		tree stmt;

		for (bsi = bsi_start(bb); !bsi_end_p(bsi); bsi_next(&bsi)) {
			stmt = bsi_stmt(bsi);

			if (!patt
			    || tree_match_disj(stmt, patt, bsi_cfg_node(bsi)))
				push_global_holes_if_new(stack);
		}
	}

	while (VEC_length(hole_p, stack)) {
		hole_p h = VEC_pop(hole_p, stack);

		restore_global_holes(h);
		tree_check_instance(cond);
		tree_check_init();	/* clear visited flag */
	}

	VEC_free(hole_p, heap, stack);
#endif
}

/* Print a condate.  */

void print_cond(condate cond)
{
	fprintf(stderr, "condate %s {", cond->name);
	pat_print(cond->from);
	fprintf(stderr, ", ");
	pat_print(cond->to);
	fprintf(stderr, ", ");
	pat_print(cond->avoid);
	fprintf(stderr, ", ");
	pat_print(cond->avoid_then);
	fprintf(stderr, ", ");
	pat_print(cond->avoid_else);
	fprintf(stderr, "} warning(%s);\n", cond->msg ? cond->msg : "<null>");
}

/* Check a list of condates on the current function.  */

static void execute_conds(condate conds[], int n, struct symbol_list *list)
{
	int i;
	condate cond;

	for (i = 0; i < n; i++) {
		cond = conds[i];
		tree_check(list, cond);
	}
}

condate conds[CONDMAX];		/* list of condated to check */
int n_conds = 0;		/* number of condates to check */

/* Flush the list of condates.  */

static void delete_conds(condate conds[], int n)
{
	int i;
	condate cond;

	for (i = 0; i < n; i++) {
		cond = conds[i];
		rmcond(cond);
	}
	n_conds = 0;
}

/* Open file containing the checks.  Used by the parser condate.y.  */
FILE *checkfile;

/* Parse the file containing condates definitions, and cache the result.  */

static int parse_check_file_once(const char *file)
{
	static const char *current_check_file = NULL;

	if (current_check_file) {
		/* Not called for the first time.  */
		if (!strcmp(current_check_file, file))
			/* file hasn't changed */
			return 0;
		else
			delete_conds(conds, n_conds);
	}

	current_check_file = file;
	checkfile = fopen(file, "r");

	if (!checkfile) {
		fprintf(stderr, "tree-check-file %s not found\n",
			file);
		return -1;
	}

	if (condate_parse() != 0) {
		fclose(checkfile);
		return -2;
	}

	fclose(checkfile);
	return 0;
}

/* Main function of the tree-check pass.  Triggered either by
   -ftree-check or -ftree-checks.  */

void
execute_tree_check(const char *file, const char *string, struct symbol_list *list)
{
	tree_check_init();

	if (file) {
		if (parse_check_file_once(file) < 0)
			return;
	} else if (string != NULL) {
		static const char *current_check_string = NULL;
		if (!current_check_string) {
			condate cond =
			    mkcond(string, mkpat(string),
				   NULL, NULL, NULL, NULL);
			add_condate(cond);
			current_check_string = string;
		}
	}
	execute_conds(conds, n_conds, list);
}
