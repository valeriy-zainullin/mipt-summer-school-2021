#pragma once

struct ast_expr {
	struct vector* assign_exprs; // Assignment expressions are separated with commas.
};

bool ast_expr_init(struct ast_expr* expr);
void ast_expr_deinit();

struct ast_expr* ast_expr_new();
struct ast_expr* ast_expr_delete(struct ast_expr* expr);

void ast_print_expr(FILE* file, struct ast_expr const* expr, size_t indent_level);

struct ast_assign_expr {
	
};

struct ast_cond_expr {
	struct ast_assign_expr* assign_expr;
};

struct ast_log_or_expr {
};

struct ast_log_and_expr {
};

struct ast_bit_or_expr {
};

struct ast_bit_xor_expr {
};

struct ast_bit_and_expr {
};

struct ast_equal_expr {
};

struct ast_rel_expr {
};

struct ast_bit_shift_expr {
};

struct ast_add_expr {
};

struct ast_mult_expr {
};

struct ast_cast_expr {
	enum {
	} type;
};

struct ast_unary_expr {
	struct {
		
	} unary_expr_type;
};

struct ast_init_list_expr {
};

struct ast_postfix_expr_op {
	enum ast_postfix_expr_op_type {
		AST_POSTFIX_EXPR_OP_INDEXATION,
		AST_POSTFIX_EXPR_OP_FUNCTION_CALL,
		AST_POSTFIX_EXPR_OP_STRUCT_MEMBER_ACCESS,
		AST_POSTFIX_EXPR_OP_STRUCT_PTR_MEMBER_ACCESS,
		AST_POSTFIX_EXPR_OP_INCREMENT,
		AST_POSTFIX_EXPR_OP_DECREMENT
	} operator_type;
};
struct ast_indexation_op {
	struct ast_postfix_expr_op postfix_expr_op;
	// struct ast_expr* index_expr;
};
struct ast_func_call_op {
	struct ast_postfix_expr_op postfix_expr_op;
	// struct ast_arg_expr_list* args;
};
struct ast_struct_member_access_op {
	struct ast_postfix_expr_op postfix_expr_op;
	// struct ast_identifier member_idfr;
};
struct ast_structp_member_access_op {
	struct ast_postfix_expr_op postfix_expr_op;
	// struct ast_identifier member_idfr;
};
struct ast_primary_expr;
struct ast_postfix_expr {
	struct ast_primary_expr* primary_expr;
	struct ast_init_list_expr* init_list_expr;
	struct vector* op_sequence;
};

struct ast_primary_expr {
	enum ast_primary_expr_type {
		AST_PRIMARY_EXPR_IDENTIFIER,
		AST_PRIMARY_EXPR_CONSTANT,
		AST_PRIMARY_EXPR_STRING_LITERAL,
		AST_PRIMARY_EXPR_WRAPPED_EXPR    // Parenthised expr.
	} primary_expr_type;
};
