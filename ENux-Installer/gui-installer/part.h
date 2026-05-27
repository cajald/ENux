/*
 * part.h -- access partions
 */

typedef struct {
	int                major;
	int                minor;
	unsigned long long blocks;
	char               name[64];
} Part;

enum fstype {
	EFST_EXT4,
	EFST_XFS,
	EFST_ZFS,
	EFST_BTRFS,
	EFST_SWAP,
};

Part* getparts(size_t* count);
void makeFs(enum fstype ft, const char* part);

