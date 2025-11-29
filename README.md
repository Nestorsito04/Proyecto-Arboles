# Sistema Genealogico Real

Aplicacion en C++ que modela el arbol genealogico de una familia real, calcula la sucesion al trono y ofrece un menu interactivo en consola siguiendo el patron Modelo-Controlador-Vista (MCV).

## Estructura del proyecto

- `main.cpp`: punto de entrada; inicializa la vista.
- `model/`: define entidades (`Person`, `TreeNode`, `FamilyTree`) y operaciones sobre el arbol. Cada `TreeNode` mantiene un vector dinámico de hijos para soportar cualquier cantidad de descendientes por persona.
- `controller/`: coordina la carga de datos, la construccion del arbol y la ejecucion de cada opcion del menu.
- `view/`: maneja menu, mensajes y formato de salida.
- `bin/familia_real.csv`: fuente de datos editable.

## Flujo general

1. `runApplication()` (vista) muestra el encabezado del sistema y llama a `initializeSystem()` (controlador).
2. `initializeSystem()` limpia el arbol, carga la informacion desde `bin/familia_real.csv` y construye la estructura con `buildFamilyTree()` (modelo).
3. Si ningun registro tiene `is_king = 1`, `assignNewKing()` corona automaticamente a la raiz si es candidata; de lo contrario continúa con las reglas de sucesion hasta encontrar al primer heredero disponible.
4. El usuario interactua con el menu hasta elegir "Salir"; finalmente `cleanupSystem()` libera los nodos creados.
5. Cualquier edicion de personas o reasignacion de la corona sincroniza las estructuras del arbol con el arreglo original de personas y reescribe `bin/familia_real.csv` desde ese arreglo, lo que evita perder registros que no se encuentren en el arbol binario.

## Formato del CSV

Cada fila (despues del encabezado) debe seguir la estructura:

```
id,nombre,apellido,genero,edad,id_padre,is_dead,was_king,is_king
```

- `id_padre = -1` o `0` identifica a la raiz del arbol.
- `is_dead`, `was_king` e `is_king` aceptan `0` (falso) o `1` (verdadero).
- El archivo debe residir en `bin/familia_real.csv` o, en su defecto, se debe actualizar la ruta utilizada en `controller/controller.cpp`.

## Opciones del menu

1. **Mostrar arbol genealogico**: imprime el arbol con indentacion y etiquetas (rey actual, ex-rey, difunto).
2. **Mostrar rey actual**: despliega la informacion del monarca vigente.
3. **Mostrar linea de sucesion**: lista, en orden, a los candidatos vivos junto con su relacion con el rey.
4. **Asignar nuevo rey**: fuerza un recalculo de la sucesion y vuelve a mostrar al rey actualizado.
   - El algoritmo salta automaticamente a la rama del siguiente hijo una vez que todos los integrantes vivos de la rama primogenita ya reinaron (tienen `was_king = 1`).
5. **Buscar persona por ID**: solicita un identificador numerico y muestra sus datos.
6. **Salir**: termina la aplicacion y libera la memoria reservada.

## Reglas de sucesion

1. Localizar al rey actual; si no hay ninguno, se intenta coronar a la raiz viva.
2. Si el monarca necesita relevo (por muerte o edad ≥ 70) o el usuario lo fuerza, se exploran las ramas en este orden:
   - **Descendencia del actual (primogenitura estricta):** `findFirstLivingMaleInLine()` recorre primero al hijo izquierdo y baja por sus descendientes antes de considerar el derecho. Solo se consideran varones vivos < 70 sin `was_king`.
   - **Hermanos:** se elige el siguiente hijo del padre original si es elegible; de lo contrario se exploran sus descendientes. Exreyes no pueden coronarse de nuevo, lo que evita ciclos entre hermanos.
   - **Tios y ramas colaterales:** se aplica la misma regla, permitiendo ascender desde un ancestro con dos hijos hacia la rama secundaria cuando la primaria se agota.
   - **Lineas secundarias y mujeres:** si no quedan varones elegibles en ninguna rama, se buscan candidatos en lineas secundarias y finalmente mujeres vivas (15‑69 años) sin `was_king`.
3. Cuando no se encuentra sucesor, se desmarca al rey actual solo si la regla de reemplazo lo exige; en caso contrario se mantiene la corona.
4. Todos los candidatos proclamados reciben `is_king = 1` y `was_king = 1`, lo que bloquea su reeleccion y permite avanzar a la siguiente rama en invocaciones sucesivas de la opcion 4.

## Manejo de entradas

- La vista usa `getline` + `stoi` para leer cada opcion del menu y los identificadores solicitados, lo que evita que el buffer de entrada quede en mal estado.
- Ante caracteres no numericos se muestra un mensaje de error y se vuelve a solicitar la opcion sin interrumpir la ejecucion.

## Compilacion y ejecucion (ejemplo con g++)

```powershell
g++ main.cpp controller/controller.cpp view/view.cpp model/model.cpp -o ../bin/programa # Windows
./../bin/reino.exe   # Windows
./bin/reino       # Linux/macOS
```

Actualiza rutas o flags segun tu entorno o sistema operativo.

## Mejoras sugeridas

- Validar los campos del CSV antes de insertarlos en el arbol.
- Persistir cambios en los registros (por ejemplo, marcar difuntos) y reescribir el CSV.
- Agregar pruebas unitarias para `assignNewKing()` y `buildFamilyTree()` contemplando casos con datos incompletos.
