{
  description = "yacpl devenv";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs";
  outputs =
    { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs {
        inherit system;
        config.allowUnfree = true;
      };
      # verify-helper 5.6.0 still uses pkg_resources, removed by setuptools 82.
      verifyHelper = pkgs.online-judge-verify-helper.overrideAttrs (old: {
        postPatch = (old.postPatch or "") + ''
          substituteInPlace onlinejudge_verify/documentation/main.py \
            --replace-fail 'import pkg_resources' 'import importlib.resources' \
            --replace-fail 'pkg_resources.resource_string(_RESOURCE_PACKAGE, _CONFIG_YML_PATH)' \
              'importlib.resources.files(_RESOURCE_PACKAGE).joinpath(_CONFIG_YML_PATH).read_bytes()'
          substituteInPlace onlinejudge_verify/documentation/build.py \
            --replace-fail 'import pkg_resources' 'import importlib.resources' \
            --replace-fail 'pkg_resources.resource_string(_RESOURCE_PACKAGE, path)' \
              'importlib.resources.files(_RESOURCE_PACKAGE).joinpath(path).read_bytes()'
        '';
      });
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        buildInputs = with pkgs; [
          gcc
          verifyHelper
        ];
      };
    };
}
