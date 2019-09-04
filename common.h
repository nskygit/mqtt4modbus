#ifndef LIBCOMMON_H__
#define LIBCOMMON_H__

#define MODBUS_CFG 		"modbus.csv"
#define MODBUS_PLC_CFG 		"modbus_plc.csv"

#define MQTT_PRODUCT		"plc/"
#define MQTT_DEVNAME		"mqtt4modbus/"
#define MQTT_PREFIX		MQTT_PRODUCT MQTT_DEVNAME
#define MQTT_MY_HELLO		"hello"
#define MQTT_MY_OPER		"oper/"
#define MQTT_MY_OPER_PUT	"put"
#define MQTT_MY_OPER_GET	"get"
#define MQTT_MY_OPER_SAVE	"save"

#define MQTT_MY_PLC_GET		"/get"
#define MQTT_MY_PLC_SET		"/set"
#define MQTT_MY_PLC_TOG		"/toggle"
#define MQTT_MY_PLC_OK		"/ok"

#define MQTT_HOST		"localhost"
#define MQTT_PORT		1883
#define MQTT_KEEPALIVE		60

#define MODBUS_DEVICE		"/dev/ttyS0"
#define MODBUS_PORT		502
#define PLC_ID_MAX		100
#define SLAVE_ID_MAX		256
#define SLAVE_ID_DEFAULT	17

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

enum {
	CSV_PLC_FIELD_NAME = 0,
	CSV_PLC_FIELD_ADDR,
	CSV_PLC_FIELD_LABEL,
	CSV_PLC_FIELD_SLAVEID,
	CSV_PLC_FIELD_MAX
};

enum {
    MODBUS_TYPE_RTU = 0,
    MODBUS_TYPE_TCP = 1,
};

typedef struct
{
    char identifier[32];
    int slaveid;
    int address;
    char label[32];
    bool status;

} plc_output_t;

struct parse_ctx {
	unsigned int nr_rows;
	unsigned int nr_fields;
	unsigned int err_row;
	unsigned int err_field;
	int not_title_row;
	int error;
};

struct csv_plc_pctx {
	struct parse_ctx ctx; /* must be first member */
	plc_output_t entry;
};

extern void mb_init(void);
extern void mb_cleanup(void);
extern int modbus_read_plc(const char *file_path);
extern int modbus_save_plc(char *file_path);
extern void mb_read_bit(unsigned char slaveid, int address, bool *state);
extern void mb_write_bit(unsigned char slaveid, int address, bool state);
extern int modbus_plc_get(char *payload, char *msg, int msg_len);
extern int modbus_plc_put(char *payload, char *msg, int msg_len);

#endif

