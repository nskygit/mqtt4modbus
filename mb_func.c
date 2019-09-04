/* mqtt4modbus
 * modbus tcp/rtu via mqtt
 * author: daixijiang@gmail.com
 */
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <modbus.h>
#include "common.h"

typedef struct
{
    int mb_type;      /* 0, rtu; 1, tcp */
    char address[64];   /* ttySN or IP */
    unsigned short port;
    modbus_t *ctx;

} mbslave_t;

int g_slave_id = SLAVE_ID_DEFAULT;

mbslave_t g_mbx[SLAVE_ID_MAX] = 
{
    [1] = {.mb_type = MODBUS_TYPE_RTU, .address = MODBUS_DEVICE},
    [17] = {.mb_type = MODBUS_TYPE_TCP, .address = "192.168.153.38", .port = MODBUS_PORT},

};

//const 
plc_output_t g_plc_output[PLC_ID_MAX] =
{
#if 0
    { .identifier = "y402", .address = 0x2081, .label = "SV 1NP SCHODY" },
    { .identifier = "y401", .address = 0x2080, .label = "SV 2NP KUCHYN LINKA" },
    { .identifier = "y403", .address = 0x2082, .label = "SV 1NP OBYVAK KRB STR" },
    { .identifier = "y404", .address = 0x2083, .label = "SV 1NP OBYVAK KRB KRJ" },
    { .identifier = "y405", .address = 0x2084, .label = "SV 2NP LOZNICE KRJ" },
    { .identifier = "y406", .address = 0x2085, .label = "SV 2NP LOZNICE STR" },
    { .identifier = "y407", .address = 0x2086, .label = "SV 1NP WC" },
    { .identifier = "y409", .address = 0x2088, .label = "SV 0NP KOMORA KRJ" },
    { .identifier = "y410", .address = 0x2089, .label = "SV 0NP KOMORA STR" },
    { .identifier = "y411", .address = 0x208a, .label = "SV 0NP GARAZ 1" },
    { .identifier = "y412", .address = 0x208b, .label = "SV 0NP VRATA" },
    { .identifier = "y413", .address = 0x208c, .label = "SV 0NP GARAZ 2" },
    { .identifier = "y414", .address = 0x208d, .label = "SV 0NP GARAZ 3" },
    { .identifier = "y415", .address = 0x208e, .label = "SV 0NP SCHODY" },
    { .identifier = "y416", .address = 0x208f, .label = "SV 0NP CHODBA" },
    
    { .identifier = "y501", .address = 0x20a0, .label = "SV 1NP VCHOD" },
    { .identifier = "y503", .address = 0x20a2, .label = "SV 1NP PRISTAVBA" },
    { .identifier = "y505", .address = 0x20a4, .label = "SV 0NP SPIZIRNA STR" },
    { .identifier = "y506", .address = 0x20a5, .label = "SV 0NP SPIZIRNA KRJ" },
    { .identifier = "y509", .address = 0x20a8, .label = "SV 2NP LOZNICE BOD L" },
    { .identifier = "y510", .address = 0x20a9, .label = "SV 2NP LOZNICE BOD P" },
    { .identifier = "y511", .address = 0x20aa, .label = "SV 1NP KOUPELNA ZRCADLO" },
    { .identifier = "y512", .address = 0x20ab, .label = "ZA 1NP KOUPELNA ZRCADLO" },
    { .identifier = "y513", .address = 0x20ac, .label = "SV 2NP KOUPELNA" },
    { .identifier = "y514", .address = 0x20ad, .label = "SV 2NP KOUPELNA SPRCHA" },
    { .identifier = "y515", .address = 0x20ae, .label = "SV 2NP JIDELNA KRJ" },
    { .identifier = "y516", .address = 0x20af, .label = "SV 2NP JIDELNA STR" },
    
    { .identifier = "y603", .address = 0x20c2, .label = "SV 2NP CHODBA" },
    { .identifier = "y605", .address = 0x20c4, .label = "SV 2NP KUCHYN STRED" },
    { .identifier = "y606", .address = 0x20c5, .label = "SV 2NP KUCHYN BAR" },
    { .identifier = "y607", .address = 0x20c6, .label = "SV 2NP OBYVAK TV KRJ" },
    { .identifier = "y608", .address = 0x20c7, .label = "SV 2NP OBYVAK TV STR" },
    { .identifier = "y609", .address = 0x20c8, .label = "SV 1NP JIDELNA KRJ" },
    { .identifier = "y610", .address = 0x20c9, .label = "SV 1NP JIDELNA STR" },
    { .identifier = "y611", .address = 0x20ca, .label = "SV 1NP KOUPELNA BOD" },
    { .identifier = "y613", .address = 0x20cc, .label = "SV 1NP KUCHYN LINKA" },
    { .identifier = "y614", .address = 0x20cd, .label = "SV 1NP KUCHYN TROUBA" },
    { .identifier = "y615", .address = 0x20ce, .label = "SV 1NP KUCHYN ZARIVKA L" },
    { .identifier = "y616", .address = 0x20cf, .label = "SV 1NP KUCHYN ZARIVKA P" },
    
    { .identifier = "y701", .address = 0x20e0, .label = "SV 2NP OBYVAK STRED KRJ" },
    { .identifier = "y702", .address = 0x20e1, .label = "SV 2NP OBYVAK STRED STR" },
    { .identifier = "y703", .address = 0x20e2, .label = "SV 1NP OBYVAK OKNO STR" },
    { .identifier = "y704", .address = 0x20e3, .label = "SV 1NP OBYVAK OKNO KRJ" },
    { .identifier = "y705", .address = 0x20e4, .label = "SV 1NP CHODBA PRISTAVBA" },
    { .identifier = "y706", .address = 0x20e5, .label = "SV 1NP CHODBA KOUPELNA" },
    { .identifier = "y707", .address = 0x20e6, .label = "SV 2NP KOUPELNA ZRCADLO" },
    { .identifier = "y709", .address = 0x20e8, .label = "SV 2NP PRACOVNA KRJ" },
    { .identifier = "y710", .address = 0x20e9, .label = "SV 2NP PRACOVNA STR" },
    { .identifier = "y711", .address = 0x20ea, .label = "SV 2NP KUCHYN SOKL" },
    { .identifier = "y712", .address = 0x20eb, .label = "SV 2NP KUCHYN SIKMINA" },
    { .identifier = "y713", .address = 0x20ec, .label = "SV 1NP LOZNICE STR" },
    { .identifier = "y714", .address = 0x20ed, .label = "SV 1NP LOZNICE KRJ" },
    { .identifier = "y715", .address = 0x20ee, .label = "SV 1NP TECH MISTNOST" },
    
    { .identifier = "y801", .address = 0x2100, .label = "SV 3NP POKOJ 1 JIH" },
    { .identifier = "y802", .address = 0x2101, .label = "SV 3NP POKOJ 1 SEVER" },
    { .identifier = "y803", .address = 0x2102, .label = "SV 3NP CHODBA ZAPAD" },
    { .identifier = "y804", .address = 0x2103, .label = "SV 2NP SCHODY" },
    { .identifier = "y805", .address = 0x2104, .label = "SV 3NP CHODBA VYCHOD" },
    { .identifier = "y807", .address = 0x2106, .label = "SV 3NP POKOJ 2 JIH" },
    { .identifier = "y808", .address = 0x2107, .label = "SV 3NP POKOJ 2 SEVER" },

    { .identifier = "c101", .address = 0x4064, .label = "SV DUM SV" },
    { .identifier = "c102", .address = 0x4065, .label = "SV DUM JV" },
    { .identifier = "c103", .address = 0x4066, .label = "SV DUM JZ" },
    { .identifier = "c104", .address = 0x4067, .label = "SV DUM SZ" },
    { .identifier = "c105", .address = 0x4068, .label = "SV PARKING" },
    { .identifier = "c106", .address = 0x4069, .label = "SV ZAHR J" },
    { .identifier = "c107", .address = 0x406a, .label = "SV RODODEN" },
    { .identifier = "c108", .address = 0x406b, .label = "SV GARAZ" },
    { .identifier = "c109", .address = 0x406c, .label = "SV CHALOUP" },
    { .identifier = "c110", .address = 0x406d, .label = "SV STUDNA" },
    { .identifier = "c111", .address = 0x406e, .label = "SV ZAHR S" },

    { .identifier = "c200", .address = 0x40c7, .label = "MB LATCH" },
    { .identifier = "c201", .address = 0x40c8, .label = "MB SENDING" },
    { .identifier = "c202", .address = 0x40c9, .label = "MB SUCCESS" },
    { .identifier = "c203", .address = 0x40ca, .label = "MB ERROR" },
#endif

    { .identifier = "", .address = 0, .label = "" }
};

void mb_connect(int type, char *address, int port, unsigned char slaveid)
{
    mbslave_t *mbp = &g_mbx[slaveid];

    if (mbp->ctx) {
        modbus_close(mbp->ctx);
        modbus_free(mbp->ctx);
    }

    if (MODBUS_TYPE_RTU == type) {
        mbp->ctx = modbus_new_rtu(address, 38400, 'O', 8, 1);
        mbp->mb_type = type;
        bzero(mbp->address, sizeof(mbp->address));
        strncpy(mbp->address, address, sizeof(mbp->address)-1);
    } else if (MODBUS_TYPE_TCP == type) {
        mbp->ctx = modbus_new_tcp(address, port);
        mbp->mb_type = type;
        mbp->port = port;
        bzero(mbp->address, sizeof(mbp->address));
        strncpy(mbp->address, address, sizeof(mbp->address)-1);
    }

    if (mbp->ctx == NULL)
    {
        fprintf(stderr, "modbus_new_rtu(%d): Call failed\n", type);
        //exit(EXIT_FAILURE);
    }

    if (MODBUS_TYPE_TCP == type)
    {
        modbus_set_response_timeout (mbp->ctx, 0, 200000);
    }

    if (modbus_set_slave(mbp->ctx, slaveid) == -1)
    {
        fprintf(stderr, "modbus_set_slave: %s on slave %d\n", modbus_strerror(errno), slaveid);
        //exit(EXIT_FAILURE);
    }

    if (modbus_connect(mbp->ctx) == -1)
    {
        fprintf(stderr, "modbus_connect: %s on slave %d \n", modbus_strerror(errno), slaveid);
        //exit(EXIT_FAILURE);
    }
}

void mb_reconnect(unsigned char slaveid)
{
    mbslave_t *mbp = &g_mbx[slaveid];
    mb_connect(mbp->mb_type, mbp->address, mbp->port, slaveid);
}

void mb_cleanup(void)
{
    int index = 0;
    for (index = 0; index < SLAVE_ID_MAX; index++)
    {
        if (g_mbx[index].ctx != NULL)
        {
            modbus_close(g_mbx[index].ctx);
            modbus_free(g_mbx[index].ctx);
        }
    }

}

void mb_write_bit(unsigned char slaveid, int address, bool state)
{
    mbslave_t *mbp = &g_mbx[slaveid];

    if (mbp->ctx && (modbus_write_bit(mbp->ctx, address, state) == -1))
    {
        fprintf(stderr, "modbus_write_bit: %s(%d) on slave %d \n", modbus_strerror(errno), errno, slaveid);
        //exit(EXIT_FAILURE);

        if (MODBUS_ENOBASE > errno) {
            mb_reconnect(slaveid);
            if (mbp->ctx && (modbus_write_bit(mbp->ctx, address, state) == -1))
            {
                fprintf(stderr, "modbus_write_bit2: %s(%d) on slave %d \n", modbus_strerror(errno), errno, slaveid);
            }
        }
    }
}

void mb_read_bit(unsigned char slaveid, int address, bool *state)
{
    uint8_t buf;
    mbslave_t *mbp = &g_mbx[slaveid];

    if (mbp->ctx && (modbus_read_bits(mbp->ctx, address, 1, &buf) == -1))
    {
        fprintf(stderr, "modbus_read_bits: %s(%d) on slave %d \n", modbus_strerror(errno), errno, slaveid);
        //exit(EXIT_FAILURE);

        if (MODBUS_ENOBASE > errno) {
            mb_reconnect(slaveid);
            if (mbp->ctx && (modbus_read_bits(mbp->ctx, address, 1, &buf) == -1))
            {
                fprintf(stderr, "modbus_read_bits2: %s(%d) on slave %d \n", modbus_strerror(errno), errno, slaveid);
                //exit(EXIT_FAILURE);
            }
        }
    }

    *state = buf;
}

void mb_init(void)
{
    int i;

    modbus_read_plc(MODBUS_PLC_CFG);
    for (i = 0; g_plc_output[i].identifier[0]; i++)
    {
        if (0 == g_plc_output[i].slaveid)
            g_plc_output[i].slaveid = g_slave_id;
    }

#if 1
    fprintf(stderr, "---------------------------------\n");
    fprintf(stderr, "identifier address label slaveid\n");
    for (i = 0; g_plc_output[i].identifier[0]; i++)
    {
        fprintf(stderr, "%s 0x%x \"%s\" %d\n", 
		g_plc_output[i].identifier, g_plc_output[i].address,
		g_plc_output[i].label, g_plc_output[i].slaveid);
    }
    fprintf(stderr, "---------------------------------\n");
#endif

    //mb_connect(MODBUS_TYPE_RTU, MODBUS_DEVICE, 0, 1);
    //mb_connect(MODBUS_TYPE_TCP, "192.168.153.38", MODBUS_PORT, g_slave_id);
    mb_reconnect(g_slave_id);
}

