#include "cli.h"
#include "support/debug_utils.h"
#include "vm/vm.h"

#include "text.h"

#include <stdio.h>

int main(int argc, char** argv) {
	if (argc != 2) {
		return 1;
	}
	const char* input_file = argv[1];

	FILE* input_stream = fopen(input_file, "rb");
	if (input_stream == NULL) {
		return 4;
	}

	vm_execute(input_stream, stdin, stdout);

	// return (int) CLI_EXIT_CODE_SUCCESS;
	return 0;
}