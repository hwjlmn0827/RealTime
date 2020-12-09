gcc -lpthread -lm test_scheduler.c -o scheduler.out
./scheduler.out
python transfer.py
open output.html