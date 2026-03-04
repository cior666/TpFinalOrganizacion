# Sistema GPS con ESP32-C3
Este repositorio contiene el desarrollo de un sistema embebido que permite recibir y procesar información de posicionamiento proveniente de un módulo GPS utilizando un microcontrolador **ESP32-C3**.
El sistema se comunica con el GPS mediante **UART**, recibe continuamente las **sentencias NMEA** generadas por el módulo y utiliza técnicas de **parsing** para extraer información relevante como:
- Coordenadas geográficas (latitud y longitud)
- Velocidad
- Altitud
El usuario puede interactuar con el sistema mediante la **consola serie**, enviando comandos que permiten seleccionar qué información desea visualizar.

## Componentes utilizados
- ESP32-C3
- Módulo GPS (tipo NEO-6M o similar)
- Antena GPS externa
- Protoboard
- Cables Dupont
- Teclado con LEDs indicadores

## Funcionamiento general
El módulo GPS transmite continuamente datos de navegación utilizando el estándar **NMEA**.  
El ESP32 recibe estas tramas mediante **UART**, analiza las cadenas de caracteres recibidas y extrae los campos correspondientes según el tipo de sentencia.
El sistema permite consultar distintos parámetros enviando comandos desde el monitor serie.

## Contenido del repositorio
El repositorio incluye:
- Código fuente en **C**
- Rutinas en **ensamblador**
- Configuración del proyecto para **ESP-IDF**
- Archivos de compilación

## Informe del trabajo
La explicación completa del proyecto, incluyendo:
- desarrollo teórico
- descripción del hardware
- arquitectura del sistema
- diagramas
- funcionamiento del software
se encuentra detallada en el **informe en formato PDF incluido en este repositorio**.

## Objetivo del trabajo

Este proyecto forma parte de un trabajo práctico orientado a comprender el funcionamiento de **sistemas embebidos**, la comunicación serie mediante **UART**, y el procesamiento de datos provenientes de sensores externos en un entorno real de hardware.
