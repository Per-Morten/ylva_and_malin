Ylva and Malin is a side project created for me to toy with projects. It is a simple 2D RPG build with minimal library support, written in C.  
A development blog for the project can be found in the [wiki](https://github.com/Per-Morten/ylva_and_malin/wiki).

The motto of the project is:  
Will this be good? Probably not.  
Will it be fast? Probably not.  
Will I learn a lot? Hell yeah.  

# Can I play it?
Probably not. First of all, there isn't anything to play. Secondly, I am writing this for my own platforms, never checked it on any other platforms or anything, some places I might even be using my own absolute filepaths, which probably wont work elsewhere.  
However, you are welcome to try. It should be as easy as cloning the repo, running the ym_setup.sh script located in the setup folder, and executing the following command while standing in the repository root directory.

```sh
    ym_compile -o game -t release && ./build/game/release/bin/main
```
