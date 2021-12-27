# Sección 3: Integración FPGA+CPU

En esta sección realizaremos multiplicaciones de matrices en FPGA y compararemos la velocidad con la de la implementación en CPU. Para esto descargaremos los archivos necesarios para trabajar con la tarjeta ZCU104, realizaremos cambios al código de la seccion anterior para que se pueda comunicar con la CPU y finalmente lo probaremos y mediremos el rendimiento.

## Preparación

* Descargar el archivo [ZCU104 Base 2020.2](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-platforms/2020-2.html)
* Descargar el archivo [ ZYNQMP common image](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-platforms/2020-2.html) y luego, en el directorio descargado, correr el siguiente comando `./sdk.sh -y -dir . -p`.

## Cambios al Código

El código que se trabajó en la [sección anterior](../Seccion2/Seccion2.md) se le agrega código para que guarde localmente las matrices A y B, de esta forma, cuando se necesiten leer los datos, estos ya están en la FPGA guardados en memorias con las características correspondientes.

## Exportar desde Vitis HLS

Se debe crear un proyecto en Vitis HLS con el código multiplicador de matrices, una vez que se tenga listo (que pase simulación, cosimulación y se esté satisfecho con el uso de recursos) se debe exportar un archivo *.xo*. En *solution settings* >> *Export* >> *Format Selection* se debe escoger la opción *Vitis Kernel (.xo)* y luego se puede exportar. 

## Crear un Proyecto en Vitis 2020.2

Al abrir vitis (preferiblemente en un nuevo workspace), seleccionar *Create Application Project*. En *add* añadir el archivo *ZCU104 Base 2020.2*

![Platform](./Imagenes/Platform.png)

Luego apretar *next* y en *Application project name* dar el nombre "mmult" (puede ser cualquier otro nombre) y apretar *next.

En la ventana *Domain* se deben indicar los archivos que se crearon en la imagen común que se descargó. 
* En *Sysroot path*:  `<PATH_TO_COMMON_IMAGE>/xilinx-zynqmp-common-v2020.2/ir/sysroots/aarch64-xilinx-linux`
* En *Root FS*: `<PATH_TO_COMMON_IMAGE>/xilinx-zynqmp-common-v2020.2/rootfs.ext4`
* En *Kernel Image*: `<PATH_TO_COMMON_IMAGE>/xilinx-zynqmp-common-v2020.2/Image`

Apretar *next* y en la ventana *Template* seleccionar *Empty Application* (debajo de *SW acceleration templates*) y luego apretar *Finish*.


Con el projecto en blanco listo, ahora debemos agregar el código. Primero agregaremos el código que se va a ejecutar en el procesador de la siguiente manera:

![Platform](./Imagenes/ImportSources.png)

En la ventana que se abre navegar a la *srcSW* y seleccionar todos los códigos.

![Platform](./Imagenes/ImportSourcesSW.png)

Luego hay que importar el kernel que se creó en Vitis HLS:

![Platform](./Imagenes/ImportKernel.png)

## Características del código host

https://xilinx.github.io/Vitis-Tutorials/2020-1/docs/my-first-program/host_program.html

https://github.com/Xilinx/Vitis-Tutorials/blob/2021.1/Getting_Started/Vitis/example/src/host.cpp

## Cambiar Frecuencia de Reloj

## Pruebas en la ZCU104

