## Задание
Задание 05 (Сигналы).
Написать программу, бесконечно выводящую в файл 1 раз в секунду значение внутреннего счетчика (1 2 3 ...). Программа должна завершаться после третьего сигнала SIGINT. При получении сигнала SIGINT или SIGQUIT в файл записывается строка, что получен и обработан сигнал (использовать одну функцию-обработчик сигнала). Не забывайте закрывать
файл!  
Запустить программу в фоновом режиме (myprogram &). Узнать идентификатор процесса и протестировать команды управления (SIGINT, SIGQUIT, SIGABRT, SIGKILL, SIGTERM, SIGTSTP, SIGSTOP, SIGCONT). Для сдачи задания нужно прислать исходный код программы и скриншоты с комментариями (что тестируется, какое результат вы ожидаете и что фактически получилось).

## Код для проверки
```
# SIGINT (Ctrl+C)
kill -SIGINT <PID>

# SIGQUIT (Ctrl+\)
kill -SIGQUIT <PID>

# SIGABRT
kill -SIGABRT <PID>

# SIGTERM
kill -SIGTERM <PID>

# SIGTSTP (Ctrl+Z)
kill -SIGTSTP <PID>

# SIGSTOP
kill -SIGSTOP <PID>

# SIGCONT
kill -SIGCONT <PID>

# SIGKILL
kill -SIGKILL <PID> 
```

## Тестируем

Код для компиляции и запуска:
```
gcc -o myprogram program.c
./myprogram &
```

Запустим программу (PID будет меняться т.к. программа останавливалась и запускалась снова во время тестирования)  
![start_program](images/start_program.png)

### Ожидаемые и получившиеся результаты тестирования

* SIGINT (2): Будет обработан, записано сообщение в файл. После 3-го сигнала программа завершится.  
![sigint_1](images/sigint_1.png)  
![sigint_3](images/sigint_3.png)  

* SIGQUIT: Будет обработан, записано сообщение в файл, но не повлияет на счетчик SIGINT.  
![sigquit](images/sigquit.png) 


* SIGABRT: Не обрабатывается явно, поэтому завершит программу.  
![sigarbt](images/sigarbt.png)

* SIGTERM: Не обрабатывается явно, поэтому завершит программу.  
![sigterm_terminal](images/sigterm_terminal.png)  
![sigterm_program](images/sigterm_program.png)  

* SIGTSTP: Игнорируется программой (но вообще останавливает процесс).  

> Ничего не произошло

* SIGSTOP: Немедленно приостанавливает процесс (нельзя перехватить).  
* SIGCONT: Возобновляет приостановленный процесс.  
![sigstopcont](images/sigstopcont.png)    

> Программа была остановлена в 21:32:15 и продолжила работать в 21:32:58

* SIGKILL: Немедленно убивает процесс (нельзя перехватить или игнорировать).  
![sigkill](images/sigkill.png)  
