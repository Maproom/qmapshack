# Cargar un archivo GPX

Esta página muestra cómo cargar un archivo GPX en QMapShack, ver sus tracks y waypoints y encontrarlos en el mapa.

Imágenes: *© OpenStreetMap contributors*; *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0*;
*`Example.gpx`: fuzzybear, CC BY-SA 4.0*.

```{index} single: GPX
```

```{index} single: Archivo; cargar
```

## Cargar el archivo

Un archivo GPX (GPS Exchange Format) contiene tracks, waypoints y rutas. Los dispositivos GPS, las aplicaciones y los
sitios web de planificación de rutas lo usan para guardarlos y compartirlos. Las imágenes de esta página muestran
`Example.gpx`, una excursión alrededor del Aggenstein.

1. Haz clic en **Archivo → Cargar datos GIS** o pulsa **Ctrl+L**.
2. Selecciona el archivo GPX y haz clic en **Abrir**. Para cargar varios archivos a la vez, mantén pulsada **Ctrl** y
    haz clic en cada archivo.

El diálogo se abre en la carpeta desde la que cargaste o en la que guardaste por última vez. Si has configurado la
[carpeta principal](quickstart-main-window.md#configurar-la-carpeta-principal), QMapShack abre el diálogo en su
carpeta `GPX`, un buen sitio para guardar tus archivos.

En lugar de usar el menú, puedes arrastrar archivos desde tu gestor de archivos al mapa.

QMapShack también carga sus propios archivos `.qms` y archivos de otros dispositivos y programas: `.fit`, `.tcx`,
`.slf`, `.sml`, `.log` y `.qlb`.

```{index} single: Proyecto
```

```{index} single: Espacio de trabajo
```

## El proyecto en el espacio de trabajo

QMapShack carga un archivo como *proyecto*: los tracks, rutas, waypoints y áreas de un archivo, reunidos. El panel
**Espacio de trabajo** lo muestra con el nombre guardado en el archivo, o con el nombre del archivo si no tiene ninguno.
QMapShack muestra en el mapa todo el contenido del proyecto de inmediato. Consulta también
[La ventana principal](quickstart-main-window.md#la-ventana-en-el-primer-inicio).

1. ```{image} ../images/quickstart-load-file/quickstart-load-file-items.png
    :align: right
    ```

    Haz clic en la flecha a la izquierda del nombre del proyecto. Debajo aparecen sus tracks, rutas, waypoints y
    áreas. Los tracks, rutas y áreas tienen un icono según su tipo, y un waypoint muestra su símbolo.

2. Mantén el puntero del ratón sobre el nombre de un elemento (un track, ruta, waypoint o área). Aparece un
    cuadro emergente con un resumen del elemento.

```{index} single: Elemento; encontrar en el mapa
```

## Encontrar un elemento en el mapa

```{image} ../images/quickstart-load-file/quickstart-load-file-dclick.png
:align: right
```

Haz doble clic en un elemento del panel **Espacio de trabajo**. El mapa hace zoom sobre él.

Si es un track, QMapShack además lo resalta en el mapa. Un cuadro abajo a la derecha muestra un resumen del track:
longitud, ascenso, tiempo, velocidad y más. Un pequeño gráfico en la esquina inferior izquierda muestra el perfil de
altitud del track.

```{image} ../images/quickstart-load-file/quickstart-load-file-zoomed.png
:class: below-right
```

```{index} single: Proyecto; ocultar en el mapa
```

## Ocultar un proyecto en el mapa

1. ```{image} ../images/quickstart-load-file/quickstart-load-file-hide.png
    :align: right
    ```

    Haz clic en el botón del ojo, en el extremo derecho de la fila del proyecto. Todo el contenido del proyecto
    desaparece del mapa, y el proyecto y sus elementos se vuelven grises.

2. Haz clic de nuevo en el botón del ojo para mostrar el proyecto.

Mientras un proyecto está oculto, hacer doble clic en uno de sus elementos no mueve el mapa.

```{index} single: Espacio de trabajo; conservar para el próximo inicio
```

## Conservar el espacio de trabajo

QMapShack conserva los proyectos del panel **Espacio de trabajo** para el próximo inicio.

Para ello, guarda su propia copia de cada proyecto y no modifica el archivo que cargaste. QMapShack solo escribe un
cambio en el archivo cuando guardas el proyecto.

QMapShack guarda el espacio de trabajo cada 5 minutos y al cerrar el programa. Para cambiar la frecuencia:

1. Haz clic en **Espacio de trabajo → Configurar espacio de trabajo**.
2. En la pestaña **General**, ajusta los minutos junto a **guardar el espacio de trabajo al salir y cada**. Si lo
    pones a 0, QMapShack guarda el espacio de trabajo solo al cerrar el programa.
3. Haz clic en **Aceptar**, confirma el mensaje y reinicia QMapShack. El nuevo ajuste solo se aplica tras reiniciar.

Si desmarcas la casilla delante de **guardar el espacio de trabajo al salir y cada**, QMapShack deja de guardar el
espacio de trabajo. En el próximo inicio, el espacio de trabajo está como se guardó por última vez.
