# Прогноз цен на криптовалюты




Для запуска требуется, чтобы был конфигурационный файл с основными параметрами модели: 
### Конфигурационный файл

Можно выбрать ключевые параметры, которые будут использоваться в модели. 

```
{
  "frequency": "day",
  "max_iterations": 20,
  "latent_dimension": 10,
  "percentage_using_data": 90,
  "number_input_files": 200,
  "ARLags": 5,
  "lambda_F": 1.5,
  "lambda_x": 1.5,
  "lambda_w": 1.5,
  "necessary_columns": [1, 2, 3, 4],
  "predict_periods": 30
}
```

### Файлы с данными

Файлы с данными должны лежать в папке data по соседуству с папкой  **crypto_predictor**, в неизменном виде. 

### Ключевые файлы в модели

* [main.cpp](crypto_predictor/main.cpp) - Основной файл, который стартует модель
* [model.h](crypto_predictor/model.h) - Файл с заголовками, который описывает структуру модели
* [json.hpp](crypto_predictor/json.hpp) - Внешняя библиотека для обработки json
* [AR.cpp](crypto_predictor/AR.cpp) - Файл с методами для AR-модели
* [initialize.cpp](crypto_predictor/initialize.cpp) - Набор методов для инициализации модели
* [train.cpp](crypto_predictor/train.cpp) - Набор методов для обучения модели
* [predict.cpp](crypto_predictor/predict.cpp) - Файл с методами для предсказывания значения на период
* [predict.cpp](crypto_predictor/predict.cpp) - Файл с методами для предсказывания значения на период
* [usefull_functions.cpp](crypto_predictor/usefull_functions.cpp) - Файл с прочими полезными функциями, которые требуется по ходу дела
* [matrix.csv](crypto_predictor/matrix.csv) - Матрица с исходными значениями, объединенная в единую таблицу
* [mask.csv](crypto_predictor/mask.csv) - Маска с исходными значениям, опоказывающая наличие или пропуск определенного элемента
* [prediction.csv](crypto_predictor/prediction.csv) - Значение предсказанных элементов на будущие периоды
* [timestamps.csv](crypto_predictor/timestamps.csv) - Задействованные при построении модели значения временного ряда=




### Команда для запуска

```
g++ -std=c++14 AR.cpp initialize.cpp main.cpp predict.cpp train.cpp usefull_functions.cpp 
./a.out
```

Таким образом запустится обучение на приложенных данных и на заданном конфиге


### Небольшой отчет

[report](https://github.com/dima9703/crypto_currency/blob/master/report.ipynb) - здесь есть небольшой приложенный отчет об алгоритме
