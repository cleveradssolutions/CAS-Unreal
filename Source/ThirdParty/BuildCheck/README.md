# Checking the module rules without an engine

`CleverAdsSolutions.Build.cs` is compiled by Unreal Build Tool, so a mistake in it is
normally found by a build, on a machine that has an engine installed. This directory
compiles it on its own, against stand-ins for the Unreal Build Tool API, and runs the
parts of it that can run outside a build:

```bash
brew install mono
./check.sh                                   # compiles both engine branches
./check.sh ../IOS/CASXCConfig.temp           # and reads that config as a build would
```

Both branches are compiled: Unreal Engine 5, which keeps the core types in
`EpicGames.Core` and reads the config as XML, and Unreal Engine 4, which keeps them in
`Tools.DotNETCommon` and reads it as JSON.

`ApplyConfig` runs on every invocation: it applies a config to a module and checks what
the build would link, for the cases that decide it. The tracking framework is the one that
needs deciding, because the SDK asks for the tracking authorization by class name and the
framework has to be loaded for that lookup to find anything.

With a config given, two more things run for real:

- `ReadConfig` reads it with `XCodeConfig.Read` and prints what the build would see,
  which is how the writer of the config and this reader are kept in step. The JSON stand-in
  maps fields by name the way the engine does, and says so when a field has no counterpart.
- `ReuseConfig` asks `IsSetupReady` whether a build would reuse what was prepared, for the
  cases that decide it: the same selection, one adapter more, a different adapter, a newer
  SDK, and the frameworks deleted. The first has to be reused and the rest prepared again.

**What a clean run does not mean.** The stubs here are an approximation of the Unreal
Build Tool API, written from how the module uses it. They catch mistakes in the module's
own code and they exercise its config handling, but they cannot prove that an engine
accepts the module. Only a real build does that.
