# Activar un mapa de Garmin

Esta página muestra cómo añadir un mapa de Garmin a QMapShack, activarlo y ajustar lo que muestra.

Imágenes: *© OpenStreetMap contributors, created by BBBike*.

```{index} single: Mapa; Garmin
```

```{index} single: Mapa; vectorial
```

## Mapas de Garmin

Un mapa online es un conjunto de imágenes que QMapShack carga mientras lo miras. Un mapa de Garmin, en cambio, es un
mapa vectorial: un archivo de carreteras, caminos, áreas y puntos que QMapShack dibuja por sí mismo. No necesita
conexión a internet.

Los mapas de Garmin son archivos `.img`, y son los únicos mapas vectoriales que QMapShack lee. El ejemplo de esta
página es un extracto de datos de OpenStreetMap, hecho con el [servicio de extractos de BBBike][BBBIKE]. Allí puedes
hacer uno de tu propia zona.

QMapShack no puede abrir dos tipos de archivos `.img`:

- Mapas en el formato NT más reciente de Garmin, que también usan algunos mapas libres. QMapShack avisa
    **El archivo está en formato NT.**
- Mapas bloqueados, como los que vende Garmin. QMapShack avisa de que el archivo contiene datos bloqueados o
    cifrados.

```{index} single: Mapa; añadir
```

## Añadir el mapa

QMapShack muestra cada mapa que está directamente en una de sus carpetas de mapas. La carpeta `Maps` de tu
[carpeta principal](quickstart-main-window.md#configurar-la-carpeta-principal) es una de ellas.

1. Copia el archivo `.img` en la carpeta `Maps`.
2. ```{image} ../images/quickstart-vector-map/quickstart-vector-map-listed.png
    :align: right
    ```

    Tras unos segundos, el mapa aparece en la lista del panel **Mapas**, en gris como todo mapa desactivado. Se
    llama como su archivo, sin el tipo de archivo y con espacios en lugar de guiones bajos: QMapShack muestra
    `Tannheimer_Tal.img` como **Tannheimer Tal**.
3. Para activar el mapa, haz clic en el botón de su fila. La fila pasa a negrita y muestra **activo**.

    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-map-on.png
    :class: below-right
    ```

```{index} single: Mapa; detalle
```

## Ajustar el detalle

Un mapa de Garmin contiene varios niveles de detalle, y QMapShack elige uno según el zoom. Puedes ajustar esa
elección en cada mapa.

1. ```{image} ../images/quickstart-vector-map/quickstart-vector-map-settings.png
    :align: right
    ```

    Haz clic en la flecha a la izquierda del nombre del mapa para abrir su configuración.

- **Detalle** fija cuánto muestra el mapa: por encima de 0, más de lo habitual con el mismo zoom; por debajo de 0,
    menos. 0 es el valor predeterminado. En la imagen de abajo, **Detalle** está en 5.

    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-detail-more.png
    :class: below-right
    ```

- Marca o desmarca **Áreas**, **Líneas** y **Puntos** para mostrar u ocultar esas partes del mapa. Sin áreas, por
    ejemplo, desaparecen los bosques y los caminos destacan.

    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-no-areas.png
    :class: below-right
    ```

```{index} single: Mapa; archivo TYP
```

## El aspecto del mapa: archivos TYP

Los colores y símbolos de un mapa de Garmin vienen de un archivo TYP. La mayoría de los mapas de Garmin traen el suyo
propio, como este.

- ```{image} ../images/quickstart-vector-map/quickstart-vector-map-typ-row.png
    :align: right
    ```

    Al final de la configuración del mapa, **Archivo TYP** muestra un archivo TYP externo en cuanto cargas uno. Si el
    campo está vacío, el mapa usa el suyo propio.
- Para cargar otro, haz clic en el botón con el icono de carpeta y selecciona un archivo `.typ`. Haz clic en el botón
    con el icono de cruz para volver a usar el archivo TYP propio del mapa.
- Un archivo TYP está hecho para un estilo de mapa. Uno hecho para otro mapa puede dar a este mapa colores y
    símbolos equivocados.

[BBBIKE]: https://extract.bbbike.org/
