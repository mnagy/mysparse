/* Library for tree matching.
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

/* The tree matching user interface is described in tree-pattern.h.  */

hole global_holes[GLOBAL_MAX];
hole local_holes[LOCAL_MAX];

/* Check whether a variable is a temporary introduced by the compiler.  */

static bool is_tmp_var(struct statement *var)
{
	return false;
#if 0
	const char *name;
	if (TREE_CODE(var) != VAR_DECL)
		return false;

	/* Artificial decls like return values have no name, so don't get hung.  */
	if (!DECL_NAME(var))
		return true;

	name = IDENTIFIER_POINTER(DECL_NAME(var));
	return !strncmp(name, "D.", 2) || strchr(name, '.');
#endif
}

/* If t is a cast expression, return the value without the cast.  */

static tree substitute_cast_expr(struct statement *t)
{
#if 0
	if (TREE_CODE(t) == CONVERT_EXPR || TREE_CODE(t) == NOP_EXPR)
		return TREE_OPERAND(t, 0);
#endif

	return NULL;
}

/* If t is a temporary and you can find its def from current node
   upwards in the same block, (including the current node or not),
   return its value; otherwise return NULL.  INCLUDING_CRT is set to
   true on recursive calls.  */

static struct statement *substitute_tmp_var(struct statement *var, cfg_node ctx_node, bool including_crt)
{
	return NULL;
#if 0
	struct statement *val;
	if (!is_tmp_var(var))
		return NULL;

	/* We are at the beginning of the block, or the context is unknown.  */
	if (!ctx_node)
		return NULL;

	if ((val = NULL, tree_scanf(cfg_node_stmt(ctx_node), "%t = %t",
				    NULL, &var, &val))
	    || (val = NULL,
		tree_scanf(cfg_node_stmt(ctx_node), "%t = (%_)%t",
			   NULL, &var, &val))) {
		if (including_crt) {
			return val;
		} else
			/* We are on a def, but exclude it => don't go up.  */
			return NULL;
	} else
		/* We are not on a def => go up and try again.  */
		return substitute_tmp_var(var, ctx_node->prev, true);
#endif
}

static struct tree_chunk *alloc_tree_chunk(void)
{
	tree_chunk *res = __alloc_tree_chunk(0);
	memset(res, 0, sizeof(*res));
	return res;
}

static void free_tree_chunk(struct tree_chunk *tc)
{
	__free_tree_chunk(tc);
}

static int tree_chunk_list_size(struct tree_chunk_list *list)
{
	return ptr_list_size(list);
}

/* FIXME: These should actually DO something. */
static struct tree_chunk_list *lazy_dump_generic_node(struct statement *stmt)
{
	struct tree_chunk_list *res = NULL;
	struct tree_chunk *tc = alloc_tree_chunk();

	add_ptr_list(res, tc);
	return res;
}

static void pp_free_list(struct tree_chunk_list *list)
{
	struct tree_chunk *tc;

	FOR_EACH_PTR(list, tc) {
		free_tree_chunk(tc);
	} END_FOR_EACH_PTR(tc);
}

static bool tree_equal_mod_tmps(struct statement *, struct statement *, cfg_node, cfg_node);

/* Worker function for tree_equal_mod_tmps.  */

static bool tree_equal(struct statement *t1, struct statement *t2, cfg_node ctx_node1, cfg_node ctx_node2)
{
	struct tree_chunk_list *chunks1 = NULL;
	struct tree_chunk_list *chunks2 = NULL;
	struct tree_chunk chunk1, chunk2;
	int len1, len2, i;
	bool res = false;

	if ((!t1 || !t2))
		return (t1 == t2);

	if (t1 == t2)
		return true;

	chunks1 = lazy_dump_generic_node(t1);
	chunks2 = lazy_dump_generic_node(t2);

	len1 = tree_chunk_list_size(chunks1);
	len2 = tree_chunk_list_size(chunks2);

	if (len1 != len2)
		goto mismatch;

	/* XXX: This loop will iterate through tree chunks and
	 * compare them. I think that with sparse, we could just
	 * write a function to compare the trees and be done with it.
	 */
	for (i = 0; VEC_iterate(tree_chunk, chunks1, i, chunk1); i++) {
		chunk2 = VEC_index(tree_chunk, chunks2, i);

		if (((chunk1->t || chunk2->t)
		     && (!(chunk1->t && chunk2->t)
			 || !tree_equal_mod_tmps(chunk1->t, chunk2->t,
						 ctx_node1, ctx_node2)))
		    || ((chunk1->s || chunk2->s)
			&& (!(chunk1->s && chunk2->s)
			    || strcmp(chunk1->s, chunk2->s)))
		    || (chunk1->c != chunk2->c))
			goto mismatch;
	}

	res = true;
 mismatch:
	pp_free_list(chunks1);
	pp_free_list(chunks2);
	return res;
}

/* Check if two trees are equal, modulo casts and substitutions of
   tmp vars with their values.  */

static bool
tree_equal_mod_tmps(struct statement *t1, struct statement *t2, cfg_node ctx_node1, cfg_node ctx_node2)
{
	//struct statement *val;

	if ((!t1 || !t2))
		return (t1 == t2);

	return tree_equal(t1, t2, ctx_node1, ctx_node2);
#if 0
	return (tree_equal(t1, t2, ctx_node1, ctx_node2)
		|| ((val = substitute_tmp_var(t1, ctx_node1, false)) != NULL
		    && tree_equal_mod_tmps(val, t2, ctx_node1, ctx_node2))
		|| ((val = substitute_tmp_var(t2, ctx_node2, false)) != NULL
		    && tree_equal_mod_tmps(t1, val, ctx_node1, ctx_node2))
		|| ((val = substitute_cast_expr(t1)) != NULL
		    && tree_equal_mod_tmps(val, t2, ctx_node1, ctx_node2))
		|| ((val = substitute_cast_expr(t2)) != NULL
		    && tree_equal_mod_tmps(t1, val, ctx_node1, ctx_node2)));
#endif
}

static char tree_1st_char(struct statement *);

/* Get the first character of (the printed form of) a tree chunk.  */

static char chunk_1st_char(tree_chunk chunk)
{
	if (chunk->t)
		return tree_1st_char(chunk->t);

	else if (chunk->s)
		return *chunk->s;

	else
		return chunk->c;
}

/* Search the first chunk of a lazy list not consisting of whitespace.  */

static tree_chunk
chunks_lookahead(VEC(tree_chunk, heap) * chunks, unsigned int i)
{
	tree_chunk chunk;

	for (; VEC_iterate(tree_chunk, chunks, i, chunk); i++)
		if (chunk->c == 0 || chunk->c != ' ')
			break;

	return chunk;
}

/* Get the first character of (the printed form of) a tree.  */

static char tree_1st_char(struct statement *t)
{
	VEC(tree_chunk, heap) * chunks;
	tree_chunk chunk;

	/* XXX: Uhm.. hopefuly, this won't be missing.. */
#if 0
	/* Don't hung on unnamed vars, etc.  Cannot dump these nodes.  */
	if (TREE_CODE(t) == VAR_DECL || TREE_CODE_CLASS(TREE_CODE(t)) == 'x')
		return '\0';
#endif

	chunks = lazy_dump_generic_node(t);
	chunk = chunks_lookahead(chunks, 0);
	pp_free_list(chunks);

	return chunk_1st_char(chunk);
}

/* Get the first non-space character in a pattern.  */

static char pattern_lookahead(patt_info * patt, int n)
{
	const char *s = patt->format_spec + n;
	int skip = 0;

	do {
		if (s[0] == '\\' && s[1] == ')')
			skip = 2;

		else if (s[0] == ' ')
			skip = 1;

		else
			skip = 0;

		s += skip;
	} while (skip);

	return s[0];
}

static bool match_tree_pattinfo(struct statement *, patt_info *, const char *, cfg_node);

/* Worker function for match_tree_pattinfo.  Matches a lazy list with
   a pattern.  */

static bool
match_chunks_pattinfo(VEC(tree_chunk, heap) * chunks, patt_info * patt,
		      const char *delim, cfg_node ctx_node)
{
	unsigned int i;
	tree_chunk chunk;

	for (i = 0; VEC_iterate(tree_chunk, chunks, i, chunk); i++) {
		if (chunk->t) {
			/* XXX: Get next char from a tree. */
			/* Compute delimiter for t.  */
			char next_char = (i + 1 == VEC_length(tree_chunk,
							      chunks) ? *delim :
					  chunk_1st_char(chunks_lookahead
							 (chunks, i + 1)));

			if (!match_tree_pattinfo
			    (chunk->t, patt, &next_char, ctx_node)) {
				return 0;
			}

		} else if (chunk->s) {
			if (*patt->format_spec == '%') {
				return 0;
			} else {
				if (memcmp
				    (patt->format_spec, chunk->s,
				     strlen(chunk->s))) {
					return 0;
				}

				patt->format_spec += strlen(chunk->s);
			}
		} else {
			/* one-character chunk */
			if (chunk->c == ' ') {
				/* whitespace */
				while (*patt->format_spec == ' ')
					patt->format_spec++;

			} else {
				/* not whitespace */
				if (*patt->format_spec == '%') {
					return 0;
				} else {
					if (*patt->format_spec != chunk->c) {
						return 0;
					}
					patt->format_spec++;
				}
			}
		}
	}
	return 1;
}

/* Match a tree t, consuming the pattern.  If the args in patt_info is
   not null, the pattern has anonymous holes, indicated by the
   va_args, so consume the args as you go.  Otherwise (args is null),
   you have a pattern with named holes.  The ctx_node (if non-null)
   indicates the cfg_node where the tree is supposed to occur, in case
   some tmp vars are to be searched for from this point backwards.  */

static bool
match_tree_pattinfo(struct statement *t, patt_info * patt, const char *delim,
		    cfg_node ctx_node)
{
	struct statement **pt;
	hole *ph = NULL;
	bool res;
	int parskip = 0;
	struct statement *val;

	info(t->pos, "tree pattinfo");
	if (patt->format_spec[0] == '%'
	    && *delim == pattern_lookahead(patt, 2)) {
		/* lookahead(1) ok */
		if (patt->format_spec[1] != '_') {
			/* not "any" hole */
			if (patt->args_ptr)	/* anonymous holes */
				pt = va_arg(*patt->args_ptr, struct statement **);
			else {
				/* named holes */
				ph = get_hole_named(patt->format_spec[1]);

				if (!ph)
					die
					    ("Invalid pattern variable: %%%c\n",
					     patt->format_spec[1]);

				pt = &ph->tree;
			}

			if (!*pt) {
				/* var hole */
				/* refuse to catch a tmpvar def */
				/* XXX: We don't do temporary variables.. */
#if 0
				if (is_tmp_var(t) && ctx_node &&
				    (tree_scanf
				     (cfg_node_stmt(ctx_node), "%t = %_", NULL,
				      &t)
				     || tree_scanf(cfg_node_stmt(ctx_node),
						   "%t = (%_)%_", NULL, &t))) {
					return 0;
				}
#endif

				*pt = t;
				if (ph)
					ph->ctx = ctx_node;
			} else {
				/* instantiated hole */
				if (!tree_equal_mod_tmps
				    (*pt, t, ph ? ph->ctx : NULL, ctx_node)) {
					return 0;
				}
			}
		}
		/* else (%_) just go on */
		patt->format_spec += 2;	/* consume %h */
		return 1;
	} else {
		/* Can't swallow a whole tree, must recurse on it.  */
		VEC(tree_chunk, heap) * chunks;

		/* Check an eventual pattern-only '(' to be skipped.  */
		if (patt->format_spec[0] == '\\' && patt->format_spec[1] == '(') {
			patt->format_spec += 2;
			parskip = 1;
		}

		/* XXX: Commented out for now */
#if 0
		/* On a tmpvar or a cast, there is no point to recurse directly (they
		   cannot be in the pattern), so substitute it before.  */
		while ((val = substitute_tmp_var(t, ctx_node, false)) != NULL
		       || (val = substitute_cast_expr(t)) != NULL) {
			t = val;
		}
#endif

		//maybe_init_pretty_print(stdout);
		chunks = lazy_dump_generic_node(t);
		res = match_chunks_pattinfo(chunks, patt, delim, ctx_node);
		pp_free_list(chunks);

		/* if needed, look for corresponding pattern-only ')' */
		if (res && parskip) {
			if (patt->format_spec[0] == '\\'
			    && patt->format_spec[1] == ')') {
				patt->format_spec += 2;
			} else {
				res = 0;
			}
		}

		return res;
	}
}

/* Check whether a hole name represents a global variable.  */

inline bool is_global_hole(char c)
{
	return 'A' <= c && c <= 'Z';
}

/* Get the hole named by a (local or global) variable name.  */

hole *get_hole_named(char c)
{
	if ('a' <= c && c <= 'z')
		return &local_holes[c - 'a'];

	if ('A' <= c && c <= 'Z')
		return &global_holes[c - 'A'];

	return NULL;
}

/* Unbind all global variables.  */

void reset_global_holes(void)
{
	int i;
	for (i = 0; i < GLOBAL_MAX; i++) {
		global_holes[i].tree = NULL;
		global_holes[i].ctx = NULL;
	}
}

/* Unbind all local variables.  */

void reset_local_holes(void)
{
	int i;
	for (i = 0; i < LOCAL_MAX; i++) {
		local_holes[i].tree = NULL;
		local_holes[i].ctx = NULL;
	}
}

/* Save the values of all global variables in a buffer.  */

hole *save_global_holes(void)
{
	hole *buf = xmalloc(sizeof(global_holes));
	memcpy(buf, global_holes, sizeof(global_holes));
	return buf;
}

/* Restore the values of all global variables from a buffer.  */

void restore_global_holes(hole_p saved)
{
	memcpy(global_holes, saved, sizeof(global_holes));
	free(saved);
}

/* Compare two sets of global variables.  */

bool eq_global_holes(hole * holes1, hole * holes2)
{
	int i;

	for (i = 0; i < GLOBAL_MAX; i++)
		if (!tree_equal_mod_tmps(holes1[i].tree, holes2[i].tree,
					 holes1[i].ctx, holes2[i].ctx))
			return 0;

	return 1;
}

/* Print the list of bounded local variables.  */

void print_local_holes(void)
{
	int i;

	for (i = 0; i < LOCAL_MAX; i++) {
		if (local_holes[i].tree) {
			fprintf(stderr, "local_holes[%d] == ", i);
			//print_generic_expr(stderr, local_holes[i].tree, 0);
			fprintf(stderr, "\n");
		}
	}
}

/* Print the list of bounded global variables.  */

void print_global_holes(void)
{
	int i, state = 0;

	fprintf(stderr, "{");

	for (i = 0; i < GLOBAL_MAX; i++) {
		if (global_holes[i].tree) {
			if (state)
				fprintf(stderr, ", ");
			fprintf(stderr, "%c <- ", 'A' + i);
			//print_generic_expr(stderr, global_holes[i].tree, 0);
			state = 1;
		}
	}

	fprintf(stderr, "}");
}

/* Match a tree with a pattern with anonymous holes, and bind the
   corresponding subtrees to the list of extra arguments.  Returns
   true if the tree completely matched the pattern, false otherwise.
   Even on unsuccessful match, some holes might be filled in.  */

bool tree_scanf(struct statement *t, const char *fmt, cfg_node ctx_node, ...)
{
	patt_info patt;
	va_list ap;
	bool res;

	va_start(ap, ctx_node);
	patt.args_ptr = &ap;
	patt.format_spec = fmt;
	res = match_tree_pattinfo(t, &patt, "", ctx_node);
	va_end(ap);

	/* nothing left in the pattern ? */
	if (res && (*patt.format_spec == '\0'))
		return true;
	else
		return false;
}

/* Match a tree against an atomic pattern with named holes.  */

bool tree_match(struct statement *t, const char *fmt, cfg_node ctx_node)
{
	patt_info patt;
	bool res;
	hole *old_global_holes = save_global_holes();

	patt.args_ptr = NULL;
	patt.format_spec = fmt;
	reset_local_holes();
	res = match_tree_pattinfo(t, &patt, "", ctx_node);

	/* nothing left in the pattern ? */
	if (res && (*patt.format_spec == '\0'))
		return true;
	else {
		restore_global_holes(old_global_holes);	/* unbind global holes */
		return false;
	}
}

/* Match a tree against a disjunctive pattern with named holes.  */

bool tree_match_disj(struct statement *t, pattern patt, cfg_node ctx_node)
{
	if (!patt)
		return false;

	return (tree_match(t, patt->format_spec, ctx_node)
		|| (patt->next && tree_match_disj(t, patt->next, ctx_node)));
}
