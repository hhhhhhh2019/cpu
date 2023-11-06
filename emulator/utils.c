#include <utils.h>

#include <string.h>
#include <stdlib.h>


unsigned long size2int(char* str) {
	char last_char = str[strlen(str)-1];

	if (last_char < '0' || last_char > '9') {
		str[strlen(str)-1] = 0;
	}

	unsigned long result = atol(str);

	if (last_char == 'K')
		result *= 1024;
	if (last_char == 'M')
		result *= 1024 * 1024;
	if (last_char == 'G')
		result *= 1024 * 1024 * 1024;

	return result;
}



void queue_init(Queue* queue, unsigned long size) {
	queue->max_queue_size = size;
	queue->head = 0;
	queue->tail = 0;
	queue->queue = malloc(sizeof(int) * size);
}


void queue_push(Queue* queue, int value) {
	queue->queue[queue->head] = value;
	queue->head = (queue->head + 1) % queue->max_queue_size;
}


int queue_pop(Queue* queue) {
	int result = queue->queue[queue->tail];
	queue->tail = (queue->tail + 1) % queue->max_queue_size;
	return result;
}


char queue_can_pop(Queue* queue) {
	return queue->tail != queue->head;
}
