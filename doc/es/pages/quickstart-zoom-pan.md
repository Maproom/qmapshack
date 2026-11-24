# Zoom y desplazamiento

Esta página muestra cómo moverte por el mapa: acercar y alejar, desplazar el mapa y limitar el rango de zoom de un
mapa.

Imágenes: *© OpenStreetMap contributors*.

```{index} single: Mapa; zoom
```

## Zoom con la rueda del ratón

1. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-wheel-before.png
    :align: right
    ```

    Apunta con el ratón al lugar que quieres ver más de cerca, aquí el pueblo de Grän.

2. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-wheel-in.png
    :align: right
    ```

    Gira la rueda del ratón hacia delante para acercar y hacia atrás para alejar. Cada paso de la rueda es un nivel de
    zoom. El lugar bajo el puntero del ratón se queda donde está. Aquí la rueda ha girado dos pasos hacia delante y
    Grän sigue bajo el puntero.

Si el sentido te parece invertido, haz clic en **Vista → Invertir la rueda del ratón**. QMapShack recuerda este
ajuste.

```{index} single: Mapa; desplazar
```

## Desplazar el mapa

- ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-drag.png
    :align: right
    ```

    Arrastra el mapa con el botón izquierdo del ratón: mantén pulsado el botón y mueve el ratón. El mapa se mueve con
    el puntero; suelta el botón cuando veas la parte que buscas. Aquí arrastras el mapa hacia la derecha y Grän se
    mueve junto con el puntero.

```{index} single: Mapa; teclado
```

## Con el teclado

Haz clic una vez en el mapa para que reciba las pulsaciones de teclas. Después pulsa:

| Tecla | Qué hace |
|---|---|
| **+** | Acerca un nivel, centrado en el mapa |
| **-** | Aleja un nivel, centrado en el mapa |
| **Flecha arriba** | Mueve el mapa hacia abajo un cuarto de la vista, para mostrar lo que hay encima |
| **Flecha abajo** | Mueve el mapa hacia arriba un cuarto de la vista, para mostrar lo que hay debajo |
| **Flecha izquierda** | Mueve el mapa a la derecha un cuarto del mapa, para mostrar lo que hay a la izquierda |
| **Flecha derecha** | Mueve el mapa a la izquierda un cuarto del mapa, para mostrar lo que hay a la derecha |

**+** y **-** funcionan en el teclado principal y en el teclado numérico.

```{index} single: Mapa; escala
```

## Niveles de zoom y escala

- ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-scale-bar.png
    :align: right
    ```

    La barra de escala, abajo a la derecha del mapa, indica qué distancia real corresponde a su longitud con el zoom
    actual. Para ocultarla o volver a mostrarla, haz clic en **Vista → Mostrar escala**.

- QMapShack hace zoom en niveles fijos. Para cambiarlos, haz clic en **Vista → Configurar vista** y elige una opción
    en **Escalas**. **Logarítmica** recorre escalas redondas, como 1; 1,5; 2; 3; 5; 7 y 10 metros por píxel.

    **Cuadrada (optimizada para teselas TMS y WMTS)** duplica o reduce a la mitad la escala en cada nivel.

    Los mapas online están hechos de imágenes cuadradas, llamadas teselas, y con estos niveles QMapShack las dibuja
    nítidas. Con **Logarítmica**, QMapShack escala los mapas online y se ven borrosos. Si has añadido los mapas online
    incluidos como se describe en [Activar mapas](quickstart-maps.md), QMapShack ya ha elegido **Cuadrada**.

    ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-scales-setup.png
    :class: below-right
    ```

```{index} single: Mapa; rango de zoom
```

## Limitar un mapa a un rango de zoom

Puedes limitar un mapa a un rango de niveles de zoom. Fuera de ese rango, QMapShack no lo dibuja. Así, por ejemplo,
un mapa detallado puede aparecer solo cuando acercas.

1. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-open.png
    :align: right
    ```

    Haz clic en la flecha a la izquierda del nombre del mapa en el panel **Mapas**. Su configuración se abre debajo.

2. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-buttons.png
    :align: right
    ```

    La barra bajo el control deslizante de opacidad marca el zoom actual con un indicador. El botón a la izquierda de
    la barra convierte el zoom actual en el límite para acercar: más cerca, QMapShack ya no dibuja el mapa. El botón a
    la derecha de la barra hace lo mismo para alejar. Haz clic de nuevo en un botón para quitar su límite.

3. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-set.png
    :align: right
    ```

    Haz clic en el botón derecho. Ahora la parte verde de la barra marca los niveles de zoom en los que QMapShack dibuja
    el mapa.

4. Aleja un nivel más. El indicador de la barra sale del rango verde y QMapShack deja de dibujar el mapa. Vuelve a
    acercar y el mapa reaparece.

    ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-out.png
    :class: below-right
    ```

Para quitar un límite, haz clic de nuevo en su botón.
