# Usar un dispositivo GPS

Esta página muestra cómo pasar tus tracks y waypoints a un dispositivo GPS de Garmin y cómo obtener los tracks que ha
grabado.

```{index} single: Dispositivo; conectar
```

## Conectar el dispositivo

Esta página usa un dispositivo de Garmin conectado como almacenamiento masivo USB, que tu ordenador muestra como una
unidad. QMapShack trata igual los dispositivos que se conectan por MTP (Media Transfer Protocol) y los de otras marcas.
Lo que necesita tu ordenador para cada dispositivo está en la página de la wiki [GPS Devices][WIKI_DEVICES] (en inglés).

1. Conecta el dispositivo a tu ordenador con su cable USB.
2. ```{image} ../images/quickstart-device/quickstart-device-connected.png
    :align: right
    ```

    El dispositivo aparece en el panel **Espacio de trabajo** con el nombre de su modelo. Debajo, QMapShack muestra
    cada archivo de tracks y waypoints del dispositivo como un proyecto.

Si el dispositivo no aparece, haz clic en **Espacio de trabajo → Configurar espacio de trabajo** y comprueba que
**Activar compatibilidad con dispositivos** esté marcada en la pestaña **General**. Viene marcada de forma
predeterminada.

```{index} single: Dispositivo; enviar proyecto
```

## Enviar un proyecto al dispositivo

1. ```{image} ../images/quickstart-device/quickstart-device-send-menu.png
    :align: right
    ```

    Haz clic con el botón derecho en el proyecto, dentro del panel **Espacio de trabajo**, y haz clic en
    **Enviar a dispositivo**. Solo puedes hacer clic en esta entrada mientras haya un dispositivo conectado.

2. ```{image} ../images/quickstart-device/quickstart-device-sent.png
    :align: right
    ```

    El proyecto aparece debajo del dispositivo. QMapShack lo guarda en el dispositivo como un archivo GPX con el nombre
    del proyecto, en este ejemplo `Example.gpx`. En el nombre de archivo, QMapShack quita todos los caracteres excepto
    letras, dígitos y `_`.

Si hay más de un dispositivo conectado, QMapShack pregunta a cuáles enviar el proyecto. Si vuelves a enviar el
proyecto, se sustituye su copia en el dispositivo.

```{index} single: Dispositivo; obtener grabación
```

## Obtener una grabación del dispositivo

1. ```{image} ../images/quickstart-device/quickstart-device-recording.png
    :align: right
    ```

    Haz clic en la flecha a la izquierda del proyecto con tu grabación, en este ejemplo
    **Track 2025-06-14 101530**, para ver su track.

2. ```{image} ../images/quickstart-device/quickstart-device-copy-menu.png
    :align: right
    ```

    No puedes cambiar los elementos del dispositivo, así que copia el track al espacio de trabajo. Haz clic con el
    botón derecho en el track y haz clic en **Copiar a...**.

3. ```{image} ../images/quickstart-device/quickstart-device-copy-dialog.png
    :align: right
    ```

    En el diálogo **Seleccionar un proyecto...**, selecciona un proyecto del espacio de trabajo o escribe un nombre
    para uno nuevo, en este ejemplo `Mis grabaciones`. Haz clic en **Aceptar**.

4. ```{image} ../images/quickstart-device/quickstart-device-copied.png
    :align: right
    ```

    El track está ahora en el proyecto del espacio de trabajo, donde puedes cambiarlo. Guarda el proyecto para
    conservarlo; consulta [Guardar tu trabajo](quickstart-save.md).

[WIKI_DEVICES]: https://github.com/Maproom/qmapshack/wiki/DocGisDevices
