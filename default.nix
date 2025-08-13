{
  stdenv,
  lib,

  # Build tools
  meson,
  ninja,
  xorriso,
  grub2,
}:
stdenv.mkDerivation {
  pname = "temail";
  version = "0.1.0";
  src = lib.cleanSource ./.;

  nativeBuildInputs = [
    meson
    ninja
    xorriso
    grub2
  ];
}
