# Añadir waypoints

Esta página muestra cómo añadir un waypoint al mapa. Las imágenes usan el mapa de [Activar mapas](quickstart-maps.md),
el DEM de [Datos de altitud y sombreado](quickstart-dem.md) y `Example.gpx` de
[Cargar un archivo GPX](quickstart-load-file.md).

Imágenes: *© OpenStreetMap contributors*; *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0*;
*`Example.gpx`: fuzzybear, CC BY-SA 4.0*.

```{index} single: Waypoint; añadir
```

## Añadir un waypoint

1. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-add-waypoint.png
    :align: right
    ```

    Haz clic con el botón derecho en el lugar del mapa donde debe ir el waypoint y haz clic en **Añadir waypoint**.
    Si mantienes pulsado un momento el botón izquierdo del ratón sobre el mapa, se abre el mismo menú.

2. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-project.png
    :align: right
    ```

    Selecciona el proyecto al que va el waypoint, o escribe un nombre para un proyecto nuevo en
    **Nombre del nuevo proyecto**.

    Para un proyecto nuevo, después de escribir el nombre, elige en **El nuevo proyecto se creará como** cómo lo guarda
    QMapShack. **\*.qms** es el formato de archivo propio de QMapShack, **\*.gpx** un archivo GPX que también leen
    otros programas y dispositivos. QMapShack preselecciona uno, así que comprueba que sea el que quieres. La tercera
    opción, **Base de datos**, se explica en [Bases de datos](quickstart-database.md).

    Haz clic en **Aceptar**.

3. Se abre la ventana de detalles del waypoint. Muestra su nombre, símbolo, posición, altitud y hora.

    ```{image} ../images/quickstart-waypoints/quickstart-waypoints-details.png
    :class: below-right
    ```

    La **Altitud** viene del DEM; consulta [Datos de altitud y sombreado](quickstart-dem.md).

4. Haz clic en el nombre, escribe uno nuevo y pulsa **Entrar**.

    ```{image} ../images/quickstart-waypoints/quickstart-waypoints-named.png
    :class: below-right
    ```

5. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-symbol.png
    :align: right
    ```

    Haz clic en el símbolo a la izquierda del nombre para elegir otro.

6. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-closed.png
    :align: right
    ```

    Pulsa **Esc** para cerrar la ventana. El waypoint aparece ahora en el mapa con su nombre.

7. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-saved.png
    :align: right
    ```

    El panel **Espacio de trabajo** muestra ahora el waypoint bajo su proyecto.

El botón de guardar en la fila del proyecto, marcado en la imagen, indica que el proyecto tiene cambios sin guardar en
un archivo. Haz clic en él para guardar el proyecto.

La primera vez que guardas el proyecto, QMapShack pregunta dónde guardarlo. El diálogo propone el nombre del proyecto
como nombre de archivo. Se abre en la carpeta desde la que cargaste o en la que guardaste por última vez. Si has
configurado la [carpeta principal](quickstart-main-window.md#configurar-la-carpeta-principal), QMapShack abre el
diálogo en su carpeta `GPX`.

En la fila del waypoint, el mismo símbolo no es un botón, sino un indicador de cambios. Muestra que el waypoint ha
cambiado desde la última vez que se guardó el proyecto.

El primer waypoint se llama `wpt`. Un waypoint nuevo recibe el último símbolo que elegiste y el último nombre que
diste. Si ese nombre termina en un número, QMapShack lo incrementa: después de `Refugio 3` viene `Refugio 4`.
