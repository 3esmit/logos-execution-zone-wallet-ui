{
  description = "Logos Execution Zone Wallet UI - QML view + C++ backend module";

  inputs = {
    logos-module-builder.url = "github:3esmit/logos-module-builder?rev=324b459c3f7b59171d249f3ccbcc362403b3fcaf";
    nix-bundle-lgx.url = "github:logos-co/nix-bundle-lgx";
    lez_core.url = "github:3esmit/logos-execution-zone-module?rev=4053564fe5dff0cd568840b2d7f52cbf37cdfb92";
  };

  outputs = inputs@{ logos-module-builder, ... }:
  let
    module = logos-module-builder.lib.mkLogosQmlModule {
      src = ./.;
      configFile = ./metadata.json;
      flakeInputs = inputs;
    };
    unitTests = logos-module-builder.lib.mkLogosModuleTests {
      src = ./.;
      testDir = ./tests;
    };
  in
    module // {
      checks = module.checks // unitTests;
    };
}
