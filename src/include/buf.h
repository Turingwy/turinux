struct buf {
    uint32_t flags;
    uint32_t dev;
    uint32_t sector;
    struct buf *prev;
    struct buf *next;
    struct buf *qnext;
    uint8_t data[512];
}

#define B_BUSY 0x1
#define B_VALID 0x2
#define B_DIRTY 0x4
