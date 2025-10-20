{
  pkgs ? import <nixpkgs> { },
}:

let
  llvmPkgs = pkgs.llvmPackages_19;
  stdenv = pkgs.stdenvAdapters.useMoldLinker llvmPkgs.stdenv;
in
(pkgs.mkShell.override { inherit stdenv; }) {
  nativeBuildInputs =
    with pkgs;
    [
      nasm
      valgrind
      gdb
      # lldb
      strace
      ltrace
      bear
      patchutils

      cmake
      meson

      vagrant
      (pkgs.writeScriptBin "nuke-amd-kvm" ''
        #!${pkgs.stdenv.shell}
        set -euo pipefail
        sudo rmmod kvm-amd
        sudo rmmod kvm
      '')
    ]
    ++ (with llvmPkgs; [
      libllvm.dev
    ]);
  LLVM_DIR = "${llvmPkgs.llvm.dev}";
}
