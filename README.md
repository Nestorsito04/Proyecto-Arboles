# Sistema Genealogico Real

Aplicacion en C++ que modela el arbol genealogico de una familia real, calcula la sucesion al trono y ofrece un menu interactivo en consola siguiendo el patron Modelo-Controlador-Vista (MCV).

## Estructura del proyecto

- `main.cpp`: punto de entrada; inicializa la vista.
- `model/`: define entidades (`Person`, `TreeNode`, `FamilyTree`) y operaciones sobre el arbol.
- `controller/`: coordina la carga de datos, la construccion del arbol y la ejecucion de cada opcion del menu.
- `view/`: maneja menu, mensajes y formato de salida.
- `bin/familia_real.csv`: fuente de datos editable.

## Flujo general

1. `runApplication()` (vista) muestra el encabezado del sistema y llama a `initializeSystem()` (controlador).
2. `initializeSystem()` limpia el arbol, carga la informacion desde `bin/familia_real.csv` y construye la estructura con `buildFamilyTree()` (modelo).
3. Si ningun registro tiene `is_king = 1`, `assignNewKing()` busca el primer candidato vivo y lo marca como monarca.
4. El usuario interactua con el menu hasta elegir "Salir"; finalmente `cleanupSystem()` libera los nodos creados.

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
3. **Asignar nuevo rey**: fuerza un recalculo de la sucesion y vuelve a mostrar al rey actualizado.
4. **Buscar persona por ID**: solicita un identificador numerico y muestra sus datos.
5. **Salir**: termina la aplicacion y libera la memoria reservada.

## Reglas de sucesion

- `assignNewKing()` primero busca al rey actual; si existe, desmarca su bandera `is_king` y conserva `was_king = 1`.
- Se prioriza al hijo izquierdo vivo; si no existe, al derecho. En caso contrario se hace una busqueda en anchura (BFS) por el arbol completo para hallar el siguiente candidato vivo.
- Cuando no quedan candidatos vivos, la corona permanece en el monarca actual.

## Compilacion y ejecucion (ejemplo con g++)

```powershell
g++ -std=c++17 -Wall -Icontroller -Imodel -Iview main.cpp \
	controller/controller.cpp view/view.cpp model/model.cpp \
	-o bin/reino
./bin/reino.exe   # Windows
./bin/reino       # Linux/macOS
```

Actualiza rutas o flags segun tu entorno o sistema operativo.

## Mejoras sugeridas

- Validar los campos del CSV antes de insertarlos en el arbol.
- Persistir cambios en los registros (por ejemplo, marcar difuntos) y reescribir el CSV.
- Agregar pruebas unitarias para `assignNewKing()` y `buildFamilyTree()` contemplando casos con datos incompletos.
