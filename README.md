# README

HTTP requests from Discord are processed by the Sleepy-discord library (```https://github.com/yourWaifu/sleepy-discord```) and translated into its unique syntax in C++. Installation guide for all necessary library dependencies: ```https://github.com/yourWaifu/sleepy-discord/tree/master?tab=readme-ov-file#requirements```. Git installation guide: ```https://github.com/git-guides/install-git```. You can also find syntax for your projects on the official library page. The guide is designed for macOS devices.

## HOW TO BUILD THE PROJECT:
Make sure all necessary dependencies for Sleepy Discord and Git are installed.

1. In your computer's terminal (or compiler), select the folder where you want your source project to be located, then enter: ```git clone https://github.com/Kyarasik/DiscordDialog```.

2. Open your project in any code text editor, then in your computer's terminal (or compiler), navigate to the project directory. On macOS, this can be done with the command ```cd path_to_project```.

3. Go to the source.cpp file, and replace the lines ```YOUR_BOT_TOKEN```, ```YOUR_BOT_ID```.

4. In the root folder of the project, create a folder called ```deps```. In your computer's terminal (or compiler), navigate to the deps folder. On macOS, this can be done with the command ```cd path_to_folder```.

5. Enter the command ```git clone https://github.com/yourWaifu/sleepy-discord```.

6. Return to the project folder with the command ```cd ..``` (if you are on macOS).

7. In your computer's terminal (or compiler), navigate to the build folder. On macOS, this can be done with the command ```cd path_to_folder```.

8. Enter the command ```cmake path_to_your_project```, a long build process should begin.

9. Update the executable file to the latest version with the command ```make``` (the command may differ on other platforms).

10. After that, enter the command ```./DiscordDialog``` in the console, and a message about a successful connection will appear in the terminal. After this, your bot will work according to the given code.

## HOW TO GET THE DISCORD ACTIVE DEVELOPER BADGE:
If you use any bot command, then within the next 24 hours you will be able to get the active developer badge on the Discord developer portal ```https://discord.com/developers/active-developer```.

## COLLABORATION:
If you have any questions or want to contact me, I provide my Discord account: ```k.y.a.r.a.s.i.k```.
