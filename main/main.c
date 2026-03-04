#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include <string.h>

extern void configurarSalida_leds(void);
extern void apagar_todos(void);
extern void prender_led1(void);
extern void prender_led2(void);
extern void prender_led3(void);
extern void apagar_led1(void);
extern void apagar_led2(void);
extern int leer_datos_gps(void);


char seleccionar_opcion(void) {
    printf("   GPS - LECTURA DE COORDENADAS Y DATOS   \n");
    printf("Elige una opcion\n");
    printf(" 1- Latitud y longitud\n");
    printf(" 2- Altitud\n");
    printf(" 3- Velocidad\n");
    char seleccion = '0';
    while (1) {
        seleccion=getchar();
        if (seleccion == '1' || seleccion == '2' || seleccion == '3') {
            printf(" Recibido, buscando información\n");
            return seleccion;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void extraer_dato_gps(const char *mensaje, int campo_deseado, char *resultado) {
    int separadores_encontrados = 0;
    int posicion_actual = 0, posicion_resultado = 0;
    
    // iteramos sobre cada carácter de la cadena GPS hasta encontrar el fin 
    while (mensaje[posicion_actual] != '\0' && mensaje[posicion_actual] != '*') {
        
        // aumentamos el contador cada vez que encuentra un delimitador (coma)
        if (mensaje[posicion_actual] == ',') {
            separadores_encontrados++;
        } 
        // copiamos el carácter al resultado si estamos en el campo buscado
        else if (separadores_encontrados == campo_deseado) {
            resultado[posicion_resultado] = mensaje[posicion_actual];
            posicion_resultado++;
        } 
        // detenemos la búsqueda si ya ha pasado el campo deseado
        else if (separadores_encontrados > campo_deseado) {
            break;
        }
        posicion_actual++;
    }
    
    // terminamos la cadena de resultado con el carácter nulo
    resultado[posicion_resultado]= '\0';
}


void mostrar_opcion(char seleccion){
    char linea_gps[128];
    int posicion_buffer = 0;
    
    while(true) {
        int caracter;

        // leemos caracteres del puerto UART1 conectado al módulo GPS
        while ((caracter = leer_datos_gps()) != -1) {

            // detectamos el final de una línea de trama GPS
            if (caracter == '\n') {

                linea_gps[posicion_buffer] = '\0';

                // procesamos tramas GPGGA (coordenadas geográficas)
                if (strstr(linea_gps, "$GPGGA") && seleccion=='1') {
                    char latitud[15] = "", longitud[15] = "", dir_lat[5] = "", dir_lon[5] = "";
                    
                    // extraemos los campos de latitud, dirección, longitud y dirección
                    extraer_dato_gps(linea_gps, 2, latitud);
                    extraer_dato_gps(linea_gps, 3, dir_lat);
                    extraer_dato_gps(linea_gps, 4, longitud);
                    extraer_dato_gps(linea_gps, 5, dir_lon);
                    
                    // mostramos los resultados en formato legible
                    printf("\n DATOS DE POSICION \n");
                    printf("Longitud %s: %.3s grados, %s minutos\n", dir_lon, longitud, longitud+3);
                    printf("Latitud %s: %.2s grados, %s minutos\n", dir_lat, latitud, latitud+2);
                    return;
                }
                
                // procesamos tramas GPGGA para altitud (altura sobre el nivel del mar)
                else if (strstr(linea_gps, "$GPGGA") && seleccion=='2') {
                    char altitud[15] = "";
                    
                    // extraemos el campo de altitud (campo número 9)
                    extraer_dato_gps(linea_gps, 9, altitud);
                    printf("\n DATOS DE ALTITUD \n");
                    printf("Altitud sobre el nivel del mar: %s metros\n", altitud);

                    return;
                }
                
                // procesamos tramas GPVTG (velocidad y dirección de movimiento)
                else if (strstr(linea_gps, "$GPVTG") && seleccion=='3') {
                    char velocidad[15] = "";
                    
                    // extraemos el campo de velocidad en km/h (campo número 7)
                    extraer_dato_gps(linea_gps, 7, velocidad);
                    printf("\n DATOS DE VELOCIDAD \n");
                    printf("Velocidad de desplazamiento: %s km/h\n", velocidad);
                    return;
                }
                
                // reiniciamos el buffer para la próxima línea
                posicion_buffer = 0;
            }

            // almacenamos caracteres en el buffer si hay espacio disponible
            // se reserva 1 byte para el terminador nulo ('\0')
            else if(posicion_buffer < 127){
                linea_gps[posicion_buffer] = (char)caracter;
                posicion_buffer++;
            }
        }
        // agregamos pausa para no saturar la CPU mientras espera datos
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void app_main(void)
{
    // inicializamos del controlador UART1 para recibir datos del módulo GPS
    // establecimos un buffer de recepción de 256 bytes en RAM para almacenar datos seriales
    // El buffer de transmisión se establece en 0 porque no se enviarán datos al GPS
    uart_driver_install(UART_NUM_1, 256, 0, 0, NULL, 0);

    // configuramos los parámetros del puerto serie UART1
    uart_config_t config_uart = {
        .baud_rate = 9600,                     // Baud rate del NEO 6M
        .data_bits = UART_DATA_8_BITS,         
        .parity    = UART_PARITY_DISABLE,     
        .stop_bits = UART_STOP_BITS_1,         
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, 
        .source_clk = UART_SCLK_APB,           
    };

    // aplicamos de la configuración al puerto UART1
    uart_param_config(UART_NUM_1, &config_uart);
    

    //uart_set_pin(uart, tx_pin, rx_pin, rts_pin, cts_pin)
    uart_set_pin(UART_NUM_1, 3, 10, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    //TX de gps -> 10 , TX de gps -> 3

    //limpiamos el pin porque estaba tomado por otra funcion
    gpio_reset_pin(5);

    // cucle principal que controla el sistema de adquisición de datos GPS
    while(true){
        // configuración inicial de los pines GPIO destinados a los LEDs
        configurarSalida_leds();
        
        // muestra el menú de opciones y espera la selección del usuario
        char opcion_usuario = seleccionar_opcion();
        
        
        // apaga todos los LEDs y enciende el LED 1 (indicador de procesamiento)
        apagar_todos();
        prender_led1();
        vTaskDelay(pdMS_TO_TICKS(200));

        // apaga el LED 1 y enciende el LED 2 (progresión visual)
        apagar_led1();
        prender_led2();
        
        
        // procesa la opción seleccionada por el usuario y muestra los datos GPS
        mostrar_opcion(opcion_usuario);
        
        // apaga el LED 2 y enciende el LED 3 (indicador de finalización)
        apagar_led2();
        prender_led3();
        

    }
}