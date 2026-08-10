<img alt="banner" src="https://github.com/user-attachments/assets/620a638e-5253-47e6-a0d8-76d49ada15ed" />

## Overview
**Blox** is a **New Super Mario Bros. U** actor pack that adds a collection of mostly block-related actors, aswell as some non-block-related actors for modders to use.

## Features
- **Triple Block**: It's exactly what it sounds like, a question block that's triple the width! But this time, the model doesn't crash! 😉
- **On/Off Switch**: It's a switch, that toggles an event, Can be used in conjunction with the On/Off Block, as it uses events.
- **On/Off block**: It's a block, that's (depending on the state of the event) either solid or not. Can be used in conjunction with the On/Off Switch, as it uses events.

### For Modders
- Download the **`bundle`** from the [latest release](https://github.com/Domthewiz/Blox/releases/latest) and extract it to your mod folder, merging the `content` and `code` folders into your project. The actors will now be available in-game.
    - The `rules.txt` doesn't matter as long as the `version = 8` in your own.
- Running on console: Use the [Telkin](https://github.com/Zenith-Team/Telkin) aroma plugin to load your whole mod.
    - Place the `code`/`content` folders in `sd:/wiiu/telkin/TITLEID/` where `TITLEID` is the [title ID](https://wiiubrew.org/wiki/Title_database#00050000:_Game_Application_Titles) of your game's region (without dashes).
- Running on Cemu: Load and distribute your mod as a GraphicPack by placing it in Cemu's `graphicPacks` folder and activating it in the game's settings.

> [!IMPORTANT]
> Make sure to also install the [editor patch](https://github.com/Domthewiz/Blox/tree/main/editor) so that you can place the actors in your levels!
