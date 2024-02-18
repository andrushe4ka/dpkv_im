# Имитатор датчика положения коленвала (ДПКВ)

На данный момент имитирует индуктивный ДПКВ в паре с задающим диском типа "60-2" (применяется на ВАЗ и др.)

# Использование

По при подаче питания на микроконтроллер устанавливаетя частота вращения КВ равная 600 об/мин. (период вращения 100 мс).

Команды принимаемые по UART:
- rNN - задание имитируемой частоты вращения КВ, где NN - сотни оборотов в минуту. Например, команда r08 задает частоту вращения 800 об/мин.
