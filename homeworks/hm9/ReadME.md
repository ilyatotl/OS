Алгоритм решение: каждый из процессов по очереди считывает сообщение из пайпа и записывает его обратно (первым запись делает
родительский поток). Синхронизация осуществляется с помощью двух семафоров. Когда родительский процесс делает запись в пайп, он обнуляет свой семафор, и увеличивает на 1 семафор ребенка. Когда дочерний процесс делает запись, он обнуляет свой семафор и увеличивает на 1 значение семафора родителя. В конце оба семафора удаляются функцией `semctl`.  
Пример работы: 
```bash
ilya@ilya-VivoBook-ASUSLaptop-X512JF-A512JF:~/Ilya/bash_practise/homeworks/hm9$ gcc main.c -o ./main
ilya@ilya-VivoBook-ASUSLaptop-X512JF-A512JF:~/Ilya/bash_practise/homeworks/hm9$ ./main 
Parent process write message
Child process read message
Child process write message
Parent process read message
Parent process write message
Child process read message
Child process write message
Parent process read message
Parent process write message
Child process read message
Child process write message
Parent process read message
Parent process write message
Child process read message
Child process write message
Parent process read message
Parent process write message
Child process read message
Child process write message
Parent process read message
Parent process write message
Child process read message
Child process write message
Parent process read message
Parent process write message
Child process read message
Child process write message
Parent process read message
Parent process write message
Child process read message
Child process write message
Parent process read message
Parent process write message
Child process read message
Child process write message
Parent process read message
Parent process write message
Child process read message

```