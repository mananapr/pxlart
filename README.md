<h3 align="center"><img src="logo.png" height="100px"></h3>
<p align="center">A pixel art editor for the terminal written in C</p>

<p align="center">
<a href="https://github.com/mananapr/pxlart/releases/latest"><img src="https://img.shields.io/badge/Release-v1.4-blue.svg" alt="Latest release" /></a>
<a href="https://aur.archlinux.org/packages/pxlart"><img src="https://img.shields.io/badge/AUR-v1.4-blue.svg" alt="Arch Linux" /></a>
</p>

<p align="center">
<a href="https://github.com/mananapr/pxlart/blob/master/LICENSE"><img src="https://img.shields.io/badge/license-MIT-yellow.svg" alt="License" /></a>
</p>

<h3 align="center"><img src="scrot.png" width="61%"></h3>

`pxlart` is a pixel art editor written in C using the ncurses library that has vim like keybindings.
I got the idea from a bash script called [pxltrm](https://github.com/dylanaraps/pxltrm).

## Dependencies
- `ncursesw`

## Installation
- Clone the repo

`git clone --depth 1 https://github.com/mananapr/pxlart`

- Compile using `make`

`cd pxlart && make`

- Install

`sudo make install`

Arch Linux users can use the [AUR](https://aur.archlinux.org/packages/pxlart) package.

## Keybindings
| Key | Function |
|:---:| --- |
| <kbd>h j k l</kbd> | Navigation keys |
| <kbd>d</kbd>/<kbd>Space</kbd> | Draw |
| <kbd>e</kbd> | Erase |
| <kbd>b</kbd> | Change Brush |
| <kbd>c</kbd> | Set FG Color |
| <kbd>B</kbd> | Set BG Color |
| <kbd>s</kbd> | Save |
| <kbd>o</kbd> | Load |
| <kbd>q</kbd> | Quit |

## A note about colors
`pxlart` uses shell color numbers (0-255) for color input. You can use a script like [this](https://gist.github.com/MicahElliott/719710) to see what number corresponds to what color.

## Image Export
Saved projects can be exported to image formats using the mkppm utility
included with pxlart.

A saved project called "foo" will generate a file called "foo.data", which
can be passed into the mkppm utility to create "foo.ppm".

`mkppm foo.data foo.ppm`

The PPM file can then be converted to a PNG file using imagemagick:

`convert foo.ppm foo.png`

## TODO
- [x] Add ability to save and load
- [x] Add ability to export to image formats
- [x] Make PKGBUILD for AUR
- [x] Write a manpage
