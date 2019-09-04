/* mqtt4modbus
 * modbus tcp/rtu via mqtt
 * author: daixijiang@gmail.com
 */
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include <mosquitto.h>
#include "common.h"

extern plc_output_t g_plc_output[];
struct mosquitto *g_mosq = NULL;

void mqtt_connect_callback(struct mosquitto *g_mosq, void *obj, int rc)
{
    if (rc == 0)
    {
        if (mosquitto_subscribe(g_mosq, NULL, MQTT_PREFIX MQTT_MY_OPER MQTT_MY_OPER_SAVE, 1) != MOSQ_ERR_SUCCESS)
        {
            fprintf(stderr, "mosquitto_subscribe: Call failed %s\n", MQTT_PREFIX MQTT_MY_OPER MQTT_MY_OPER_SAVE);
        }

        if (mosquitto_subscribe(g_mosq, NULL, MQTT_PREFIX MQTT_MY_OPER MQTT_MY_OPER_GET, 1) != MOSQ_ERR_SUCCESS)
        {
            fprintf(stderr, "mosquitto_subscribe: Call failed %s\n", MQTT_PREFIX MQTT_MY_OPER MQTT_MY_OPER_GET);
        }

        if (mosquitto_subscribe(g_mosq, NULL, MQTT_PREFIX MQTT_MY_OPER MQTT_MY_OPER_PUT, 1) != MOSQ_ERR_SUCCESS)
        {
            fprintf(stderr, "mosquitto_subscribe: Call failed %s\n", MQTT_PREFIX MQTT_MY_OPER MQTT_MY_OPER_PUT);
        }

        for (size_t i = 0; g_plc_output[i].identifier[0]; i++)
        {
            char *topic = malloc(strlen(MQTT_PREFIX) + strlen(g_plc_output[i].identifier) + strlen(MQTT_MY_PLC_SET) + 1);

            if (topic == NULL)
            {
                fprintf(stderr, "malloc: Not enough memory\n");
                exit(EXIT_FAILURE);
            }

            strcpy(topic, MQTT_PREFIX);
            strcat(topic, g_plc_output[i].identifier);
            strcat(topic, MQTT_MY_PLC_SET);

            if (mosquitto_subscribe(g_mosq, NULL, topic, 1) != MOSQ_ERR_SUCCESS)
            {
                fprintf(stderr, "mosquitto_subscribe: Call failed %s\n", topic);
            }

            free(topic);
        }

        for (size_t i = 0; g_plc_output[i].identifier[0]; i++)
        {
            char *topic = malloc(strlen(MQTT_PREFIX) + strlen(g_plc_output[i].identifier) + strlen(MQTT_MY_PLC_GET) + 1);

            if (topic == NULL)
            {
                fprintf(stderr, "malloc: Not enough memory\n");
                exit(EXIT_FAILURE);
            }

            strcpy(topic, MQTT_PREFIX);
            strcat(topic, g_plc_output[i].identifier);
            strcat(topic, MQTT_MY_PLC_GET);

            if (mosquitto_subscribe(g_mosq, NULL, topic, 1) != MOSQ_ERR_SUCCESS)
            {
                fprintf(stderr, "mosquitto_subscribe: Call failed %s\n", topic);
            }

            free(topic);
        }

        for (size_t i = 0; g_plc_output[i].identifier[0]; i++)
        {
            char *topic = malloc(strlen(MQTT_PREFIX) + strlen(g_plc_output[i].identifier) + strlen(MQTT_MY_PLC_TOG) + 1);

            if (topic == NULL)
            {
                fprintf(stderr, "malloc: Not enough memory\n");
                exit(EXIT_FAILURE);
            }

            strcpy(topic, MQTT_PREFIX);
            strcat(topic, g_plc_output[i].identifier);
            strcat(topic, MQTT_MY_PLC_TOG);

            if (mosquitto_subscribe(g_mosq, NULL, topic, 1) != MOSQ_ERR_SUCCESS)
            {
                fprintf(stderr, "mosquitto_subscribe: Call failed %s\n", topic);
            }

            free(topic);
        }
    }
    else
    {
        fprintf(stderr, "mqtt_connect_callback: Connection failed\n");
    }
}

void mqtt_message_callback(struct mosquitto *g_mosq, void *obj, const struct mosquitto_message *message)
{
    char topic_reply[10240] = {0};

    if (strcmp(message->topic, MQTT_PREFIX MQTT_MY_OPER MQTT_MY_OPER_SAVE) == 0) {
        modbus_save_plc(MODBUS_PLC_CFG);
        if (mosquitto_publish(g_mosq, NULL, MQTT_PREFIX MQTT_MY_OPER MQTT_MY_OPER_SAVE MQTT_MY_PLC_OK, 
			strlen(topic_reply)+1, topic_reply, 1, false) != MOSQ_ERR_SUCCESS)
        {
            fprintf(stderr, "mosquitto_publish: Call failed %s\n", MQTT_PREFIX MQTT_MY_OPER MQTT_MY_OPER_SAVE MQTT_MY_PLC_OK);
        }
        return;
    } else if (strcmp(message->topic, MQTT_PREFIX MQTT_MY_OPER MQTT_MY_OPER_PUT) == 0) {
        modbus_plc_put(message->payload, topic_reply, sizeof(topic_reply));
        if (mosquitto_publish(g_mosq, NULL, MQTT_PREFIX MQTT_MY_OPER MQTT_MY_OPER_PUT MQTT_MY_PLC_OK, 
			strlen(topic_reply)+1, topic_reply, 1, false) != MOSQ_ERR_SUCCESS)
        {
            fprintf(stderr, "mosquitto_publish: Call failed %s\n", MQTT_PREFIX MQTT_MY_OPER MQTT_MY_OPER_PUT MQTT_MY_PLC_OK);
        }
        return;
    } else if (strcmp(message->topic, MQTT_PREFIX MQTT_MY_OPER MQTT_MY_OPER_GET) == 0) {
        modbus_plc_get(message->payload, topic_reply, sizeof(topic_reply));
        if (mosquitto_publish(g_mosq, NULL, MQTT_PREFIX MQTT_MY_OPER MQTT_MY_OPER_GET MQTT_MY_PLC_OK, 
			strlen(topic_reply)+1, topic_reply, 1, false) != MOSQ_ERR_SUCCESS)
        {
            fprintf(stderr, "mosquitto_publish: Call failed %s\n", MQTT_PREFIX MQTT_MY_OPER MQTT_MY_OPER_GET MQTT_MY_PLC_OK);
        }	
        return;
    }

    for (size_t i = 0; g_plc_output[i].identifier[0]; i++)
    {
        char *topic = malloc(strlen(MQTT_PREFIX) + strlen(g_plc_output[i].identifier) + strlen(MQTT_MY_PLC_SET) + 1);

        if (topic == NULL)
        {
            fprintf(stderr, "malloc: Not enough memory\n");
            exit(EXIT_FAILURE);
        }

        strcpy(topic, MQTT_PREFIX);
        strcat(topic, g_plc_output[i].identifier);
        strcat(topic, MQTT_MY_PLC_SET);

        if (strcmp(message->topic, topic) == 0)
        {
            bool ok = false;

            if (message->payloadlen == strlen("true") && strcmp(message->payload, "true") == 0)
            {
                mb_write_bit(g_plc_output[i].slaveid, g_plc_output[i].address, true);
	        g_plc_output[i].status = true;

                ok = true;
            }
            else if (message->payloadlen == strlen("false") && strcmp(message->payload, "false") == 0)
            {
                mb_write_bit(g_plc_output[i].slaveid, g_plc_output[i].address, false);
                g_plc_output[i].status = false;

                ok = true;
            }

            if (ok)
            {
                char *response = malloc(strlen(topic) + strlen(MQTT_MY_PLC_OK) + 1);

                if (response == NULL)
                {
                    fprintf(stderr, "malloc: Not enough memory\n");
                    exit(EXIT_FAILURE);
                }

                strcpy(response, topic);
                strcat(response, MQTT_MY_PLC_OK);

                if (mosquitto_publish(g_mosq, NULL, response, 0, NULL, 1, false) != MOSQ_ERR_SUCCESS)
                {
                    fprintf(stderr, "mosquitto_publish: Call failed %s\n", response);
                }

                free(response);
            }
        }

        free(topic);
    }

    for (size_t i = 0; g_plc_output[i].identifier[0]; i++)
    {
        char *topic = malloc(strlen(MQTT_PREFIX) + strlen(g_plc_output[i].identifier) + strlen(MQTT_MY_PLC_GET) + 1);

        if (topic == NULL)
        {
            fprintf(stderr, "malloc: Not enough memory\n");
            exit(EXIT_FAILURE);
        }

        strcpy(topic, MQTT_PREFIX);
        strcat(topic, g_plc_output[i].identifier);
        strcat(topic, MQTT_MY_PLC_GET);

        if (strcmp(message->topic, topic) == 0)
        {
            bool ok = false;
            mb_read_bit(g_plc_output[i].slaveid, g_plc_output[i].address, &ok);
            char *response = malloc(strlen(topic) + strlen(MQTT_MY_PLC_OK) + 1);

            if (response == NULL)
            {
                fprintf(stderr, "malloc: Not enough memory\n");
                exit(EXIT_FAILURE);
            }

            strcpy(response, topic);
            strcat(response, MQTT_MY_PLC_OK);
            char get_payload[10];
            bzero(get_payload, sizeof(get_payload));
            if (ok)
                strcpy(get_payload, "true");
            else
                strcpy(get_payload, "false");

            if (mosquitto_publish(g_mosq, NULL, response, strlen(get_payload), get_payload, 1, false) != MOSQ_ERR_SUCCESS)
            {
                fprintf(stderr, "mosquitto_publish: Call failed %s\n", response);
            }

            free(response);
        }

        free(topic);
    }

    for (size_t i = 0; g_plc_output[i].identifier[0]; i++)
    {
        char *topic = malloc(strlen(MQTT_PREFIX) + strlen(g_plc_output[i].identifier) + strlen(MQTT_MY_PLC_TOG) + 1);

        if (topic == NULL)
        {
            fprintf(stderr, "malloc: Not enough memory\n");
            exit(EXIT_FAILURE);
        }

        strcpy(topic, MQTT_PREFIX);
        strcat(topic, g_plc_output[i].identifier);
        strcat(topic, MQTT_MY_PLC_TOG);

        if (strcmp(message->topic, topic) == 0)
        {
            bool ok = false;

            if (message->payloadlen == 0)
            {
                bool state;

                mb_read_bit(g_plc_output[i].slaveid, g_plc_output[i].address, &state);
                mb_write_bit(g_plc_output[i].slaveid, g_plc_output[i].address, !state);
                g_plc_output[i].status = !g_plc_output[i].status;

                ok = true;
            }

            if (ok)
            {
                char *response = malloc(strlen(topic) + strlen(MQTT_MY_PLC_OK) + 1);

                if (response == NULL)
                {
                    fprintf(stderr, "malloc: Not enough memory\n");
                    exit(EXIT_FAILURE);
                }

                strcpy(response, topic);
                strcat(response, MQTT_MY_PLC_OK);

                if (mosquitto_publish(g_mosq, NULL, response, 0, NULL, 1, false) != MOSQ_ERR_SUCCESS)
                {
                    fprintf(stderr, "mosquitto_publish: Call failed %s\n", response);
                }

                free(response);
            }
        }

        free(topic);
    }
}

void cleanup(void)
{
    if (g_mosq != NULL) {
        mosquitto_destroy(g_mosq);
    }

    mosquitto_lib_cleanup();

    mb_cleanup();
}

int main(int argc, char **argv)
{
    if (atexit(cleanup) != 0)
    {
        fprintf(stderr, "atexit: Call failed\n");
        exit(EXIT_FAILURE);
    }

    /* load config */
    mb_init();
    /* end for load */

    mosquitto_lib_init();

    g_mosq = mosquitto_new(NULL, true, NULL);

    if (g_mosq == NULL)
    {
        fprintf(stderr, "mosquitto_new: Call failed\n");
        exit(EXIT_FAILURE);
    }

    mosquitto_connect_callback_set(g_mosq, mqtt_connect_callback);
    mosquitto_message_callback_set(g_mosq, mqtt_message_callback);

    if (mosquitto_connect(g_mosq, MQTT_HOST, MQTT_PORT, MQTT_KEEPALIVE) != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr, "mosquitto_connect: Unable to connect\n");
        exit(EXIT_FAILURE);
    }

    /* hello for mqtt */
    char topic_hello[128] = {0};
    time_t timep;
    time (&timep);
    snprintf(topic_hello, sizeof(topic_hello)-1, "%s%s oneline %s", MQTT_PREFIX, MQTT_MY_HELLO, ctime(&timep));
    if (mosquitto_publish(g_mosq, NULL, MQTT_PREFIX MQTT_MY_HELLO, strlen(topic_hello)+1, topic_hello, 1, false) != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr, "mosquitto_publish: Call failed %s\n", MQTT_PREFIX MQTT_MY_HELLO);
    }
    /* end for hello */

    mosquitto_loop_forever(g_mosq, -1, 1);

    exit(EXIT_SUCCESS);
}

