#include <map.h>
#include <utils.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void map(Compiler_state* state) {
	FILE* f = fopen(state->map_filename ,"wb");

	if (f == NULL) {
		ERROR("Cannot open file \"%s\": %s\n", state->output_filename, strerror(errno));
#ifdef DEBUG
		perror("fopen " STR(__FILE__) " " STR(__LINE__));
#endif
		exit(errno);
	}

	for (int i = 0; i < state->semant_result.labels_count; i++) {
		Label label = state->semant_result.labels[i];

		fprintf(f, "%s: %016lx\n", label.name, label.offset);
	}

	fclose(f);
}
