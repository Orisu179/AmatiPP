{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {

    nativeBuildInputs = with pkgs; [
      gnumake
      cmake
      pkg-config
    ];

    buildInputs = with pkgs; [
      xorg.libX11.dev
      xorg.libXft
      xorg.libXinerama
      xorg.libXrandr
      xorg.libXext
      xorg.libXcursor
      alsa-lib
      faust
    ];

  }
