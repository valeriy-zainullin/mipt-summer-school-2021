#include "vm/state.h"

#include "arg_type.h"
#include "bytecode/operation.h"
#include "status.h"
#include "support/forward_stream.h"

#include <stdbool.h>
#include <stdint.h>

bool vm_execute_operation(VmStatus* status, VmState* state, const VmOperation* operation, void* arg) {
	int32_t* argument_memory = nullptr;
	int32_t argument = 0;

	if ((operation->arg_type & VM_COMMAND_ARG_USES_REGISTER) != 0) {
		uint8_t register_index = operation->register_index;
		if (register_index >= VM_MACHINE_NUM_REGISTERS) {
			return false;
		}
		argument = state->registers[register_index];
		argument_memory = &state->registers[register_index];
	}
	if ((operation->arg_type & VM_COMMAND_ARG_USES_IMMEDIATE_CONST) != 0) {
		int32_t immediate_const = operation->immediate_const;
		argument += immediate_const;
		argument_memory = NULL;
	}
	if ((operation->arg_type & VM_COMMAND_ARG_USES_MEMORY) != 0) {
		if (argument_memory == NULL) {
			argument_memory = (int32_t*) &state->memory[argument];
			argument = *argument_memory;
		}
	}

	#define ARGUMENT_MEMORY argument_memory
	#define ARGUMENT argument
	#define MEMORY(ADDRESS) (int32_t*) &state->memory[ADDRESS]
	#define REGISTER(INDEX) (int32_t*) &state->registers[INDEX]
	#define STACK_POP(VARIABLE_PTR) stack_int_pop(&state->stack, VARIABLE_PTR) // VERIFY!
	#define STACK_PUSH(VALUE) stack_int_push(&state->stack, VALUE) // VERIFY!
	#define OPERAND(NAME) int32_t NAME = 0; STACK_POP(&NAME)
	#define SET_IP(VALUE) state->ip = VALUE
	#define SEND_INT(VALUE) vm_on_send_int(status, arg, VALUE)
	#define SEND_BYTE(VALUE) vm_on_send_byte(status, arg, (uint8_t) VALUE)
	#define HALT() *status = VM_STATUS_HALT_REQUESTED; return true
	#define COMMAND(NAME, INDEX, ALLOWED_ARG_TYPES, EXECUTION_CODE, ...) \
		if (operation->command_index == INDEX) {                            \
			EXECUTION_CODE                                                   \
		} else
	#include "commands.h"
	#undef COMMAND
	{
		return false;
	}

	return true;
}
