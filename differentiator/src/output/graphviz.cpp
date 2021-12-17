#include "output/graphviz.h"

#include "macroutils.h"
#include "tree.h"

#include <assert.h>
#include <stdio.h>

void graphviz_writer_init(GraphvizWriter* graphviz_writer, FILE* output_stream, bool is_debug) {
	graphviz_writer->stream = output_stream;
	graphviz_writer->is_debug = is_debug;
}

void graphviz_writer_deinit(GraphvizWriter* graphviz_writer) {
	(void) graphviz_writer;
}

void graphviz_writer_before_differentiation(void* callback_arg, const Tree* tree) {
	GraphvizWriter* graphviz_writer = (GraphvizWriter*) callback_arg;
	
	fprintf(graphviz_writer->stream, "digraph {\n");

	fprintf(graphviz_writer->stream, "subgraph cluster_source {\n");
	fprintf(graphviz_writer->stream, "label=\"Исходное выражение\";\n");	
	graphviz_writer_print_expression(graphviz_writer, tree->root);
	fprintf(graphviz_writer->stream, "}\n");
	
	graphviz_writer->step = 1;
}

void graphviz_writer_on_differentiation_started(void* callback_arg, const TreeNode* node) {
	(void) callback_arg;
	(void) node;
}

void graphviz_writer_on_differentiation_ended(void* callback_arg, const TreeNode* node, TreeNode** output_node) {
	GraphvizWriter* graphviz_writer = (GraphvizWriter*) callback_arg;
	
	fprintf(graphviz_writer->stream, "subgraph cluster_step%d {\n", graphviz_writer->step);
	fprintf(graphviz_writer->stream, "label = \"Шаг %d\";\n", graphviz_writer->step);
	
	// The only way I've found to make the subgraphs follow in chronological order is to reverse them in source code.
	
	fprintf(graphviz_writer->stream, "subgraph cluster_step%d_2 {\n", graphviz_writer->step);
	fprintf(graphviz_writer->stream, "label = \"После дифференцирования\";\n");
	graphviz_writer_print_expression(graphviz_writer, *output_node);
	fprintf(graphviz_writer->stream, "}\n");

	fprintf(graphviz_writer->stream, "subgraph cluster_step%d_1 {\n", graphviz_writer->step);
	fprintf(graphviz_writer->stream, "label = \"До дифференцирования\";\n");
	graphviz_writer_print_expression(graphviz_writer, node);
	fprintf(graphviz_writer->stream, "}\n");
	
	fprintf(graphviz_writer->stream, "}\n");
	
	
	graphviz_writer->step += 1;
}

void graphviz_writer_after_differentiation(void* callback_arg, const Tree* tree) {
	GraphvizWriter* graphviz_writer = (GraphvizWriter*) callback_arg;
	
	fprintf(graphviz_writer->stream, "subgraph cluster_derivative {\n");
	fprintf(graphviz_writer->stream, "label = \"Производная\";\n");	
	graphviz_writer_print_expression(graphviz_writer, tree->root);
	fprintf(graphviz_writer->stream, "}\n");
	
	fprintf(graphviz_writer->stream, "}\n");
}

void graphviz_writer_print_expression(GraphvizWriter* graphviz_writer, const TreeNode* node) {
	assert(node != NULL);

	switch (node->type) {
		case TREE_NODE_TYPE_OPERATION: {
			fprintf(graphviz_writer->stream, "node_%d_%p [shape=record,label=\" { %c | { <left> %p | <right> %p } } \" ];\n", graphviz_writer->step, (void*) node, node->operation, (void*) node->lhs, (void*) node->rhs);
			fprintf(graphviz_writer->stream, "node_%d_%p:<left> -> node_%d_%p [label=\"LHS\"]", graphviz_writer->step, (void*) node, graphviz_writer->step, (void*) node->lhs);
			fprintf(graphviz_writer->stream, "node_%d_%p:<right> -> node_%d_%p [label=\"RHS\"]", graphviz_writer->step, (void*) node, graphviz_writer->step, (void*) node->rhs);
			
			graphviz_writer_print_expression(graphviz_writer, node->lhs);
			graphviz_writer_print_expression(graphviz_writer, node->rhs);
			
			break;
		}
		
		case TREE_NODE_TYPE_FUNCTION: {
			fprintf(graphviz_writer->stream, "node_%d_%p [shape=record,label=\"{ %s | <inner> %p }\" ];\n", graphviz_writer->step, (void*) node, node->name, (void*) node->inner);
			fprintf(graphviz_writer->stream, "node_%d_%p:<inner> -> node_%d_%p [label=\"INNER\"]", graphviz_writer->step, (void*) node, graphviz_writer->step, (void*) node->inner);
			
			graphviz_writer_print_expression(graphviz_writer, node->inner);
			
			break;
		}
		
		case TREE_NODE_TYPE_VARIABLE: {
			fprintf(graphviz_writer->stream, "node_%d_%p [shape=record,label=\"{ %s }\" ];\n", graphviz_writer->step, (void*) node, node->name);
			
			break;
		}
		
		case TREE_NODE_TYPE_NUMBER: {
			fprintf(graphviz_writer->stream, "node_%d_%p [shape=record,label=\"{ %d }\" ];\n", graphviz_writer->step, (void*) node, node->number);
			
			break;
		}
		
		default: assert(false); UNREACHABLE;
	}
}

