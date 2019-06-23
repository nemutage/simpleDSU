# simpleDSU

simple Dynamic Software Updating can update the program without exiting it.
This is implemented to work on Ubuntu 16.04 (64bit) without using external libraries.


![result](https://github.com/nemutage/simpleDSU/blob/media/simpleDSU.gif)


## Usage
how to update function 'hoge' in 'target' using 'patch'.

### Build main

```shell
$ make
```

### Compile target.c and patch.c 

```shell
$ gcc ./target/target.c -o ./target/target
$ gcc -fPIC -c ./patch/patch.c -o ./patch/patch.o
```

### Run target

```shell
$ ./target/target
```

### Update target

```shell
$ sudo ./main [target PID] ./target/target ./patch/patch.o hoge
```


