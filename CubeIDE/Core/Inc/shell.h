/*
 * shell.h
 *
 *  Created on: 7 juin 2019
 *      Author: laurent
 */

#ifndef INC_LIB_SHELL_SHELL_H_//pour éviter double inclusion
#define INC_LIB_SHELL_SHELL_H_

#include <stdint.h>
#include "usart.h"
#include "cmsis_os.h"
#define UART_DEVICE huart2

#define ARGC_MAX 8
#define BUFFER_SIZE 40
#define SHELL_FUNC_LIST_MAX_SIZE 64

struct h_shell_struct;
typedef uint8_t (* drv_shell_transmit_t)(char * pData, uint16_t size);
typedef uint8_t (* drv_shell_receive_t)(char * pData, uint16_t size);

typedef struct drv_shell_struct
{
drv_shell_transmit_t drv_shell_transmit;
drv_shell_receive_t drv_shell_receive;
} drv_shell_t;

typedef struct{
	char c;
	int (* func)(int argc, char ** argv,struct h_shell_struct* h_shell);
	char * description;
} shell_func_t;

typedef struct h_shell_struct{
	UART_HandleTypeDef *huart;
	drv_shell_t drv_shell;
	SemaphoreHandle_t sem_uart_rx;

	int shell_func_list_size;
	shell_func_t shell_func_list[SHELL_FUNC_LIST_MAX_SIZE];

	char print_buffer[BUFFER_SIZE];
	char cmd_buffer[BUFFER_SIZE];
}h_shell_t;



void shell_init(h_shell_t *h_shell);
int shell_add(char c, int (* pfunc)(int argc, char ** argv,h_shell_t* h_shell), char * description, h_shell_t *h_shell);
int shell_run(h_shell_t *h_shell);
void shell_uart_rx_callback(h_shell_t *h_shell);
#endif /* INC_LIB_SHELL_SHELL_H_ */

//les fonctions masquées sont les fonctions statiques
