{
  description = "linux develop env";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixpkgs-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";

    nixcode = {
      url = "github:Dessera/nixcode";
      inputs.flake-parts.follows = "flake-parts";
    };
  };

  nixConfig = {
    extra-substituters = [
      "https://nixcode.cachix.org"
    ];

    extra-trusted-public-keys = [
      "nixcode.cachix.org-1:6FvhF+vlN7gCzQ10JIKVldbG59VfYVzxhH/+KGHvMhw="
    ];
  };

  outputs =
    { flake-parts, ... }@inputs:
    flake-parts.lib.mkFlake { inherit inputs; } (
      { withSystem, ... }:
      {
        systems = [ "x86_64-linux" ];

        perSystem =
          {
            pkgs,
            system,
            ...
          }:
          let
            stdenv = pkgs.gcc14Stdenv;

            clang-tools = pkgs.callPackage ./.nix-support/clang-tools.nix {
              inherit stdenv;
            };

            code = withSystem system ({ inputs', ... }: inputs'.nixcode.packages.nixcode-cpp);
          in
          {
            devShells.default =
              pkgs.mkShell.override
                {
                  inherit stdenv;
                }
                {
                  packages =
                    (with pkgs; [
                      nixd
                      nixfmt-rfc-style
                      bear
                      gdb
                      qemu
                    ])
                    ++ [
                      clang-tools
                      code
                    ];
                };
          };
      }
    );
}
