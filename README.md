# simpleDSU

simple Dynamic Software Updating can update without exiting the program.
This is based on [livepatch](http://ukai.jp/Software/livepatch/). 
It is implemented to work on Ubuntu 16.04 (64bit) without using external libraries.


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


