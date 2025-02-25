# PZEM-004T-Point

Датчик PZEM-004T работает с OrangePi Zero 512 (Armbian).
Установка:
    cd /home
    git clone https://github.com/alxkey/PZEM-004T-Point.git
    cd PZEM-004T-Point
    mkdir files
    mkdir logs
    make
    make clean
Настройки в файле:
    working.ini
Данные в файлы в files и на сервер по UDP
Логи в logs и на syslog сервер
Запуск:
    ./app
Для запуска при старте системы:
    cp script /opt
    cd /opt
    chmod +x script
    nano /etc/rc.local
    добавить в конец файла: /opt/script
    перед exit 0 
При включении будет стартовать автоматом.
В файле working.ini:
    delay=30
интервал времени в секундах между опросами PZEM-004T.