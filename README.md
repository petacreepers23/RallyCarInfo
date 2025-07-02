# RallyCarInfo
Rally Car Info

# Vehicle Telemetry Display System

Sistema modular de telemetría para vehículo con visualización en pantalla táctil. Muestra en tiempo real información crítica como temperaturas, presiones, brújula, inclinación y estado de los neumáticos, mediante sensores externos, OBDII y procesamiento distribuido con microcontroladores ESP32.

## 🚗 Funcionalidades principales

- Temperatura y presión del agua y aceite del motor.
- Presión y temperatura de cada neumático, con indicador de batería.
- Brújula digital con rumbo actual en grados y cardinales.
- Inclinómetro longitudinal y transversal con alertas de umbral.
- Interfaz gráfica táctil en pantalla Nextion 7”.
- Sistema de alertas visuales (parpadeos, colores, fondo rojo).
- Distribución del cómputo entre tres nodos ESP32 para eficiencia.

## 🧱 Arquitectura del sistema

| Nodo | Responsabilidades principales |
|------|-------------------------------|
| ESP32 A  | Lectura de sensores BN085 (brújula e inclinación) |
| ESP32 B  | Lectura de sensores TPMS y OBDII (temperaturas y presiones) |
| ESP32 C  | Renderizado en pantalla Nextion, recepción de datos, lógica de interfaz |

Los nodos se comunican mediante UART o WiFi y publican datos en formato JSON o buffers personalizados.

## 🖥️ Interfaz

La pantalla táctil Nextion de 7” (800x480) organiza la información en los siguientes componentes:

- Diales tipo aguja para temperaturas.
- Indicadores digitales para presiones.
- Esquema visual del coche para neumáticos.
- Banda superior con brújula en tiempo real.
- Indicadores de inclinación con alertas.

## 📦 Componentes de hardware

| Componente | Descripción |
|-----------|-------------|
| 3x ESP32| Procesamiento distribuido |
| Nextion NX8048P070-011C | Pantalla táctil 7” |
| BN085 | Acelerómetro + brújula |
| Sensor TPMS | Sensor de presión y temperatura neumáticos |
| OBDII (por confirmar) | Fuente adicional de datos del vehículo |

## 📚 Documentación

Los requerimientos del sistema están definidos en el documento privado **SRS.doc** (Software Requirements Specification).

Las especificaciones detalladas por componente se encuentran en documento privado **SRD.md** (Software Requirements Design), que incluye:

- Dimensiones de pantalla por componente.
- Frecuencia de actualización por sensor.
- Asignación de tareas por ESP32.
- Librerías utilizadas.
- Buffers y comunicación entre nodos.


## ✅ Estado del proyecto

- [x] Especificación de requisitos (SRS)
- [x] Diseño detallado del sistema (SRD)
- [ ] Prototipo funcional
- [ ] Montaje final y pruebas en vehículo

