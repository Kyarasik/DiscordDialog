# ПЕРЕД ИСПОЛЬЗОВАНИЕМ БОТА ВНИМАТЕЛЬНО ПРОЧИТАЙТЕ РУКОВОДСТВО

HTTP-запросы с Discord обрабатываются библиотекой Sleepy-discord (yourWaifu). Руководство по установке всех нужных файлов для работы сервера: https://github.com/yourWaifu/sleepy-discord/tree/master?tab=readme-ov-file#requirements. Также на официальной странице библиотеки вы можете найти синтаксис данной библиотеки для своих проектов. Данный проект разчитан на macOS устройства.

## КАК СОБРАТЬ ПРОЕКТ:
Убедитесь, что все нужные зависимости для Sleepy Discord и Git у вас установлены.

1. В терминале вашего компьютера (или компилятора) выберите папку, где хотите, чтобы находился ваш исходный проект, после чего введите: ```git clone https://github.com/Kyarasik/DiscordDialog```

2. Зайдите в ваш проект в любом текстовом редакторе кода.

3. Перейдите в файл source.cpp, замените строки ```YOUR_BOT_TOKEN```, ```YOUR_BOT_ID```, ```YOUR_BOT_CHANNEL_ID```.

4. В корневой папке проекта создайте папку ```deps```. В терминале вашего компьютера (или компилятора) перейдите в папку deps, на macOS это можно сделать командой ```cd путь_до_папки``` (если у вас macOS).

5. Введите команду ```git clone https://github.com/yourWaifu/sleepy-discord```.

6. Вернитесь в папку проекта командой ```cd ..``` (если у вас macOS).

7. В терминале вашего компьютера (или компилятора) перейдите в папку build, на macOS это можно сделать командой ```cd путь_до_папки```.

8. Введите команду ```cmake путь_до_вашего_проекта```, должен начаться длительный процесс установки сборки.

9. Обновите исполняемый файл до последней версии командой ```make``` (на других платформах команда может отличаться).

10. После чего введите в консоль ```./DiscordDialog```, после чего в терминале будет сообщение о успешном подключении. После этого ваш бот будет работать согласно данному коду.

## БОНУС:
Если вы используете любую команду бота, то в течение следующих 24 часов вы сможете получить бейдж активного разработчика в Discord на портале разработчиков Discord.

## СОТРУДНИЧЕСТВО:
Если у вас остались какие-либо вопросы или вы хотите со мной связаться, предоставляю вам мой Discord аккаунт: ```k.y.a.r.a.s.i.k```.