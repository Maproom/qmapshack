# Configurar el enrutamiento

Esta página muestra cómo configurar un enrutador para que QMapShack te dibuje un track a lo largo de caminos y
carreteras.

```{index} single: Enrutamiento
```

```{index} single: Enrutador
```

## Qué hace un enrutador

Un *enrutador* calcula el recorrido por caminos y carreteras de un punto al siguiente. Cuando dibujas un track, solo
haces clic en los puntos por los que debe pasar, y el enrutador añade el recorrido entre ellos.

Para añadir el recorrido mientras haces clic, el enrutador tiene que trabajar *al vuelo*: responde al instante, con
datos de enrutamiento de tu ordenador. Un enrutador que consulta un servidor de internet no puede hacerlo. Enviaría una
petición por cada clic, y para evitar abusos, los servidores bloquean a los clientes que envían demasiadas peticiones
en poco tiempo.

```{index} single: Enrutamiento; elegir un enrutador
```

## Elegir un enrutador

QMapShack puede usar dos enrutadores, Routino y BRouter. Selecciona uno en la lista de la parte superior del panel
**Enrutamiento**.

- **Routino (offline)** calcula rutas con datos de tu ordenador. Esos datos los creas a partir de datos de
    OpenStreetMap con **Herramientas → Crear una base de datos de Routino**. Los pasos de esta página usan Routino.
- **BRouter** es más fácil de configurar. En su modo **BRouter (online)**, consulta el servidor de BRouter-Web, así que
    no puede calcular al vuelo. Instalado en tu ordenador, aparece como **BRouter (offline)** y entonces calcula al
    vuelo.

```{index} single: BRouter; instalar
```

## Instalar BRouter

```{image} ../images/quickstart-routing/quickstart-routing-brouter-setup.png
:align: right
```

BRouter funciona con Java. Instala Java primero: sin él, el asistente de configuración no continúa.

Un asistente instala BRouter por ti. Selecciona **BRouter (online)** en la lista y haz clic en **Configuración**. En
el asistente **Configuración de BRouter**, elige **Instalación local** y sigue sus páginas.

El asistente descarga BRouter y los datos de enrutamiento de las zonas que marques en su mapa. Los guarda en la carpeta
`BRouter` de tu [carpeta principal](quickstart-main-window.md#configurar-la-carpeta-principal).

Cuando el asistente termina, la lista muestra **BRouter (offline)**.

```{index} single: Routino; configurar
```

## Configurar Routino

Routino calcula rutas con una base de datos que QMapShack crea a partir de datos de OpenStreetMap. Necesitas un
extracto de tu zona como archivo `.pbf`, por ejemplo de [Geofabrik][GEOFABRIK]. Routino no puede calcular de una base
de datos a otra, así que el extracto debe cubrir todas las zonas que cruza una ruta. Puedes combinar varios archivos
`.pbf` en una base de datos.

En Linux, instala primero el paquete Routino de tu distribución: la herramienta necesita su programa `planetsplitter`.

1. Selecciona **Routino (offline)** en la lista de la parte superior del panel **Enrutamiento**.
2. Haz clic en **Herramientas → Crear una base de datos de Routino**.
3. Haz clic en el botón de carpeta junto a **Seleccionar archivos de origen** y selecciona los archivos `.pbf`.
4. Selecciona dónde va la base de datos en la lista **Carpeta de destino**. La lista ofrece las carpetas registradas
    para Routino, entre ellas la carpeta `Routino` de tu
    [carpeta principal](quickstart-main-window.md#configurar-la-carpeta-principal). El botón de carpeta a la izquierda
    de **Carpeta de destino** añade otras carpetas.
5. Escribe un nombre para la base de datos en el campo **Prefijo del archivo**.
6. Haz clic en **Empezar**. La creación tarda un rato. Cuando la herramienta dice **¡¡¡Hecho!!!**, la base de datos
    aparece en **Base de datos** en el panel **Enrutamiento**.
7. ```{image} ../images/quickstart-routing/quickstart-routing-routino-select.png
    :align: right
    ```

    Selecciona la base de datos en **Base de datos**. Selecciona cómo te desplazas en **Perfil**, por ejemplo
    **A pie** o **Bicicleta**.

Una base de datos creada en otro sitio es un conjunto de cuatro archivos: `<nombre>-nodes.mem`, `<nombre>-segments.mem`,
`<nombre>-ways.mem` y `<nombre>-relations.mem`. Cópialos directamente en la carpeta `Routino`, no en una subcarpeta.
Tras unos segundos, la base de datos aparece en **Base de datos**. Se llama como sus archivos, con espacios en lugar de
guiones bajos: los archivos `tannheimer_tal-*.mem` aparecen como **tannheimer tal**.

[GEOFABRIK]: https://download.geofabrik.de/ "Geofabrik downloads"
