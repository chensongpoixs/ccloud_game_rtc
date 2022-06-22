# inject


##  linux

> sudo apt-get install clang -y 


### build

> make arm
> make x86 
> make x86_64 


### run test 

#### run sample
> ./sample-target

#### inject pid

> sudo ./inject -p 3961  sample-library.so

#### inject sample 

> sudo ./inject -n sample-target  sample-library.so