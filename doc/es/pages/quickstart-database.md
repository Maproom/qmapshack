# Bases de datos

Esta página muestra cómo guardar proyectos en una base de datos de QMapShack. Las imágenes usan la base de datos de
ejemplo `Example.db`.

```{index} single: Base de datos; añadir
```

## Crear una base de datos

Una base de datos es un solo archivo que contiene tus proyectos, ordenados en carpetas. El panel **Bases de datos**
muestra tus bases de datos.

1. ```{image} ../images/quickstart-database/quickstart-database-add-menu.png
    :align: right
    ```

    Haz clic con el botón derecho en un espacio vacío del panel **Bases de datos** y haz clic en
    **Añadir base de datos**.

2. ```{image} ../images/quickstart-database/quickstart-database-add-dialog.png
    :align: right
    ```

    Escribe `Example` en **Nombre** y deja seleccionado **SQLite**.

3. Haz clic en el botón **+**, marcado en la imagen del paso 2. Elige una carpeta, escribe un nombre de
    archivo y haz clic en **Guardar**.

    El diálogo se abre en la carpeta en la que elegiste una base de datos por última vez. Si has configurado la
    [carpeta principal](quickstart-main-window.md#configurar-la-carpeta-principal), QMapShack lo abre en su carpeta
    `Databases`.

    Si ya tienes un archivo de base de datos, haz clic en su lugar en el botón de carpeta a la derecha del
    botón **+**. Selecciona el archivo y haz clic en **Abrir**.

4. ```{image} ../images/quickstart-database/quickstart-database-panel.png
    :align: right
    ```

    Haz clic en **Aceptar**. La base de datos aparece en el panel **Bases de datos**.

```{index} single: Base de datos; carpeta
```

## Carpetas

Una base de datos guarda sus datos en carpetas. El color de una carpeta indica su tipo:

- Una carpeta **Grupo** (azul) contiene otras carpetas y ayuda a estructurar los datos. No puedes cargarla en el
    espacio de trabajo. Pon las carpetas **Proyecto** dentro de una carpeta **Grupo**, no directamente en la base de
    datos.
- Una carpeta **Proyecto** (verde) contiene tracks, waypoints, rutas y áreas, como un archivo de proyecto. Puedes
    cargarla en el espacio de trabajo.
- Una carpeta **Otros** (naranja) funciona como una carpeta **Proyecto**. Úsala como quieras. Por ejemplo, pon la
    grabación de una excursión en una carpeta **Proyecto** y la planificación en una carpeta **Otros** debajo de ella.

1. ```{image} ../images/quickstart-database/quickstart-database-folder-menu.png
    :align: right
    ```

    Haz clic con el botón derecho en la base de datos **Example** y haz clic en **Añadir carpeta**.

2. ```{image} ../images/quickstart-database/quickstart-database-folder-dialog.png
    :align: right
    ```

    Escribe `Excursiones` en **Nombre de la carpeta** y elige **Grupo**.

3. ```{image} ../images/quickstart-database/quickstart-database-folder-group.png
    :align: right
    ```

    Haz clic en **Aceptar**. El grupo **Excursiones** aparece debajo de **Example**.

4. ```{image} ../images/quickstart-database/quickstart-database-folder-project.png
    :align: right
    ```

    Haz clic con el botón derecho en **Excursiones** y haz clic en **Añadir carpeta**. Escribe
    `Excursión de montaña`, elige **Proyecto** y haz clic en **Aceptar**.

5. ```{image} ../images/quickstart-database/quickstart-database-folders.png
    :align: right
    ```

    Haz clic con el botón derecho en **Excursión de montaña** y haz clic en **Añadir carpeta**. Escribe
    `Planificación`, elige **Otros** y haz clic en **Aceptar**.

```{index} single: Base de datos; importar
```

## Importar archivos

Puedes guardar archivos de proyecto en una base de datos sin cargarlos en el espacio de trabajo.

1. ```{image} ../images/quickstart-database/quickstart-database-import-menu.png
    :align: right
    ```

    Haz clic con el botón derecho en la carpeta que recibirá los proyectos, en este ejemplo el grupo **Excursiones**, y
    haz clic en **Importar desde archivos...**.

2. Selecciona uno o varios archivos, por ejemplo `Vilsalpsee.gpx`, y haz clic en **Abrir**.

3. ```{image} ../images/quickstart-database/quickstart-database-imported.png
    :align: right
    ```

    QMapShack crea una carpeta **Proyecto** para cada archivo dentro de la carpeta en la que hiciste clic con el botón
    derecho. En este ejemplo, **Vilsalpsee** aparece debajo de **Excursiones**. La carpeta toma el nombre del proyecto
    del archivo. QMapShack omite los proyectos que ya están en la base de datos.

```{index} single: Base de datos; exportar
```

## Exportar a GPX

Puedes exportar una base de datos, o una de sus carpetas, a archivos GPX, por ejemplo para compartir los datos.

1. ```{image} ../images/quickstart-database/quickstart-database-export-menu.png
    :align: right
    ```

    Haz clic con el botón derecho en la base de datos **Example** y haz clic en **Exportar a GPX...**.

2. ```{image} ../images/quickstart-database/quickstart-database-export-dialog.png
    :align: right
    ```

    Haz clic en el botón de carpeta marcado en la imagen y elige una carpeta de tu ordenador. Marca
    **GPX 1.1 sin extensiones** para dejar fuera los datos propios de QMapShack.

3. Haz clic en **Empezar**. QMapShack crea una carpeta para la base de datos y para cada grupo, y un archivo GPX para
    cada carpeta **Proyecto** y **Otros**. El nombre de archivo de una carpeta **Otros** empieza por el nombre de la
    carpeta superior.

4. Cuando aparezca **¡Hecho!**, haz clic en **Cerrar**.

    ```{image} ../images/quickstart-database/quickstart-database-exported.png
    :class: below-right
    ```

```{index} single: Base de datos; espacio de trabajo
```

## Cargar y guardar proyectos de la base de datos

La base de datos guarda tus datos. Los cambias en el espacio de trabajo: carga un proyecto, cámbialo, guárdalo en la
base de datos y quítalo después del espacio de trabajo.

1. ```{image} ../images/quickstart-database/quickstart-database-loaded-db.png
    :align: right
    ```

    Haz clic en el botón de carga marcado en la imagen, en el extremo derecho de la fila de **Vilsalpsee**.

2. ```{image} ../images/quickstart-database/quickstart-database-loaded-wks.png
    :align: right
    ```

    El proyecto aparece en el panel **Espacio de trabajo** como **Vilsalpsee @ Excursiones**: su nombre y la carpeta
    en la que está.

3. Cambia el proyecto como cambiarías un archivo de proyecto, por ejemplo
    [añadiendo un waypoint](quickstart-waypoints.md). Después haz clic en el botón de guardar en la fila del proyecto
    en el panel **Espacio de trabajo**. QMapShack escribe los cambios en la base de datos.

4. Vuelve a hacer clic en el botón de carga del panel **Bases de datos** para quitar el proyecto del espacio de trabajo.
    Si el proyecto tiene cambios sin guardar, QMapShack pregunta antes: **Guardar** los guarda, **No** los descarta e
    **Interrumpir** deja el proyecto cargado.

Si quitas un elemento de un proyecto en el espacio de trabajo, sigue en la base de datos, incluso después de guardar.
Para borrarlo de una carpeta, haz clic con el botón derecho en él en el panel **Bases de datos** y haz clic en
**Eliminar elemento**. Un elemento que ya no está en ninguna carpeta pasa a la carpeta **Papelera**. Si lo borras allí,
se elimina definitivamente.
