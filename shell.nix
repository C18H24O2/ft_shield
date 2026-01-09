{
  pkgs ? import <nixpkgs> { },
}:

let
  llvmPkgs = pkgs.llvmPackages_21;
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
      jq
      bear
      patchutils

      util-linux.dev
      util-linux.lib
      xorg.libX11.dev
      python313

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
