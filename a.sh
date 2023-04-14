#!/bin/bash
for i in {1..100}
do
    # Generar un tamaño aleatorio entre 10KB y 1MB
    size=$(shuf -i 10000-1000000 -n 1)

    # Generar contenido aleatorio y guardarlo en un archivo
    dd if=/dev/urandom of=files/file-$i bs=$size count=1

    echo "Archivo $i generado con un tamaño de $size bytes"
done

for i in {0..15}
do
    dd if=/dev/urandom of=files/file-$(($i*7))-bigchungus bs=500000000 count=1
done


