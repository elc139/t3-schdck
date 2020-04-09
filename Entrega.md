Para mais informações sobre cada tipo de escalonamento, ver o arquivo [Resumo.md](Resumo.md).

### Resultados sem usar mutex

```
No mutex and schedule = runtime (expecting wrong results)
A--BA---AB--A--DA-CDA-B-AB--AB-DAB-DAB-DAB-DAB---B--AB--AB-DAB--AB-D-B-D-BCD-A--
A=17 B=16 C=2 D=10 

Same as previous (possibly getting different results due to scheduling)
B-D-BD--BDCABDC-ADC-ADCA-DCABDCABDCABDCABDC---C--B--BDCABDC-BD-CBD--BDC-BDC-B---
A=9 B=16 C=15 D=17 
```

### Resultados usando `static`

No caso do `static` o OpenMP divide as iterações em `chunks` de tamanho `chunk-size` e atribui os grupos entre as threads de forma circular.

Podemos ver que quando usamos o `chunk_size` padrão (o mesmo que não informar), ele cria `chunks` de tamanho igual e cada thread é responsável por exatamente um `chunk`.

```
Using mutex, schedule = omp_sched_static and chunk_size = default (expecting correct results)
AAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBCCCCCCCCCCCCCCCCCCCCDDDDDDDDDDDDDDDDDDDD
A=20 B=20 C=20 D=20 
```

Quando especificamos o `chunk_size` como `4`, ele distribui os `chunks` entre as threads de maneira circular (nesse caso, na ordem A -> B -> C -> D -> A ...)

```
Using mutex, schedule = omp_sched_static and chunk_size = 4 (expecting correct results)
AAAABBBBCCCCDDDDAAAABBBBCCCCDDDDAAAABBBBCCCCDDDDAAAABBBBCCCCDDDDAAAABBBBCCCCDDDD
A=20 B=20 C=20 D=20 
```

### Resultados usando `dynamic`

O OpenMP divide as iterações em `chunks` de tamanho `chunk-size` e atribui uma para cada thread. Quando a thread termina o processamento, ela solicita um chunk novo.

Como no caso do nosso programa as cargas de trabalho das iterações são iguais, espera-se que a ordem do início se repita até o final (e.g. se o programa começou na ordem ABCD ele provavelmente continuará nessa ordem até o final).

Quando não informamos o `chunk_size`, o OpenMP usa o default `1`.

```
Using mutex, schedule = omp_sched_dynamic and chunk_size = default (expecting correct results)
ABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCD
A=20 B=20 C=20 D=20 
```

Com o `chunk_size` settado em `4`, cada thread recebe 4 iterações por vez. Podemos ver que nesse caso a terceira thread (`C`) foi escalonada primeiro.

```
Using mutex, schedule = omp_sched_dynamic and chunk_size = 4 (expecting correct results)
CCCCBBBBAAAADDDDCCCCBBBBAAAADDDDCCCCBBBBAAAADDDDCCCCBBBBAAAADDDDCCCCBBBBAAAADDDD
A=20 B=20 C=20 D=20 
```

### Resultados usando `auto`

No caso do `auto`, onde o OpenMP deixa a decisão para o compilador (em tempo de compilação) ou para o sistema (em tempo de execução), meu computador usa o `static`. Como o `auto` ignora o `chunk_size`, ambas as saídas são iguais:

```
Using mutex, schedule = omp_sched_auto and chunk_size = default (expecting correct results)
AAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBCCCCCCCCCCCCCCCCCCCCDDDDDDDDDDDDDDDDDDDD
A=20 B=20 C=20 D=20 
```

```
Using mutex, schedule = omp_sched_auto and chunk_size = 4 (expecting correct results)
AAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBCCCCCCCCCCCCCCCCCCCCDDDDDDDDDDDDDDDDDDDD
A=20 B=20 C=20 D=20 
```

### Resultados usando `guided`:

O `guided` é basicamente um `dynamic`, mas com o `chunk-size` variável. O `chunk-size` é proporcional a quantidade de iterações faltando divido pela quantidade de threads (ou seja, diminui com o tempo).

A ideia é criar `chunks` maiores no início (diminuindo o `over-head`) e deixar `chunks` menores (com iterações mais custosas) para o final.

O `chunk-size` que você informa é, na verdade, o tamanho mínimo de um `chunk`. O `chunk` com a última iteração pode ser menor (ver o último exemplo).

Quando não informamos o `chunk_size`, o OpenMP usa o default `1`. Podemos ver que ele começa usando chunks de tamanho 20 e termina usando chunks de tamanho 1.

```
Using mutex, schedule = omp_sched_guided and chunk_size = default (expecting correct results)
DDDDDDDDDDDDDDDDDDDDAAAAAAAAAAAAAAABBBBBBBBBBBBCCCCCCCCCDDDDDDAAAAABBBBCCCDDABCD
A=21 B=17 C=13 D=29 
```

No caso em que setamos o `(min_)chunk_size` para `4`, podemos ver que ele começa usando chunks de tamanho 20 e termina usando chunks de tamanho 4 (menos no último caso, onde não há 4 iterações a serem processadas).

```
Using mutex, schedule = omp_sched_guided and chunk_size = 4 (expecting correct results)
CCCCCCCCCCCCCCCCCCCCDDDDDDDDDDDDDDDAAAAAAAAAAAABBBBBBBBBCCCCCCDDDDDAAAABBBBCCCCD
A=16 B=13 C=30 D=21 
```

### Usando runtime sem chamar `omp_set_schedule`

No caso do meu sistema, o algoritmo usado foi o `dynamic`

```
Using mutex and schedule = runtime (expecting correct results)
BCADBCADBCADBCADBCADBCADBCADBCADBCADBCADBCADBCADBCADBCADBCADBCADBCADBCADBCADBCAD
A=20 B=20 C=20 D=20 
```