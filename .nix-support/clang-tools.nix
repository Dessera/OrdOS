{
  stdenv,
  lib,
  runCommand,
  makeWrapper,
  llvmPackages,
  extraQueryDriver ? "",
}:
let
  queryDriver = lib.concatStringsSep "," [
    "${stdenv.cc}/bin/*"
    extraQueryDriver
  ];

  nextPath =
    path:
    let
      subNext = lib.lists.head (lib.attrNames (builtins.readDir path));
    in
    "${path}/${subNext}";

  buildGccInternalInc =
    stdenv:
    let
      baseLib = "${stdenv.cc.cc}/lib/gcc";
    in
    # arch and version
    # because of the inconsistency of derivation version of gcc, there is
    # no way to get the version here
    "${nextPath (nextPath baseLib)}/include";

  buildClangInternalInc = stdenv: "${stdenv.cc}/resource-root/include";

  # once https://github.com/clangd/clangd/issues/2074 is fixed,
  # we can remove these function and use the built-in include path
  buildInternalInc =
    stdenv:
    if stdenv.cc.isGNU then
      buildGccInternalInc stdenv
    else if stdenv.cc.isClang then
      buildClangInternalInc stdenv
    else
      "";
in
runCommand "clang-tools"
  {
    unwrapped = llvmPackages.clang-unwrapped;
    buildInputs = [ makeWrapper ];
  }
  ''
    mkdir -p $out/bin

    for tool in $unwrapped/bin/clang-*; do
      tool=$(basename "$tool")

      if [[ $tool == "clang-cl" || $tool == "clang-cpp" ]]; then
        continue
      fi

      if [[ ! $tool =~ ^clang\-[a-zA-Z_\-]+$ ]]; then
        continue
      fi

      ln -s $unwrapped/bin/$tool $out/bin/$tool
    done

    makeWrapper $unwrapped/bin/clangd $out/bin/clangd \
      --add-flags "--query-driver=${queryDriver}"     \
      --set LANG "en_US.UTF-8"                        \
      --set CPATH "${buildInternalInc stdenv}"
  ''
