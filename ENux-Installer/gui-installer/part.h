/*
 * part.h -- access partions
 */

typedef struct {
	int                major;
	int                minor;
	unsigned long long blocks;
	char               name[64];
} Part;

Part* getparts(size_t* count);
