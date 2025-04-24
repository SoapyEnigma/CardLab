# Enigma's Card Lab

**Alpha release date: April 25th, 2025**

There is a download for a pre compiled version of this program so that you can skipp the build and compile process. On the side bar, click on `Releases`, then  `Assets`, and there is a file called `Alpha.Release.v0.10.250424a.zip`. Download that and unzip it.

* [What is this?](#What-is-this?)
* [OS Support](#OS-Support)
* [IDE Support](#IDE-Support)
* [Dependencies](#Dependencies)
* [How-To Build](#How---To-Build)
* [How-To Use](#How---To-Use)
* [Card Biology](#Card-Biology)
* [To-Do List](#To---Do-List)
* [Socials](#Socials)

## What is this?
This is the Enigma's Card Lab application. This tool is used to create custom cards for the mobile game Pokemon TCG Pocket. The goal of this tool is to provide users a quick and easy way to add and combine their own custom art for borders, illustrations, flairs, and etcetera to create new and unqiue cards that resemble and could pass for legitimate cards. The intent isn't to fool anyone, but to provide realistic concepts for everyone to theorycraft, enjoy, and share. 

## OS Support
- Windows

## IDE Support
- [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/)

## Dependencies
- [premake5](https://premake.github.io/download)

## How-To Build
1. Download the dependencies and IDE.
2. Fork and clone the repository.
3. Build the project using [premake5](https://premake.github.io/download). You do this by opening a terminal at the project folder location and enter and run the command `premake5 vs2022 `
4. Go to the new `Build` folder and open the CardLab.sln using [VS2022](https://visualstudio.microsoft.com/downloads/).
5. Compile the project in release mode (or debug if you're feeling silly).
6. The project will compile to `\Build\bin\x86_64\Release`.

## How-To Use
1. Go to the `Release` (or `Debug`) folder..
2. Put your custom illustrations into the `Custom/Illustrations/` folder.
3. Put your custom flairs (or fancy layers) into the `Custom/Flairs/` folder.
4. Run the application.

## Card Biology

## To-Do List:
- Add the x and + for attack damage.
- Add trainers.
- Reorganize/rewrite the entire program and get it out of main.cpp

## Socials
- [X/SoapyEnigma](https://x.com/SoapyEnigma)
- [Card Lab Discord](https://discord.gg/kFZ7WjSZQT)
- [Buy Me A Coffe](https://buymeacoffee.com/soapyenigma)
