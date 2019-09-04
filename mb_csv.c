/* mqtt4modbus
 * modbus tcp/rtu via mqtt
 * author: daixijiang@gmail.com
 */
#include <stdbool.h>
#include <string.h>

#include "common.h"
#include "csv/csv.h"
#include "cJson/cJSON.h"

extern plc_output_t g_plc_output[];
extern int json_get_int(cJSON *inJson, const char *key);
extern char *json_get_char(cJSON *inJson, const char *key);

/* cvs functions */
static inline int put_comma(FILE *fp)
{
	int rc;

	rc = putc(CSV_COMMA, fp);
	if (rc == EOF) {
		fprintf(stderr, "Failed to put char '%c'", CSV_COMMA);
		return -1;
	}

	return 0;
}

static inline int put_newline(FILE *fp)
{
	int rc;

	rc = putc(CSV_CR, fp);
	if (rc == EOF) {
		fprintf(stderr, "Failed to put char '%c'", CSV_CR);
		return -1;
	}

	rc = putc(CSV_LF, fp);
	if (rc == EOF) {
		fprintf(stderr, "Failed to put char '%c'", CSV_LF);
		return -1;
	}

	return 0;
}

static inline int put_string(FILE *fp, const char *value)
{
	int rc;

	rc = csv_fwrite(fp, value, strlen(value));
	if (rc == EOF) {
		fprintf(stderr, "Failed to file write %s", value);
		return -1;
	}

	rc = put_comma(fp);
	if (rc)
		return rc;

	return 0;
}

static inline const char *utoa_dec(unsigned long num)
{
	static char s[32];

	bzero(s, sizeof(s));
	snprintf(s, sizeof(s), "%lu", num);
	return s;
}

static inline const char *utoa_hex(unsigned long num)
{
	static char s[32];

	bzero(s, sizeof(s));
	snprintf(s, sizeof(s), "0x%x", (unsigned int)num);
	return s;
}

static inline int put_uint_dec(FILE *fp, unsigned int i)
{
	const char *value;

	value = utoa_dec(i);
	return put_string(fp, value);
}

static inline int put_uint_hex(FILE *fp, unsigned int i)
{
	const char *value;

	value = utoa_hex(i);
	return put_string(fp, value);
}

static inline int is_null_value(const void *s, size_t len)
{
	return (!s || !len);
}

static inline void get_string(char *dstr, size_t dlen, const void *s, size_t slen)
{
	size_t len = MIN(slen, dlen - 1);

	if (is_null_value(s, len)) {
		dstr[0] = 0;
		return;
	}

	memcpy(dstr, s, len);
	dstr[len] = 0;
}

static inline unsigned int get_uint(const void *s, size_t len)
{
	char str[len + 1];

	if (is_null_value(s, len))
		return 0;

	memcpy(str, s, len);
	str[len] = 0;
	return atoi(str);
}

static inline int char2int(char hex) 
{
    if (hex>='0' && hex <='9')
        return hex - '0';
    if (hex>='A' && hex <= 'F')
        return hex-'A'+10;
    if(hex>='a' && hex <= 'f')
        return hex-'a'+10;
    return 0;
}

static inline int hex2int(char *hex, int len) 
{
    if ((hex[0] == '0') && (hex[1] == 'x') ) {
        if (3 == len)
            return char2int(hex[2]);
        else if (4 == len)
            return (char2int(hex[2]) << 4) + char2int(hex[3]);
        else if (5 == len)
            return (char2int(hex[2]) << 8) + (char2int(hex[3]) << 4) 
                    + char2int(hex[4]);
        else if (6 == len)
            return (char2int(hex[2])<< 12) + (char2int(hex[3]) << 8)
                    + (char2int(hex[4]) << 4) + char2int(hex[5]) ;	
    }
    return 0;
}

static void csv_plc_add_cb1(void *s, size_t len, void *data)
{
	struct csv_plc_pctx *pctx = (struct csv_plc_pctx *)data;
	plc_output_t *plc_ctx = NULL;
	char value[64];
	pctx->ctx.nr_fields++;
	
	get_string(value, sizeof(value), s, len);
	//fprintf(stderr, "cb1 col(%d) row(%d) %s(%d)\n", pctx->ctx.nr_fields, pctx->ctx.nr_rows, value, (int)len);

	if (0 == pctx->ctx.nr_rows)
		return;
	plc_ctx = &g_plc_output[pctx->ctx.nr_rows-1];

	switch (pctx->ctx.nr_fields - 1) {
	case CSV_PLC_FIELD_NAME:
		if (!is_null_value(value, len)){
			memcpy(plc_ctx->identifier, value, sizeof(plc_ctx->identifier)-1); 
			plc_ctx->identifier[sizeof(plc_ctx->identifier)-1] = 0;
		}
		break;
	case CSV_PLC_FIELD_ADDR:
		if (!is_null_value(value, len)){
			plc_ctx->address = hex2int(value, len);
		}
		break;
	case CSV_PLC_FIELD_LABEL:
		if (!is_null_value(value, len)){
			memcpy(plc_ctx->label, value, sizeof(plc_ctx->label)-1); 
			plc_ctx->label[sizeof(plc_ctx->label)-1] = 0;
		}
		break;
	case CSV_PLC_FIELD_SLAVEID:
		if (!is_null_value(value, len)){
			plc_ctx->slaveid = atoi(value);
		}
		break;
	default:
		break;
	}

}

static void csv_plc_add_cb2(int c, void *data)
{
	struct csv_plc_pctx *pctx = (struct csv_plc_pctx *)data;
	pctx->ctx.nr_rows++;
	pctx->ctx.nr_fields = 0;
}

int modbus_read_plc(const char *file_path)
{
	int rc, ret = 0;
	FILE *fp;
	struct csv_parser parser;
	struct csv_plc_pctx pctx;
	char buf[10240];
	size_t n;

	bzero(&pctx, sizeof(pctx));
	rc = csv_init(&parser, 0);
	if (rc) {
		fprintf(stderr, "Failed to initialize csv parser");
		return -1;
	}

	fp = fopen(file_path, "rb");
	if (!fp) {
		fprintf(stderr, "Failed to read open file %s", file_path);
		return -1;
	}

	while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
		//fprintf(stderr, "%s\n", buf);		
		if (csv_parse(&parser, buf, n, csv_plc_add_cb1, csv_plc_add_cb2, &pctx) != n) {
			fprintf(stderr, "Error while parsing csv file: %s", 
				csv_strerror(csv_error(&parser)));
			goto eout;
		}
	}

	rc = csv_fini(&parser, csv_plc_add_cb1, csv_plc_add_cb2, &pctx);
	if (rc) {
		fprintf(stderr, "Error while parsing csv file: %s", 
			csv_strerror(csv_error(&parser)));
		goto eout;
	}

out:
	csv_free(&parser);
	fclose(fp);
	return ret;
eout:
	ret = -1;
	goto out;
}

int modbus_save_plc(char *file_path)
{
    FILE *fp;

    fp = fopen(file_path, "wb");
    if (!fp) {
        fprintf(stderr, "Failed to write open file %s", file_path);
        return -1;
    }

    /* put title */
    put_string(fp, "identifier");
    put_string(fp, "address");
    put_string(fp, "label");
    put_string(fp, "slaveid");
    put_newline(fp);

    /* put record */
    for (size_t i = 0; g_plc_output[i].identifier[0]; i++)
    {
        put_string(fp, g_plc_output[i].identifier);
        put_uint_hex(fp, g_plc_output[i].address);
        put_string(fp, g_plc_output[i].label);
        put_uint_dec(fp, g_plc_output[i].slaveid);
        put_newline(fp);
    }

    fclose(fp);
    return 0;
}

int modbus_plc_get(char *payload, char *msg, int msg_len)
{
    int found = 0;
    cJSON *resp_root = NULL;
    cJSON *req_json_obj = NULL;
    cJSON *array = NULL, *obj = NULL;
    char *cj_identifier = NULL;	
	
    //fprintf(stderr, "modbus_plc_get %s", payload);

    req_json_obj = cJSON_Parse(payload);
    resp_root = cJSON_CreateObject();

    if (req_json_obj)
        cj_identifier = json_get_char(req_json_obj, "identifier");

    if (cj_identifier) {
        if (strcmp(cj_identifier, "all") == 0) {
            cJSON_AddNumberToObject(resp_root, "code", 0);
            cJSON_AddItemToObject(resp_root,"all",array = cJSON_CreateArray()); 
            for (size_t i = 0; g_plc_output[i].identifier[0]; i++) {
                cJSON_AddItemToArray(array, obj = cJSON_CreateObject());
                cJSON_AddStringToObject(obj, "identifier", g_plc_output[i].identifier);
                cJSON_AddStringToObject(obj, "address", utoa_hex(g_plc_output[i].address));
                cJSON_AddStringToObject(obj, "label", g_plc_output[i].label);
                cJSON_AddNumberToObject(obj, "slaveid", g_plc_output[i].slaveid);
            }			
            found = 1;
        } else {
            for (size_t i = 0; g_plc_output[i].identifier[0]; i++) {
                if (strncmp(g_plc_output[i].identifier, cj_identifier, sizeof(g_plc_output[i].identifier)-1) == 0) {
                    cJSON_AddNumberToObject(resp_root, "code", 0);
                    cJSON_AddStringToObject(resp_root, "identifier", g_plc_output[i].identifier);
                    cJSON_AddStringToObject(resp_root, "address", utoa_hex(g_plc_output[i].address));
                    cJSON_AddStringToObject(resp_root, "label", g_plc_output[i].label);
                    cJSON_AddNumberToObject(resp_root, "slaveid", g_plc_output[i].slaveid);
                    found = 1;
                    break;
                }
            }
        }
    }

    if (!found) {
        cJSON_AddNumberToObject(resp_root, "code", -1);
        cJSON_AddStringToObject(resp_root, "msg", "Invalid identifier");
    }

    cj_identifier = cJSON_PrintUnformatted(resp_root);
    //fprintf(stderr, "resp %s\n", cj_identifier);
    strncpy(msg, cj_identifier, msg_len-1);
    cJSON_Delete(resp_root);

    return 0;
}

int modbus_plc_put(char *payload, char *msg, int msg_len)
{
    int found = 0;
    cJSON *resp_root = NULL;
    cJSON *req_json_obj = NULL;
    char *cj_identifier = NULL;	
    char *cj_address = NULL;	
    char *cj_label = NULL;	
    int address = 0;	
    int slaveid = 0;	
	
    //fprintf(stderr, "modbus_plc_put %s", payload);

    req_json_obj = cJSON_Parse(payload);
    resp_root = cJSON_CreateObject();

    if (req_json_obj) {
        cj_identifier = json_get_char(req_json_obj, "identifier");
        cj_label = json_get_char(req_json_obj, "label");
        cj_address = json_get_char(req_json_obj, "address");
        if (cj_address)
	    address = hex2int(cj_address, strlen(cj_address));
        slaveid = json_get_int(req_json_obj, "slaveid");
    }

    if (cj_identifier && cj_label && address && slaveid) {
        for (size_t i = 0; g_plc_output[i].identifier[0]; i++) {
            if (strncmp(g_plc_output[i].identifier, cj_identifier, sizeof(g_plc_output[i].identifier)-1) == 0) {
                strncpy(g_plc_output[i].label, cj_label, sizeof(g_plc_output[i].label)-1);				
                g_plc_output[i].address = address;
                g_plc_output[i].slaveid = slaveid;

                cJSON_AddNumberToObject(resp_root, "code", 0);
                cJSON_AddStringToObject(resp_root, "identifier", g_plc_output[i].identifier);
                cJSON_AddStringToObject(resp_root, "address", utoa_hex(g_plc_output[i].address));
                cJSON_AddStringToObject(resp_root, "label", g_plc_output[i].label);
                cJSON_AddNumberToObject(resp_root, "slaveid", g_plc_output[i].slaveid);
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        cJSON_AddNumberToObject(resp_root, "code", -1);
        cJSON_AddStringToObject(resp_root, "msg", "Invalid identifier");
    }

    cj_identifier = cJSON_PrintUnformatted(resp_root);
    //fprintf(stderr, "resp %s\n", cj_identifier);
    strncpy(msg, cj_identifier, msg_len-1);
    cJSON_Delete(resp_root);
    return 0;
}

