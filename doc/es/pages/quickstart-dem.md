# Datos de altitud y sombreado

Esta página muestra cómo añadir datos de altitud a QMapShack, activarlos y sombrear el terreno con ellos.

Imágenes: *© OpenStreetMap contributors*; *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0*.

```{index} single: DEM
```

```{index} single: Altitud; bajo el puntero del ratón
```

## Para qué sirven los datos de altitud

Los datos de altitud, un DEM (modelo digital del terreno; el programa lo llama MDT), contienen la altura del terreno.
QMapShack los usa de tres maneras:

- Muestra la altitud de cualquier punto. Con un DEM activado, la barra de estado abajo a la derecha muestra la
    altitud (**Altitud**) y la pendiente (**Pendiente**) bajo el puntero del ratón.

    ```{image} ../images/quickstart-dem/quickstart-dem-status-bar.png
    :class: below-right
    ```

- Si lo pides, añade una altitud a tracks y waypoints, o sustituye la que tienen. Un track que dibujas toma sus
    altitudes del DEM al guardarlo; consulta
    [Dibujar un track](quickstart-draw-track.md#altitudes-del-dem). Las rutas toman las suyas del DEM cuando
    QMapShack las calcula.
- Sombrea el terreno en el mapa, para que destaquen montañas y valles.

```{index} single: DEM; añadir
```

## Añadir un DEM

Los datos de altitud suelen venir en uno o varios archivos de imagen, por ejemplo GeoTIFF. QMapShack no los abre
directamente: lee un archivo `.vrt` (un ráster virtual), que lista los archivos de imagen y los une en un solo DEM.

1. Copia los archivos en la carpeta `DEM` de tu
    [carpeta principal](quickstart-main-window.md#configurar-la-carpeta-principal): un solo archivo directamente en
    ella, varios archivos en una subcarpeta propia.
2. Haz clic en **Herramientas → Asistente VRT**.
3. Haz clic en el botón de carpeta junto a **Seleccionar archivos de origen** y selecciona los archivos que has
    copiado.
4. Haz clic en el botón de carpeta junto a **Archivo VRT de salida**. La primera vez, el diálogo se abre en la carpeta
    `Maps`: ve a la carpeta `DEM`, escribe un nombre que termine en `.vrt` y guarda.
5. Haz clic en **Empezar**.
6. ```{image} ../images/quickstart-dem/quickstart-dem-listed.png
    :align: right
    ```

    Cuando el Asistente VRT termina, el DEM aparece en la lista, en gris como todo DEM desactivado.

Un `.vrt` creado en otro sitio puede apuntar a sus archivos por su ruta completa, así que quizá no los encuentre
después de copiarlo. Si lo creas en la carpeta `DEM`, esto no pasa.

```{index} single: DEM; activar
```

## Activar el DEM

1. ```{image} ../images/quickstart-dem/quickstart-dem-dem-button.png
    :align: right
    ```

    Haz clic en el botón de la fila del DEM en el panel **MDT (DEM)**. La fila pasa a negrita y dice **activo**.

2. El mapa se ve igual que antes: el DEM no sombrea nada hasta que activas el sombreado. Pero donde el DEM tiene
    datos, la barra de estado ya muestra la altitud y la pendiente bajo el puntero. Si no, algo falla en el DEM.

```{index} single: DEM; sombreado
```

```{index} single: Sombreado
```

## Sombrear el terreno

1. ```{image} ../images/quickstart-dem/quickstart-dem-hillshading.png
    :align: right
    ```

    Haz clic en la flecha a la izquierda del nombre del DEM. Su configuración se abre debajo. Marca **Sombreado**.

2. ```{image} ../images/quickstart-dem/quickstart-dem-shaded.png
    :align: right
    ```

    Ahora el terreno aparece sombreado en el mapa: las laderas que no miran a la luz son más oscuras.

3. El control deslizante bajo el nombre del DEM ajusta su opacidad. QMapShack dibuja el DEM sobre el mapa. Por
    eso, al arrastrar el control hacia la derecha, el sombreado se hace más fuerte y el mapa más oscuro.

    El control deslizante junto a **Sombreado** cambia el propio relieve. Hacia la izquierda, las laderas parecen más
    empinadas; hacia la derecha, el terreno parece más llano. La luz siempre viene del noroeste.

    ```{image} ../images/quickstart-dem/quickstart-dem-stronger.png
    :class: below-right
    ```

Igual que con un mapa, puedes limitar un DEM a un rango de zoom. Usa los dos botones junto a la barra bajo el control
deslizante. Consulta [Limitar un mapa a un rango de zoom](quickstart-zoom-pan.md#limitar-un-mapa-a-un-rango-de-zoom).
