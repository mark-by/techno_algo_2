# Алгоритмы и структуры данных. Модуль 2
## Задача 1. Хеш-таблица
Реализуйте структуру данных типа “множество строк” на основе динамической хеш-таблицы с открытой адресацией. Хранимые строки непустые и состоят из строчных латинских букв.  
Хеш-функция строки должна быть реализована с помощью вычисления значения многочлена методом Горнера.  
Начальный размер таблицы должен быть равным 8-ми. Перехеширование выполняйте при добавлении элементов в случае, когда коэффициент заполнения таблицы достигает 3/4.  
Структура данных должна поддерживать операции добавления строки в множество, удаления строки из множества и проверки принадлежности данной строки множеству.  

**Для разрешения коллизий используйте двойное хеширование.**  
**Требования**: В таблице запрещено хранение указателей на описатель элемента.  
Формат входных данных  
Каждая строка входных данных задает одну операцию над множеством. Запись операции состоит из типа операции и следующей за ним через пробел строки, над которой проводится операция.  
Тип операции  – один из трех символов:  
    +  означает добавление данной строки в множество;   
    -  означает удаление  строки из множества;  
    ?  означает проверку принадлежности данной строки множеству.  
При добавлении элемента в множество НЕ ГАРАНТИРУЕТСЯ, что он отсутствует в этом множестве. При удалении элемента из множества НЕ ГАРАНТИРУЕТСЯ, что он присутствует в этом множестве.  
Формат выходных данных  
Программа должна вывести для каждой операции одну из двух строк OK или FAIL, в зависимости от того, встречается ли данное слово в нашем множестве.  
```
stdin
+ hello
+ bye
? bye
+ bye
- bye
? bye
? hello

stdout
OK
OK
OK
FAIL
OK
FAIL
OK
```
## Задача 2. Порядок обхода (4 балла)
Дано число N < 106 и последовательность целых чисел из [-231..231] длиной N.  
Требуется построить бинарное дерево, заданное наивным порядком вставки.  
Т.е., при добавлении очередного числа K в дерево с корнем root, если root→Key ≤ K, то узел K добавляется в правое поддерево root; иначе в левое поддерево root.  
**Требования: Рекурсия запрещена. Решение должно поддерживать передачу функции сравнения снаружи.**  
**Выведите элементы в порядке in-order (слева направо).**  
```
in          out
3
2 1 3       1 2 3
3
1 2 3       1 2 3
3
3 1 2       1 2 3
```

