{
  description = "Logos Execution Zone Wallet UI - QML view + C++ backend module";

  inputs = {
    logos-module-builder.url = "github:3esmit/logos-module-builder?rev=324b459c3f7b59171d249f3ccbcc362403b3fcaf";
    nix-bundle-lgx.url = "github:logos-co/nix-bundle-lgx";
    lez_core.url = "github:3esmit/logos-execution-zone-module?rev=b8d353bd973fce7cd06b238f1a2362ede829c1df";
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
