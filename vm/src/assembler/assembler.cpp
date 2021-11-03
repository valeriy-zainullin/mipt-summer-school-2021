#define VM_ASSEMBLER_IMPL
#include "assembler.h"

#include "arch/arg_types.h"

#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <inttypes.h>
#include <ctype.h>

static bool assemble_operation(
	VmAssemblyStatus* status,
	FILE* output_stream,
	int32_t* ip,

	uint8_t command_number,
	uint8_t* allowed_arg_types,
	size_t num_allowed_arg_types,
	uint8_t arg_type,
	bool immediate_const_is_label,
	uint32_t register_index,
	int32_t immediate_const,
) {

	bool found_arg_type = false;
	for (size_t i = 0; i < num_allowed_arg_types; ++i) {
		if (allowed_arg_types[i] == arg_type) {
			found_arg_type = true;
			break;
		}
	}
	if (!found_arg_type) {
		status->error = VM_ASSEMBLY_ERROR_INVALID_ARG;
		return;
	}
	
	assert(command_number < (1 << 5));

	uint8_t operation = (arg_type << 5) | command_number;

	WRITE(operation);

	if (!vm_encode_arg(status, output_stream, arg_type, register_index, immediate_const, immediate_const_is_label, )) {

	}

	return true;
}

#undef WRITE

static bool process_operation(
	AssemblyStatus* status,
	unsigned char* line,
	size_t length,
	AssemblyLabel* labels,
	size_t* num_labels,
	int32_t* ip,
	FILE* output_stream
) {
	((void) length);
	((void) labels);
	((void) num_labels);
	((void) ip);

	char command[VM_ASSEMBLY_MAX_COMMAND_LENGTH + 1];

	int arg_start = 0;

	int num_read = sscanf((char*) line, COMMAND_NAME_SCANF_FORMAT "%n", command, &arg_start);
	if (num_read < 1) {
		return false;
	}

	uint8_t arg_type = 0;
	bool immediate_const_is_label = false;

	char register_name[2] = {};
	int32_t immediate_const = 0;
	char label[VM_ASSEMBLY_MAX_LABEL_LENGTH + 1] = {};

	bool read = false;

	if (!read) {
		return false;		
	}

	#define COMMAND(NAME, NUMBER, ALLOWED_ARG_TYPES)                    \
		if (strcmp(command, #NAME) == 0) {                              \
                                                                        \
			uint8_t command_number = NUMBER;                            \
			uint8_t allowed_arg_types[] = ALLOWED_ARG_TYPES;            \
                                                                        \
			assemble_operation(                                         \
				status,                                                 \
				command_number,                                         \
				allowed_arg_types,                                      \
				sizeof(allowed_arg_types) / sizeof(*allowed_arg_types), \
				arg_type,                                               \
				immediate_const_is_label,                               \
				register_name,                                          \
				immediate_const,                                        \
				ip,                                                     \
				output_stream                                           \
			);                                                          \
                                                                        \
			return true;                                                \
                                                                        \
		} else
	#include "arch/commands.h"
	#undef COMMAND
	{
		status->error = VM_ASSEMBLY_ERROR_INVALID_COMMAND;
		return true;
	}
}

static bool is_label_decl(unsigned char* string, size_t length) {
	return length >= 1 && string[length - 1] == ':';
}

// Max number of operations restriction.

static bool process_line(
	VmAssemblyStatus* status,
	FILE* output_stream,

	const unsigned char* line,
	size_t length,

	AssemblyLabels* labels,
	int32_t* ip
) {
	assert(line[length] == 0);

	if (process_label(status, output_stream, line, length, labels, ip)) {
		return true;
	}

	if (process_operation(status, output_stream, line, length, labels, ip)) {
		return true;
	}
	
	status->error = VM_ASSEMBLY_ERROR_INVALID_EXPRESSION;
	return false;
}

VmAssemblyStatus vm_assemble(TextLines* lines, FILE* output_stream) {
	VmAssemblyStatus status;
	status.line = 0;
	status.error = VM_ASSEMBLY_SUCCESS;

	VmAssemblyLabels labels = {};
	labels.nlabels = 0;

	int32_t ip = 0;

	for (size_t i = 0; i < lines->number_of_lines; ++i) {

		TextLine* text_line = &lines->lines[i];

		status.line = i;

		unsigned char* line = text_line->first_character;
		size_t length = (size_t) (text_line->after_the_last_character - text_line->first_character);

		while (length >= 1 && isspace(*line)) {
			++line;
			--length;
		}

		if (length == 0) {
			continue;
		}

		if (!process_line(&status, line, length, labels, &num_labels, &ip, output_stream)) {
			return status;
		}
	}

	if (ip == 0) {
		status.line = lines->number_of_lines;
		status.error = VM_ASSEMBLY_ERROR_EMPTY_PROGRAM;
		return status;
	}
	
	return status;
}
