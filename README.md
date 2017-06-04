# Reason starter bot for the Riddles.io Light Riders competition.

This is a port of the [OCaml starter bot by smiley1983](https://github.com/smiley1983/lightriders-ocaml-starter) using
Facebook's [Reason](https://facebook.github.io/reason/index.html) syntax.

See starapple.riddles.io/competitions/light-riders for details of the game
--------------------------------------------------------------------------

## Getting started

The provided code is a minimal bot which makes random legal moves.

As seen in `main.re`, your bot should provide a function which takes a
`game_state` as its argument, and calls `issue_order` once. You then provide
this function as an argument to the `Game.run_bot` function.

The type definition for `game_state` can be found in  `td.re`.

## Setup

These instructions assume [OCaml](https://ocaml.org/docs/install.html) and [OPAM](https://opam.ocaml.org/doc/Install.html) have been installed.

Installation instructions for the Reason toolchain can be found at https://facebook.github.io/reason/nativeWorkflow.html.

The short version is as follows:

```
opam switch 4.03.0
eval $(opam config env)
opam install ocamlfind.1.7.1 reason merlin
```

## Compilation

Simply use `rebuild` instead of `ocamlbuild`:

```
rebuild -lib unix -I src main.native
```
