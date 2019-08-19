# qv (quick view): a viewer for images and image-like data

## Description

qv shows images and image-like data and provides some simple analysis tools.
It is useful for all kinds of 2D data from sensors, simulation, or rendering,
but also images, including high dynamic range images.

Start it from the command line and give it a list of files or directories to
open, then use keyboard and mouse to analyze the data. Each file can contain
multiple images, called frames in qv.

## Key bindings

### General

- Q: quit

- ESC: leave fullscreen mode or quit

- F11: toggle fullscreen mode

- l: toggle linear interpolation when magnifying

- g: toggle grid when magnifying

### Move and zoom image

- -, +: zoom out/in

- =: reset zoom

- space: reset mouse-based translation

### Navigate in files and frames

- <, >: switch to previous/next file

- left, right: go one frame backward/forward

- up, down: go ten frames backward/forward

- page up, page down: go 100 frames backward/forward

### Analysis tools

- i: toggle info overlay

- s: toggle statistics overlay

- h: toggle histogram overlay

- m: toggle color map overlay

### Choose the channel to view

- c: view color channel

- 0, 1, 2, 3, 4, 5, 6, 7, 8, 9: view this channel

### Select displayed value range

- (, ): shift selected range left/right

- [, ]: decrease/increase upper bound

- {, }: decrease/increase lower bound

- \: reset value range

### Choose color map

- F4: disable color map

- F5: use predefined sequential color map (press multiple times to cycle)

- F6: use predefined diverging color map (press multiple times to cycle)

- F7: use predefined qualitative color map

- F8: use custom color map from clipboard, as generated by 
      [gencolormap](https://marlam.de/gencolormap)
