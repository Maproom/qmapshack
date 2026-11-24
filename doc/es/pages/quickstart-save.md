# Guardar tu trabajo

Esta página muestra cómo guardar tus proyectos en un archivo o en una base de datos.

```{index} single: Proyecto; guardar
```

## Ver los cambios sin guardar

Cuando vuelves a iniciar QMapShack, restaura todos los proyectos del panel **Espacio de trabajo**. Para ello guarda su
propia copia de cada proyecto. El archivo del que se cargó un proyecto solo cambia cuando guardas el proyecto. Un
proyecto que has creado tú no tiene archivo hasta que lo guardas.

1. ```{image} ../images/quickstart-save/quickstart-save-changed.png
    :align: right
    ```

    En el panel **Espacio de trabajo**, busca el proyecto con cambios sin guardar: muestra un botón de guardar en su
    fila, marcado en la imagen. En este ejemplo, **Mis waypoints** tiene un waypoint nuevo y **Example** no tiene
    cambios.

2. Haz clic en el botón de guardar. Si el proyecto no se ha guardado nunca, QMapShack te pregunta primero dónde
    guardarlo; consulta [Guardar en un archivo](#guardar-en-un-archivo). Después de guardar, el botón desaparece.

En la fila de un elemento, el mismo símbolo no es un botón, sino un indicador de cambios. Muestra que el elemento ha
cambiado desde la última vez que se guardó el proyecto.

## Guardar en un archivo

1. Haz clic con el botón derecho en el proyecto, dentro del panel **Espacio de trabajo**, y haz clic en
    **Guardar como...**.
2. En el diálogo, elige el tipo de archivo, escribe un nombre de archivo y haz clic en **Guardar**.

Si el proyecto ya tiene un archivo, haz clic en **Guardar** en el mismo menú. QMapShack sobrescribe el archivo sin
preguntar.

Los tipos de archivo:

- **QMapShack Binary (\*.qms)** es el formato propio de QMapShack. Guarda todo lo que QMapShack sabe de un proyecto,
    incluida la lista de cambios, así que puedes deshacerlos. Úsalo para tus propios proyectos.
- **GPS Exchange Format (\*.gpx \*.GPX)**: también lo leen otros programas y dispositivos. Guarda la lista de cambios,
    pero después de volver a cargar el archivo ya no puedes deshacerlos.

**Guardar como GPX 1.1 (sin extensiones)** en el menú del proyecto escribe un archivo GPX 1.1 simple, sin los datos
propios de QMapShack. Sirve para programas que no leen las extensiones de QMapShack.

Si guardas un proyecto con otro tipo de archivo, QMapShack escribe una copia. El proyecto conserva su propio archivo y
su botón de guardar.

**Archivo → Guardar todos los datos GIS** (**Ctrl+S**) guarda todos los proyectos del espacio de trabajo. Para un
proyecto que no se ha guardado nunca, QMapShack pregunta dónde guardarlo.

Cuando cierras un proyecto con cambios sin guardar, QMapShack pregunta si quieres guardarlo antes.

```{index} single: Proyecto; guardar en base de datos
```

## Guardar en una base de datos

Para guardar en su carpeta un proyecto cargado desde una base de datos, haz clic en su botón de guardar. Consulta
[Cargar y guardar proyectos de la base de datos](quickstart-database.md#cargar-y-guardar-proyectos-de-la-base-de-datos).
Para guardar un archivo en una base de datos, copia su proyecto en el espacio de trabajo como se muestra abajo. También
puedes importar el archivo; consulta [Importar archivos](quickstart-database.md#importar-archivos).

1. ```{image} ../images/quickstart-save/quickstart-save-copy-menu.png
    :align: right
    ```

    Haz clic con el botón derecho en el proyecto, dentro del panel **Espacio de trabajo**, y haz clic en
    **Copiar a...**.

2. ```{image} ../images/quickstart-save/quickstart-save-copy-dialog.png
    :align: right
    ```

    En el diálogo **Seleccionar un proyecto...**, escribe un nombre para el proyecto nuevo, en este ejemplo
    `Vuelta al lago`. Elige **Base de datos** y haz clic en **Aceptar**.

3. ```{image} ../images/quickstart-save/quickstart-save-copy-folder.png
    :align: right
    ```

    En el diálogo **Seleccionar la carpeta superior...**, haz clic en la flecha a la izquierda de la base de datos
    para desplegarla. Selecciona la carpeta donde irá el proyecto, en este ejemplo el grupo **Projects**, y haz
    clic en **Aceptar**.

4. En el diálogo **Carpeta de las bases de datos**, deja seleccionado **Proyecto** y haz clic en **Aceptar**.

5. ```{image} ../images/quickstart-save/quickstart-save-copied.png
    :align: right
    ```

    La copia aparece en el panel **Espacio de trabajo** como **Vuelta al lago @ Projects**, con un botón de guardar.

6. ```{image} ../images/quickstart-save/quickstart-save-copied-db.png
    :align: right
    ```

    En el panel **Bases de datos** aparece también la carpeta **Vuelta al lago**. Todavía está vacía.

7. Haz clic en el botón de guardar de **Vuelta al lago @ Projects** en el panel **Espacio de trabajo**. QMapShack
    escribe los elementos en la carpeta.

El diálogo **Seleccionar un proyecto...** del paso 2 también se abre con **Espacio de trabajo → Añadir proyecto vacío**.
También se abre cuando añades un elemento nuevo a un proyecto nuevo. Elige allí **Base de datos** para crear también ese
proyecto en una base de datos.
