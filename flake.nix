{
  description = "Logos Execution Zone Wallet UI - QML view + C++ backend module";

  inputs = {
    logos-module-builder.url = "github:logos-co/logos-module-builder/0.2.6";
    nix-bundle-lgx.url = "github:logos-co/nix-bundle-lgx";
    lez_core.url = "github:logos-blockchain/logos-execution-zone-module?rev=7dd1d29ced0971e24b05d978fb2b560b240d212f";
  };

  outputs = inputs@{ logos-module-builder, ... }:
    logos-module-builder.lib.mkLogosQmlModule {
      src = ./.;
      configFile = ./metadata.json;
      flakeInputs = inputs;
    };
}
