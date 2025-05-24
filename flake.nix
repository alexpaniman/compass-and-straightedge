{
  description = "Compass and straightedge constructor";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    nixgl.url = "github:nix-community/nixGL";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, nixgl, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [ nixgl.overlay ];
          config.allowUnfree = true;
        };
      in
      {
        packages.default = pkgs.clangStdenv.mkDerivation {
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
            cp -r "$src/res" "$out"
  
            executable_name="compass-and-straightedge"
  
            wrapper="$out/bin/''${executable_name}"
            wrapped="$out/bin/.''${executable_name}-wrapped"
  
            mv "$wrapper" "$wrapped"
  
            echo "#! @shell@ -e"                                         > "$wrapper"
            echo "export GLNIMAN_RES_HOME='$out/res'"                   >> "$wrapper"
            echo "${pkgs.nixgl.auto.nixGLDefault}/bin/nixGL '$wrapped'" >> "$wrapper"
  
            chmod +x "$wrapper"
            substituteAllInPlace "$wrapper"
          '';
        };
      }
    );
}
