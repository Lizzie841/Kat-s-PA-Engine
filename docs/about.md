## About Project Alice

### Getting Started
Please consult docs/contributing.md for more information! it contains information on how to compile the project on both Windows AND Linux

### Picking up where Open V2 left off

The goal of Project Alice (named after [Alice Roosevelt Longworth](https://en.wikipedia.org/wiki/Alice_Roosevelt_Longworth)) is essentially to create a new version of [open v2](https://github.com/schombert/Open-V2) -- my earlier project to create a Victoria 2 clone -- and then to finish a working, feature complete, version of the game. This means that, at least initially, there will be few departures from doing things as Victoria 2 did them, simply to keep the project focused and on track. Once a 1.0 is complete, we can then use it as a playground for new experiments. As of July 2023, I have basically caught up to where open v2 was, minus some deficiencies in the ui.

I hope to find a few people to form an art team that can recreate the assets that Victoria 2 uses to give this project its own distinctive visual identity. I know that is a big ask, but on the other hand, there will be no 3d modeling required. We already have access to a set of new flags that we can use, courtesy of the SOE project.

### In comparison to other projects

In comparison to SOE (Symphony of Empires) this project is a more direct Victoria 2 clone, while SOE is its own game. Nevertheless, I tend to think of it as a sister project. We do have some people here involved with both (I myself make the occasional suggestion there), and if you see something that we make that they could use, I am sure that they would appreciate the contribution. Likewise, we will be using at least some of their work.

With regards to the OpenVic2 project, although it "marketed" as a Victoria 2 "clone," that isn't what they are working towards creating. Instead, they intend to provide an entirely new set of assets, including events, decisions, etc., which will make their project more of a Victoria 2-ish game. It is also run with a very different managerial style. If you are interested in both Project Alice and OpenVic2, I suggest spending time with both teams or even joining both projects.

### Licensing

All code produced as part of this project will be released under the GPL3 license (as was open v2). The license for art assets will be up to the art team, but I would prefer some form of Creative Commons.

### Programming tools

The project itself uses CMake (yes, I know that we all hate CMake). Ideally it should be able to be compiled with any of the big three compilers: MSVC, Clang/LLVM, or GCC. I personally tend to use MSVC, but I also use Clang on windows. Making sure that things keep working in GCC will have to be someone else's problem, and if no contributor turns out to be a regular GCC user, then GCC compatibility may disappear from this list.

Personally I use visual studio 2022 (the free, community edition) to work on the project, but if you are contributing, you should be able to use whatever tools you like, including VS Code, CLion, EMACS, VI, etc

### Where will Project Alice diverge from Victoria 2?

Initially, Project Alice will imitate the Victoria 2 mechanics extremely closely wherever we can, with a few minor exceptions. Yes, there are lots of things we could improve. I went into open v2 with the mindset that I would just improve a few things here and there as I went, and that was a major reason that the project got sidetracked and ultimately abandoned. I don't want to make that mistake again, so this time I am going to try as hard as possible to resist the urge to make improvements until we have a working, feature complete, game. That said, there are a few changes we have to make no matter what. The globe map is simply too fun not to put in. And there are are few QOL improvements (like hyperlinks that send you to what an event is talking about or a summary of why an event was triggered) that are easy enough and nice enough that it would be a real shame to leave them out. But no major adjustments to the mechanics this time! I promise!
