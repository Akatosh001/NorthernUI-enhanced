# NorthernUI

**All the work put into reverse engineering Oblivion and developing NorthernUI's code base was done by David Cobb (https://github.com/DavidJCobb/NorthernUI). This is a fork of his work designed to fix minor UI errors in the NorthernUI-away version (the vanilla controller interface). All credit and much thanks to David for his work on making Oblivion playable with a controller on PC.**

Source code for a DLL that modifies Oblivion's UI engine. The mod is currently available for distribution [on NexusMods](https://www.nexusmods.com/oblivion/mods/48577).

## Setup

Source code here depends on, but does not include, OBSE source files. Get those from their original source and fit them into the folder hierarchy in this project. Make sure this project's Visual Studio-specific stuff (e.g. solution files) overwrite those from OBSE. Everything's set up for Visual Studio Community 2015.

**UPDATE:** Turns out that when I was first starting out, I built against [an unofficial fork of OBSE](https://github.com/llde/Oblivion-Script-Extender) which is at version 0022, rather than the last official release which is version 0021. NorthernUI is compatible with the official release, but vanilla class definitions in version 0022 have names and definitions for functions that were unidentified in the version 0021 source code. I apologize for the error; when I next find time to work on NorthernUI, I will see if it is possible for me to replace v0022 with v0021 (replicating any function/class definitions as needed).

## Licensing
The code in the "obse_plugin_example" folder is mine unless otherwise stated; I retain whatever rights, permissions, etc., I have over it "naturally."

Everything in the "ReverseEngineered" folder constitutes information gathered while reverse-engineering Oblivion, using OBSE class definitions as a base for some of mine, and using OBSE offsets as a basis for exploration. As such, everything in that folder besides that class shall be considered as free and open as I'm allowed to make it; I place no limitations, restrictions, or requirements on the use of any code in the "ReverseEngineered" folder.

### Graphics assets

Assets in the "graphics" folder are my work, and are a mixture of final assets (used in the mod) and work-in-progress assets (retained for archival purposes). The loading screens are screenshots taken in-game while playing Oblivion, with subsequent retouching. Other assets are handmade pixel art mimicking the art style of Bethesda's other works.

Some older asset files, included for archival purposes, include text using the Calibri font, a Microsoft font; Microsoft places [no restrictions](https://docs.microsoft.com/en-us/typography/fonts/font-faq) on the use of that in computer graphics.

* "gamepad_keybind_ds_hd" XCF files
* DDS files in the "gamepad_hd_singles" folder

Some older asset files, included for archival purposes, include text using the [Roboto](https://github.com/googlefonts/roboto) font. Roboto uses the [Apache License 2.0](https://github.com/googlefonts/roboto/blob/main/LICENSE).

* "keybinds - rev" XCF files
* "keybind_special_no_key.dds"

Most of the original (32x32) map markers are trace-overs of Bethesda's original map marker graphics in Oblivion; some of the work-in-progress XCF files for them include the original Oblivion assets, which are wholly Bethesda's intellectual property. The map markers used in the final mod were upscaled by hand using these lower-resolution derivative works as a base.
