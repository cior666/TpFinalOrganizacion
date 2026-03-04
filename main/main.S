.equ GPIO_BASE, 0x60004000
.equ GPIO_ENABLE_W1TS_REG, GPIO_BASE + 0x0024
.equ GPIO_ENABLE_W1TC_REG, GPIO_BASE + 0x0028
.equ GPIO_OUT_W1TC_REG, GPIO_BASE + 0x000C
.equ GPIO_OUT_W1TS_REG, GPIO_BASE + 0x0008
.equ GPIO_IN_REG, GPIO_BASE + 0x003C 
.equ UART1_BASE, 0x60010000
.equ UART1_FIFO, UART1_BASE + 0x0000
.equ UART_CLKDIV_REG, UART1_BASE + 0x0014 
.equ UART1_STATUS_REG, UART1_BASE + 0x001C
.equ UART_RXFIFO_CNT_MASK, 0x000003FF
.equ UART_CONF0_REG, UART1_BASE + 0x0020 
.equ GPIO0_LED1, (1<<0)
.equ GPIO1_LED2, (1<<1)
.equ GPIO5_LED3, (1<<5)



.global configurarSalida_leds
.global apagar_todos
.global apagar_led1
.global apagar_led2
.global prender_led1
.global prender_led2
.global prender_led3
.global leer_datos_gps

.text


configurarSalida_leds:
    li t0, GPIO_ENABLE_W1TS_REG
    li t1, GPIO0_LED1 | GPIO1_LED2 | GPIO5_LED3 
    sw t1, 0(t0)
    ret

//Los leds son activos bajos, se apagan con Set y se prenden con Clear, por eso en apagar se usa W1TS y en prender W1TC
apagar_todos:
    li t1, GPIO0_LED1 | GPIO1_LED2 | GPIO5_LED3 
    li t2, GPIO_OUT_W1TS_REG
    sw t1, 0(t2)
    ret
apagar_led1:
    li t1, GPIO0_LED1
    li t2, GPIO_OUT_W1TS_REG
    sw t1, 0(t2)
    ret
apagar_led2:
    li t1, GPIO1_LED2
    li t2, GPIO_OUT_W1TS_REG
    sw t1, 0(t2)
    ret
prender_led1:
    li t1, GPIO0_LED1
    li t2, GPIO_OUT_W1TC_REG
    sw t1, 0(t2)
    ret
prender_led2:
    li t1, GPIO1_LED2
    li t2, GPIO_OUT_W1TC_REG
    sw t1, 0(t2)
    ret
prender_led3:
    li t1, GPIO5_LED3
    li t2, GPIO_OUT_W1TC_REG
    sw t1, 0(t2)
    ret
leer_datos_gps:
    li t0, UART1_STATUS_REG
    lw t2, 0(t0)
    andi t2, t2, UART_RXFIFO_CNT_MASK
    beqz t2, fin
    li t3, UART1_FIFO
    lb a0, 0(t3)
    ret
fin:
    li a0, -1
    ret
