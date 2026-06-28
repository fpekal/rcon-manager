{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs = inputs @ {flake-parts, ...}:
    flake-parts.lib.mkFlake {inherit inputs;} {
      systems = ["x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin"];

      perSystem = {
        pkgs,
        self',
        ...
      }: {
        packages.default = pkgs.callPackage ./default.nix {};
        apps.default = {
          program = "${self'.packages.default}/bin/rcon-manager";
          type = "app";
        };
      };
    };
}
