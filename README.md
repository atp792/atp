##
Состоит из двух программ - generator и sorter.
Для сборки используется cmake. 


## Generator
Используется для генерации тестового файла. Создает файл `output_filename` с размером прибизительно равным `approximate_size_in_bytes`. Запуск:
```
generator output_filename approximate_size_in_bytes
```

## Sorter
Используется для сортировки файла. Для сортировки файла на 100 Гб потребует порядка 10 Гб оперативной памяти и 300 Гб места.  
Сортирует файл `input_filename` записывая результат в `output_filename`. В ходе работы создает `intermediate_filename`. Если не указать путь до `intermediate_filename`, то данный временный файл будет создан в текущей рабочей директории программы с именем `intermediate_filename.txt`. Запуск:
```
sorter input_filename output_filename [intermediate_filename]
```

## Время работы sorter
Замеры производились на intel core i7-7500U, 16 Гб Ram, ssd samsung mzvlb1t0halr-000l7.  
100 Гб файл - 17 минут  
10 Гб файл - 85 секунд  
