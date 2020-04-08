Fonte: http://jakascorner.com/blog/2016/06/omp-for-scheduling.html
Resumo tipos de escalonamento

## `static`:

É mais apropriado quando todas as iterações tem o mesmo custo computacional.

#### Especificando o `chunk-size`

O OpenMP divide as iterações em `chunks` de tamanho `chunk-size` e atribui os grupos entre as threads de forma circular:

```
schedule(static, 4):   
****            ****            ****            ****            
    ****            ****            ****            ****        
        ****            ****            ****            ****    
            ****            ****            ****            ****
```

#### Sem especificar o `chunk-size`

O OpenMP divide as iterações em `chunks` de tamanho _aproximadamente_ igual e atribui um grupo para cada thread:

```
schedule(static):      
****************                                                
                ****************                                
                                ****************                
                                                ****************
```

## `dynamic`:

É mais apropriado quando as iterações possuem custos computacionais diferentes. Possui mais _overhead_ do que o `static`.

O OpenMP divide as iterações em `chunks` de tamanho `chunk-size` e atribui uma para cada thread. Quando a thread termina o processamento, ela solicita um chunk novo.

*Quando o `chunk-size` não é informado, o OpenMP usa `1` como default.

```
schedule(dynamic):     
*   ** **  * * *  *      *  *    **   *  *  * *       *  *   *  
  *       *     *    * *     * *   *    *        * *   *    *   
 *       *    *     * *   *   *     *  *       *  *  *  *  *   *
   *  *     *    * *    *  *    *    *    ** *  *   *     *   * 
```

```
schedule(dynamic, 4):  
            ****                    ****                    ****
****            ****    ****            ****        ****        
    ****            ****    ****            ****        ****    
        ****                    ****            ****            
```

## `guided`:

É mais apropriado quando as iterações possuem custos computacionais diferentes e as iterações finais tendem a ser mais custosas.

É basicamente um `dynamic`, mas com o `chunk-size` variável. O `chunk-size` é proporcional a quantidade de iterações faltando divido pela quantidade de threads (ou seja, diminui com o tempo).

A ideia é criar `chunks` maiores no início (diminuindo o `over-head`) e deixar `chunks` menores (com iterações mais custosas) para o final.

O `chunk-size` que você informa é, na verdade, o tamanho mínimo de um `chunk`. O `chunk` com a última iteração pode ser menor (ver o último exemplo).

*Quando o `chunk-size` não é informado, o OpenMP usa `1` como default.

```
schedule(guided):      
                            *********                        *  
                ************                     *******  ***   
                                     *******                   *
****************                            *****       **    * 
```

```
schedule(guided, 4):   
                                     *******                    
                ************                     ****    ****   
                            *********                           
****************                            *****    ****    ***
```

## `runtime`

Deixa a decisão para o sistema (em tempo de execução). 

Para definir o tipo de `scheduling`, podemos usar a variável de ambiente `OMP_SCHEDULE` ou a função `omp_set_schedule`.

## `auto`

Deixa a decisão para o compilador (em tempo de compilação) ou para o sistema (em tempo de execução).