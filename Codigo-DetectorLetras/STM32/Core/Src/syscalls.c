/*
 * syscalls.c
 *
 *  Created on: Oct 5, 2025
 *      Author: Melissa
 */

#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include "usbd_def.h"

// Debes declarar esta función externa, ya que está definida en tu middleware USB
extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);
// (Si tu proyecto usa UART, usarías el manejador HAL_UART_Transmit en su lugar)

int __io_putchar(int ch) {
    // Implementación de _write para stdout/stderr
    uint8_t temp = ch;

    // El '100' es el tiempo de espera (timeout) en ms.
    // Usamos CDC_Transmit_FS para enviar el byte.
    CDC_Transmit_FS(&temp, 1);

    return ch;
}

// Implementación de _write para redirigir la salida
int _write(int file, char *ptr, int len)
{
    // Solo redirigimos stdout (1) y stderr (2)
    if (file == 1 || file == 2)
    {
        uint8_t status = CDC_Transmit_FS((uint8_t*)ptr, len);
        return (status == USBD_OK ? len : 0);
    }

    // Si no es un descriptor de archivo válido
    errno = EIO;
    return -1;
}

void _close(){}
void _fstat(){}
void _getpid(){}
void _isatty(){}
void _kill(){}
void _lseek(){}
void _read(){}

