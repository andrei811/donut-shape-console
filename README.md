# Rotating character donut in C++

## Prerequisites
   * MinGW complier

## How to run the code on Windows?

* Download the project in your computer
##### Open cmd, then type and replace "downloadPath" with the desired download path
```
cd "downloadPath"
git clone https://github.com/andrei811/donut-shape-console.git
```

* Go to "code" folder, inside the project
```
    cd "downloadPath"\src\code
```

* Type in cmd in order to compile the files:
```
g++ *.cpp -o donut
```

* Then, execute the program
    The `-a` (axis) argument takes after it `x` or `y` or `z` or `xyz`
    By default, the standard rotation axis is `xyz` 

    Thus, you can execute as that
    ```
    donut -a x
    ```
    for `x`-rotation, or as that
    ```
    donut
    ```
    for a `xyz`-rotation

