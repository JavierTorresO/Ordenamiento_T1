# Analisis teorico de las versiones paralelas

## 1. Mergesort paralelo

La version paralela divide el arreglo en dos mitades y ordena ambas mitades en paralelo usando tareas OpenMP. El merge final de cada llamada recursiva sigue siendo secuencial.

- PRAM: las dos llamadas recursivas son paralelizables; el merge de cada nivel limita la escalabilidad.
- Work:
  `W(n) = 2W(n/2) + O(n) = O(n log n)`
- Span:
  `S(n) = S(n/2) + O(n) = O(n)`
- Brent:
  `T_p = O(W(n)/p + S(n)) = O((n log n)/p + n)`

Partes naturalmente paralelizables:
- ordenamiento de la mitad izquierda y derecha;
- niveles bajos del arbol recursivo.

Cuello de botella:
- el merge secuencial en los niveles superiores.

## 2. K-way mergesort paralelo

La version paralela divide el arreglo en `k` bloques, ordena cada bloque en paralelo y luego realiza merges por rondas de pares.

- PRAM: los `k` subproblemas se pueden resolver en paralelo.
- Work:
  para `k` constante, cada nivel sigue procesando `O(n)` trabajo total, por lo que
  `W(n) = kW(n/k) + O(n) = O(n log n)`
- Span:
  el merge por rondas sigue teniendo una fase secuencial dominante por nivel, por lo que para `k` constante
  `S(n) = S(n/k) + O(n) = O(n)`
- Brent:
  `T_p = O((n log n)/p + n)`

Partes naturalmente paralelizables:
- ordenamiento independiente de cada bloque;
- merges de pares disjuntos dentro de una misma ronda.

Cuellos de botella:
- las rondas de merge deben ejecutarse una tras otra;
- si el merge interno es secuencial, los niveles altos vuelven a limitar la aceleracion.

## 3. Parallel merge por particiones y ranks

La implementacion usa particiones tipo merge-path. Se calculan cortes sobre la diagonal del merge usando busqueda binaria, y cada hebra fusiona secuencialmente un tramo independiente.

- PRAM:
  con `p` procesadores, el particionado cuesta `O(log n)` por procesador y luego cada procesador fusiona aproximadamente `n/p` elementos.
- Work:
  `W(n) = O(n)`
- Span:
  `S(n) = O(log n)` para el calculo de particiones, mas el costo local `O(n/p)` cuando se analiza con `p` procesadores.
- Brent:
  `T_p = O(n/p + log n)`

Partes naturalmente paralelizables:
- el calculo de particiones;
- el merge de tramos independientes.

Cuellos de botella:
- sincronizacion entre la fase de particionado y la fusion por bloques;
- sobrecosto cuando los subarreglos son pequenos.

## 4. Version paralela completa

La version completa combina:

- recursion paralela del k-way mergesort;
- merge por rondas entre bloques disjuntos;
- merge paralelo interno por particiones.

Si `k` es constante y el merge paralelo es work-efficient:

- Work:
  `W(n) = kW(n/k) + O(n) = O(n log n)`
- Span:
  cada nivel paga `O(log n)` por el merge paralelo y hay `O(log_k n)` niveles, por lo que
  `S(n) = O(log^2 n)` para `k` constante.
- Brent:
  `T_p = O((n log n)/p + log^2 n)`

Partes naturalmente paralelizables:
- ordenamiento de los subbloques;
- merges disjuntos dentro de cada ronda;
- fusion interna de cada par usando particiones.

Factores que limitan la escalabilidad:
- creacion de tareas demasiado pequenas;
- desbalance cuando `k` no divide bien a `n`;
- costos de memoria temporal y sincronizacion en los niveles altos.

## 5. Granularidad

La granularidad es clave para que OpenMP no desperdicie tiempo creando tareas pequenas.

- Si la subinstancia tiene tamano menor o igual a `granularity`, el codigo cae a una version secuencial.
- Esto reduce el overhead de tareas y mejora la estabilidad experimental.
- Conviene reportar en el informe que el valor de granularidad tambien es un parametro del experimento.
