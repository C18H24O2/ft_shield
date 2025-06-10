{
  pkgs ? import <nixpkgs> { },
}:

let
  llvmPkgs = pkgs.llvmPackages_18;
  stdenv = pkgs.stdenvAdapters.useMoldLinker llvmPkgs.stdenv;
in
(pkgs.mkShell.override { inherit stdenv; }) {
  nativeBuildInputs =
    with pkgs;
    [
      nasm
      valgrind
      gdb
      lldb
      strace
      ltrace
      bear
      patchutils

      # obfuscator-llvm
      cmake
      meson
    ]
    ++ (with llvmPkgs; [
      libllvm.dev
    ]);
  LLVM_DIR = "${llvmPkgs.llvm.dev}";
}
