{
  description = "Compass and straightedge constructor";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
  {

    packages.${system}.default = pkgs.clangStdenv.mkDerivation {
      name = "compass-and-straightedge";
      src = ./.;

      nativeBuildInputs = with pkgs; [
        clang-tools

        cmake
        ninja

        m4
        makeWrapper
      ];

      buildInputs = with pkgs; [ glew glfw3 ];

      cmakeFlags = [ "-DCMAKE_BUILD_TYPE=Release" ];

      postFixup = ''
        cp -r $src/res $out
        wrapProgram $out/bin/compass-and-straightedge --set GLNIMAN_RES_HOME $out/res
      '';
    };

  };
}
