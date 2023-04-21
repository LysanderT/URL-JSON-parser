#include "parse.h"
#include "third_party/cJSON/cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char*
file_get_contents(const char* path)
{
	FILE* file;
	long size;
	char* buffer;

	file = fopen(path, "r");
    if(file == NULL){
        perror("fopen failed \n");
    }
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	buffer = malloc(size + 1);
	fread(buffer, 1, size, file);
	fclose(file);
	return buffer;
}

static int
cJSON_GetObjectSize(const cJSON* object)
{
	int size = 0;

	for (const cJSON* child = object->child; child; child = child->next) {
		size++;
	}
	return size;
}

#define return_error() do {					\
	printf("ERROR: file %s line %d\n", __FILE__, __LINE__);	\
	cJSON_Delete(json);					\
	return 1;						\
} while (0)

int main(void)
{
	char* json_string;
	cJSON* json;
	const cJSON* testcases;
	const cJSON* testcase;

	json_string = file_get_contents("../testcases.json");
	if (!json_string) {
		return 1;
	}

	json = cJSON_Parse(json_string);
	free(json_string);

    if(!json){ // ADDED. maybe failed to parse.
        return_error();
    }

	if (!cJSON_IsObject(json)) {
		return_error();
	}

	testcases = cJSON_GetObjectItem(json, "testcases");
	if (!cJSON_IsArray(testcases)) {
		return_error();
	}
	cJSON_ArrayForEach (testcase, testcases) {
		const cJSON* input;
		const cJSON* output;
		char* url;
		struct query_parameter* params = NULL; // CHANGED. pointer should be initialized!!!
		int count;

		input = cJSON_GetObjectItem(testcase, "input");
		output = cJSON_GetObjectItem(testcase, "output");
		url = strdup(input->valuestring);

        if (!url) { // ADDED
            return_error();
        }

		count = parse_query_string(url, &params); // params has been modified in this line

        if(count < 0){ // check if parse.c goes well
            free(url);
            return_error();
        }

		printf("\ncount %d\n", count);
		if (count != cJSON_GetObjectSize(output)) {
            free(url);
            free(params);
            return_error();
		}
		for (int i = 0; i < count; i++) {
			const cJSON* json_value;

			printf("[%s] = %s\n", params[i].name, params[i].value);
			json_value = cJSON_GetObjectItem(output, params[i].name);
			if (!cJSON_IsString(json_value)) {
                free(url);
                free(params);
				return_error();
			}
			if (strcmp(json_value->valuestring, params[i].value) != 0) {
                free(url);
                free(params);
				return_error();
			}
		}
		printf("OK\n\n");
        free(url);
        free(params);
	}
	cJSON_Delete(json);
	return 0;
}
