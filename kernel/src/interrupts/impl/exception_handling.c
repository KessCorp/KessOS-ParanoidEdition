#include <debug/log.h>
#include <stdint.h>

#define BIT_P   (1 << 0)
#define BIT_W   (1 << 1)
#define BIT_U   (1 << 2)
#define BIT_R   (1 << 3)
#define BIT_I   (1 << 4)
#define BIT_PK  (1 << 5)
#define BIT_SS  (1 << 6)


void get_page_fault_code(uint16_t error_code) {
    log("\nBelow are error flags, take a photo of this screen and send it to [HSC] Spurious#0070 on Discord.\n\n", -1);
    log("-|", -1);

    if (error_code & BIT_P)
        log("P|", -1);

    if (error_code & BIT_W)
        log ("W|", -1);

    if (error_code & BIT_U)
        log("U|", -1);

    if (error_code & BIT_R)
        log("R|", -1);

    if (error_code & BIT_I)
        log("I|", -1);

    if (error_code & BIT_PK)
        log("PK|", -1);

    if (error_code & BIT_SS)
        log("SS|", -1);

    log("-\n\n", -1);

}
