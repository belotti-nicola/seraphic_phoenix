#!/usr/bin/bash

cd ../amalgamation/
python3 amalgamate.py

gcc main.c seraphic_phoenix_am.h seraphic_phoenix_am.c -o main.out
