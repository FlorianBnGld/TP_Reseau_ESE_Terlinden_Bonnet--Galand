/*
 * shell.c
 *
 *  Created on: 7 juin 2019
 *      Author: Laurent Fiack
 */

#include "shell.h"

#include <stdio.h>

#include "usart.h"
#include "gpio.h"
#include "cmsis_os.h"
#include "semphr.h"



//static int shell_func_list_size = 0;
//static shell_func_t shell_func_list[SHELL_FUNC_LIST_MAX_SIZE];

//static char print_buffer[BUFFER_SIZE];

static char uart_read(h_shell_t *h_shell) {
	char c;
	//HAL_UART_Receive(&UART_DEVICE, (uint8_t*)(&c), 1, HAL_MAX_DELAY);
	//HAL_UART_Receive_IT(h_shell->huart, (uint8_t*)(&c), 1);
	//Ici il faut bloquer la tâche (eg : Prendre un sémaphore vide)
	//xSemaphoreTake(h_shell->sem_uart_rx, portMAX_DELAY);
	//return c;

	h_shell->drv_shell.drv_shell_receive(&c,1);
	return c;
}

static int uart_write(char * s, uint16_t size, h_shell_t *h_shell) {
	//HAL_UART_Transmit(h_shell->huart, (uint8_t*)s, size, HAL_MAX_DELAY);
	h_shell->drv_shell.drv_shell_transmit(s,1);
	return size;
}

static int sh_help(int argc, char ** argv,h_shell_t *h_shell) {
	int i;
	for(i = 0 ; i < h_shell->shell_func_list_size ; i++) {
		int size;
		size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "%c: %s\r\n", h_shell->shell_func_list[i].c, h_shell->shell_func_list[i].description);
		uart_write(h_shell->print_buffer, size,h_shell);
	}

	return 0;
}

void shell_init(h_shell_t *h_shell) {
	int size = 0;
	h_shell->sem_uart_rx= xSemaphoreCreateBinary();
	h_shell->shell_func_list_size=0;
	size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "\r\n\r\n===== Monsieur Shell v0.2 =====\r\n");
	uart_write(h_shell->print_buffer, size,h_shell);

	shell_add('h', sh_help, "Help",h_shell);


}

int shell_add(char c, int (* pfunc)(int argc, char ** argv,h_shell_t *h_shell), char * description, h_shell_t *h_shell) {
	int idx = h_shell->shell_func_list_size++;
	if (h_shell->shell_func_list_size < SHELL_FUNC_LIST_MAX_SIZE) {
		h_shell->shell_func_list[idx].c = c;
		h_shell->shell_func_list[idx].func = pfunc;
		h_shell->shell_func_list[idx].description = description;
		return 0;
	}

	return -1;
}
//function to call in the uart rx interrupt
void shell_uart_rx_callback(h_shell_t *h_shell){
	BaseType_t higher_priority_task_woken=pdFALSE;
	xSemaphoreGiveFromISR(h_shell->sem_uart_rx,&higher_priority_task_woken);
	portYIELD_FROM_ISR(higher_priority_task_woken);
}

static int shell_exec(char * buf, h_shell_t *h_shell) {
	int i;
	char c = buf[0];

	int argc;
	char * argv[ARGC_MAX];
	char *p;

	for(i = 0 ; i < h_shell->shell_func_list_size ; i++) {
		if (h_shell->shell_func_list[i].c == c) {
			argc = 1;
			argv[0] = buf;

			for(p = buf ; *p != '\0' && argc < ARGC_MAX ; p++){
				if(*p == ' ') {
					*p = '\0';
					argv[argc++] = p+1;
				}
			}

			return h_shell->shell_func_list[i].func(argc, argv,h_shell);
		}
	}

	int size;
	size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "%c: no such command\r\n", c);
	uart_write(h_shell->print_buffer, size,h_shell);
	return -1;
}



int shell_run(h_shell_t *h_shell) {
	int reading = 0;
	int pos = 0;


	while (1) {
		uart_write(">", 2,h_shell);
		reading = 1;

		while(reading) {
			char c = uart_read(h_shell);
			int size;

			switch (c) {
			//process RETURN key
			case '\r':
				//case '\n':
				size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "\r\n");
				uart_write(h_shell->print_buffer, size,h_shell);
				h_shell->cmd_buffer[pos++] = 0;     //add \0 char at end of string
				size = snprintf (h_shell->print_buffer, BUFFER_SIZE, ":%s\r\n", h_shell->cmd_buffer);
				uart_write(h_shell->print_buffer, size,h_shell);
				reading = 0;        //exit read loop
				pos = 0;            //reset buffer
				break;
				//backspace
			case '\b':
				if (pos > 0) {      //is there a char to delete?
					pos--;          //remove it in buffer

					uart_write("\b \b", 3,h_shell);	// delete the char on the terminal
				}
				break;
				//other characters
			default:
				//only store characters if buffer has space
				if (pos < BUFFER_SIZE) {
					uart_write(&c, 1,h_shell);
					h_shell->cmd_buffer[pos++] = c; //store
				}
			}
		}
		shell_exec(h_shell->cmd_buffer,h_shell);
	}
	return 0;
}
