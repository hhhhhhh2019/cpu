#ifndef UTILS_H
#define UTILS_H


#ifdef DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif


unsigned long size2int(char*);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ 

static inline unsigned char cpu2lt8(unsigned char n) {
	return (n);
}
static inline unsigned short cpu2lt16(unsigned short n) {
	return (n);
}
static inline unsigned int cpu2lt32(unsigned int n) {
	return (n);
}
static inline unsigned long cpu2lt64(unsigned long n) {
	return (n);
}

#else

static inline unsigned char cpu2lt8(unsigned char n) {
	return (n);
}
static inline unsigned short cpu2lt16(unsigned short n) {
	return
		((n & 0x00ff) << 8) |
		((n & 0xff00) >> 8);
}
static inline unsigned int cpu2lt32(unsigned int n) {
	return
		(n & 0x000000ff << 24) |
		(n & 0x0000ff00 << 8)  |
		(n & 0x00ff0000 >> 8)  |
		(n & 0xff000000 >> 24);
}
static inline unsigned long cpu2lt64(unsigned long n) {
	return
		(n & 0x00000000000000ff << 56) |
		(n & 0x000000000000ff00 << 40) |
		(n & 0x0000000000ff0000 << 24) |
		(n & 0x00000000ff000000 <<  8) |
		(n & 0x000000ff00000000 >>  8) |
		(n & 0x0000ff0000000000 >> 24) |
		(n & 0x00ff000000000000 >> 40) |
		(n & 0xff00000000000000 >> 56);
}

#endif // __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__



typedef struct Queue {
	unsigned long max_queue_size;
	int* queue;
	unsigned long head;
	unsigned long tail;
} Queue;

void queue_init(Queue*, unsigned long size);
void queue_push(Queue*, int value);
int  queue_pop(Queue*);
char queue_can_pop(Queue*);


#endif
