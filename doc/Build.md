# Dependency:
libcap, libseccomp, libpthread.      
# Build:
```
git clone https://github.com/Moe-hacker/ruri
cd ruri
make static_config
make
sudo cp ruri /usr/bin/ruri
```
# NOTE:
The test script has a part that must be run with `sudo`, `DO NOT` run `make test` on your devices!!!!      
# Build options:
```
make config        # configure
make static_config # static build

#### Only For Developers ####

make dev_config    # dev build
make dbg_config    # dev build with debug log
```
After running one of these command, you can now `make`.      
Note that `-j` option will not have any effect since ruri use NOTPARALLEL flag.
# Customize your build:
You can also use configure script to customize the build.      
```
Usage: ./configure [OPTION]...
    -h, --help             show help
    -s, --static           compile static binary
    -d, --dev              compile dev version
    -D, --debug            compile with debug log
    -c, --core             compile only core module, will not include libseccomp and libcap
    --disable-libseccomp   disable libseccomp
    --disable-libcap       disable libcap
    --disable-rurienv      disable .rurienv
```
Note: `--core` will auto enable `--disable-libseccomp --disable-libcap --disable-rurienv`      
## Build Debian package: 
Debian do not like static binary, so in debian package, ruri is dynamically linked.      
You can run:       
```sh
apt update
apt build-dep . -y
dpkg-buildpackage -b -us -uc -d
```
This will bulid the debian package.      