
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "parse.h"

int parse_query_string(char* url, struct query_parameter** out) {
    /* TODO */
    if (url == NULL) {
        return -EINVAL;
    }

    char* URL = strdup(url); // an original copy
    char* query = strchr(URL, '?'); // find "?"
    if (query == NULL) {
        *out = NULL;
        return 0;
    }

    query++; // skip "?"

    // Calculate the return value, i.e. the "count" field
    int count = 1;
    char* p = query;
    while (*p != '\0') {
        if (*p == '&') {
            count++;
        }
        p++;
    }


    struct query_parameter* params = malloc(sizeof(struct query_parameter) * count);
    if (params == NULL) {
        return -ENOMEM;
    }



    p = query;
    int i = 0;
    while (*p != '\0') {
        char* eq = strchr(p, '=');
        if (eq == NULL) {
            free(params);
            return -EINVAL;
        }

        *eq = '\0'; // Null-terminate the field "name"
        params[i].name = p;
        params[i].value = eq + 1;

        p = eq + 1;
        eq = strchr(p, '&');
        if (eq != NULL) {
            *eq = '\0'; // Null-terminate the field "value"
            p = eq + 1;
        }else{
            break;
        }
        i++;
    }

    *out = params;
    return count;
}