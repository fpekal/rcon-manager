{stdenv}:
stdenv.mkDerivation {
  pname = "rcon-manager";
  version = "1.0.0";

  src = ./.;
}
