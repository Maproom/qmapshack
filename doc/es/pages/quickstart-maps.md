# Activar mapas

Esta página muestra cómo poner tus primeros mapas en QMapShack y cómo mostrarlos.

Imágenes: *© OpenStreetMap contributors*; *OpenTopoMap, CC-BY-SA*; *trails by waymarkedtrails.org*.

```{index} single: Mapa; online
```

## Obtener los mapas incluidos

QMapShack trae cinco mapas online, entre ellos OpenStreetMap y OSM Topo. Un mapa online necesita conexión a internet:
QMapShack carga el mapa mientras lo miras.

1. Haz clic en **Archivo → Configurar rutas a los mapas**. La misma entrada aparece en el menú que se abre al hacer
    clic con el botón derecho en la lista del panel **Mapas**.
2. ```{image} ../images/quickstart-maps/quickstart-maps-map-path-setup.png
    :align: right
    ```

    Haz clic en **¡Quiero mapas! ¡No quiero ver la documentación!**

3. QMapShack pregunta **¿Dónde quieres guardar los mapas?** y ya te propone la carpeta `Maps` de tu
    [carpeta principal](quickstart-main-window.md#configurar-la-carpeta-principal).
    Confírmala. El diálogo **Configurar rutas a los mapas** se cierra solo.
4. ```{image} ../images/quickstart-maps/quickstart-maps-maps-panel.png
    :align: right
    ```

    El panel **Mapas** lista los mapas. Un mapa aparece en gris mientras está desactivado.

    QMapShack nombra cada mapa según su archivo, sin el tipo de archivo y con espacios en lugar de guiones bajos:
    `OSM_Topo.tms` aparece como **OSM Topo**. Los datos de altitud y los archivos POI se nombran igual.

```{index} single: Mapa; activar
```

## Activar un mapa

1. ```{image} ../images/quickstart-maps/quickstart-maps-osm-button.png
    :align: right
    ```

    Para activar un mapa, haz clic en el botón de su fila en el panel **Mapas**. Vuelve a hacer clic para
    desactivarlo.

2. Activa **OpenStreetMap**. Su fila pasa a negrita y dice **activo**, y el mapa sustituye a la página de bienvenida.

    ```{image} ../images/quickstart-maps/quickstart-maps-osm-on.png
    :class: below-right
    ```

```{index} single: Mapa; superponer
```

## Superponer mapas

Cuando hay varios mapas activados, se superponen en el orden de la lista: un mapa tapa los mapas que están por encima
de él.

1. Activa también **OSM Topo**. Está debajo de **OpenStreetMap** en la lista, así que **OSM Topo** tapa
    **OpenStreetMap**.

    ```{image} ../images/quickstart-maps/quickstart-maps-both-on.png
    :class: below-right
    ```

2. Para cambiar el orden, arrastra un mapa hacia arriba o hacia abajo en la lista. O selecciónalo y pulsa
    **Mayús+Flecha arriba** o **Mayús+Flecha abajo**; el menú contextual también tiene **Subir** y **Bajar**.
    Con **OSM Topo** más arriba, **OpenStreetMap** lo tapa.

    ```{image} ../images/quickstart-maps/quickstart-maps-topo-up.png
    :class: below-right
    ```

```{index} single: Mapa; opacidad
```

## Hacer un mapa transparente

1. Para ver el mapa de abajo a través del de arriba, haz clic en la flecha a la izquierda del nombre del mapa de
    arriba. Su configuración se abre debajo de la fila.
2. Arrastra hacia la izquierda el control deslizante bajo el nombre para bajar la opacidad del mapa. Aquí
    **OSM Topo** está semitransparente sobre **OpenStreetMap**.

    ```{image} ../images/quickstart-maps/quickstart-maps-topo-half.png
    :class: below-right
    ```
