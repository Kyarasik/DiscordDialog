#!/bin/bash
# Перемещаемся в директорию build
cd /app/build
# Убедимся, что исполняемый файл имеет права на выполнение
chmod +x ./minion
# Запускаем бот
./minion
