cell
====
A simple (and bad) cellular automata written in C using SDL2 to render the display and handle keyboard input.

A matrix of cells is generated with varying ages and colors. During each update step, any alive cell can attack a neighboring cell of a different color (if # of neighbors > 1), run away (if # of neighbors = 1), or wander (if # of neighbors = 0). Any dead cell can be reborn as the offspring of the two brightest neighboring cells, if the two parents are within mating range color-wise. Each generation, there is a chance of genetic disease occurring, represented by a black square in drawn in the diseased cell. Diseased cells have a significantly shorter lifespan and serve as a measure to deter homogeny (since that's boring). This is also achieved by the color of offspring being a mixture of both parents' colors. This process repeats ad infinitum until you either pause the close the simulation.

Example
-------
![](demo.gif)

Usage
-----
0. Install SDL2 development packages (Ex. `sudo apt install libsdl2-dev`)
1. Compile using `make`
2. Run using `./cell <seed> <mate range> <friend range> <disease chance> <offspring color>`. Ex. `./cell 1337 64 32 1 1.1`
3. Use the 'P' key to toggle automatic playback and the spacebar to advance one step

Known Issues
------------
* Since I'm only using one matrix with no buffering, a cell can perform multiple actions in one step if it moves right or down.
* There are no comments in the source, because I'm too lazy. 😎
* Zero optimization and minimal cleanup was done, so don't be surprised when you see some of the worst C you've ever seen in your life. On the bright side, it is fully ANSI C compliant with rigorous use of `inttypes.h`.

License
-------

    Copyright (c) 2019 William Sengir <wsengir@asu.edu>

    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
