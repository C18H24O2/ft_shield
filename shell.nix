{ pkgs ? import <nixpkgs> {} }:
let
  stdenv = pkgs.llvmPackages_20.stdenv;
in
(pkgs.mkShell.override { inherit stdenv; }) {
  nativeBuildInputs = with pkgs; [
    nasm
    valgrind
    gdb
    strace
    ltrace
  ];
}
