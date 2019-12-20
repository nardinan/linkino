THE LINKER (Geneva Game Developers - Dec 2019)
-------------------------

The linker is a quick'n dirty game written in a couple of weeks for the Geneva Game Developers Meetup of December 2019. In this game you are the so-called "expert", called to help a set of different companies with their networking problems.
You have a scenario, you have a budget and a set of tools you can buy to solve their problems. You can re-shape the network in real time and the system assigns you a score based on the number of packets (represented as e-mail) that arrived at destination, the broken packages (spam) at destination and a set of other non-well-calibrated criteria.

The game is *far* from being complete (there is only one level and doesn't even end). There is a long-long list of things I wanted to integrate, but I got bored during the development and I am waiting to reach once again the good amount of inspiration.

Moreover, due to the fact I were in a rush, I had to take some questionable decisions about the code architecture.

# Compile it

The game has been sucessfully compiled on Linux (Ubuntu) and MacOS (Catalina)

### Dependencies
The game depends on the following libraries:
 * libmiranda (https://github.com/nardinan/miranda)
 * libSDL2
 * libSDL2_ttf
 * libSDL2_mixer
 * libSDL2_image

### Compilation on Ubuntu (and possibly any other _Debian-based_ distribution)

Open a terminal and install the SDL2 libraries using **apt**:
```bash
$ sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
```

Now that all the required SDL2 packages have been installed, it's time for Miranda:
```bash
$ git clone https://github.com/nardinan/miranda
$ cd miranda
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```
Great! Miranda is now installed in your /usr/local/lib directory. **Be sure to add the corresponding entry "/usr/local/lib" to LD_LIBRARY_PATH (DYLD_LIBRARY_PATH on OSX).** After that, from your terminal, perform the following steps to build Pomodoro:
```bash
$ git clone https://github.com/nardinan/linkino
$ cd linkino
$ mkdir build
$ cmake ..
$ make
```

### Compilation on Mac OS X

On MacOS X, the installation procedure has been tested with Brew (https://brew.sh/). To install the SDL2 libraries via the *brew* command, type the following:
```bash
$ brew install sdl2 sdl2_image sdl2_ttf
$ brew install sdl2_mixer --with-flac --with-fluid-synth --with-libmikmod --with-mpg123 --HEAD
```
Once the libraries have been successfully installed, refer to the paragraph about the installation of Miranda and Linkino on the section 'Compilation on Ubuntu' above.
