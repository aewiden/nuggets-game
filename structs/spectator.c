/*
 * Spectator data struct
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "mem.h"
#include "message.h"

typedef struct spectator {
    addr_t* address;
} spectator_t;


spectator_t* spectator_new(addr_t* address) {
    spectator_t* spectator = mem_malloc(sizeof(spectator_t));
    spectator->address = address;
    return spectator;
}

addr_t* spectator_getAddress(spectator_t* spectator) {
    if (spectator!=NULL) {
        return spectator->address;
    }
    return NULL;
}

void spectator_delete(spectator_t* spectator) {
    if (spectator!=NULL) {
        mem_free(spectator->address);
        mem_free(spectator);
    }
}