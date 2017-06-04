set -x -e
ocamlbuild -lib unix main.native
cat sample.txt | .build/src/main.native
